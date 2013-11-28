/*
  Copyright (c) 2012-2013 DataLab, S.L. <http://www.datalab.es>

  This file is part of the gfsys library for GlusterFS.

  The gfsys library for GlusterFS is free software: you can redistribute
  it and/or modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  The gfsys library for GlusterFS is distributed in the hope that it will
  be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with the gfsys library for GlusterFS. If not, see
  <http://www.gnu.org/licenses/>.
*/

#include "gfsys.h"

#define SYS_CALLS_NULL  ((sys_callback_f)(intptr_t)0)
#define SYS_CALLS_SWAP  ((sys_callback_f)(intptr_t)1)
#define SYS_CALLS_END   ((sys_callback_f)(intptr_t)2)
#define SYS_CALLS_EMPTY ((sys_callback_f)(intptr_t)3)

#define SYS_CALLS_INC ((CACHE_LINE_SIZE / sizeof(void *)) + 1)

#define SYS_CALLS_NOTIFIED (~0xFFFF)

static sys_calls_config_t sys_calls_config = { 0, };
static uint32_t sys_calls_used = 0;

void sys_calls_config_prepare(sys_calls_config_t * config)
{
    if (config->size < SYS_CALLS_MIN_SIZE)
    {
        config->size = SYS_CALLS_MIN_SIZE;
    }
    config->size = sys_bits_next_two_power32(config->size - 1);
    config->size /= sizeof(uintptr_t);
    if (config->backlog_entries < SYS_CALLS_MIN_BACKLOG_ENTRIES)
    {
        config->backlog_entries = SYS_CALLS_MIN_BACKLOG_ENTRIES;
    }
    config->backlog_entries =
        sys_bits_next_two_power32(config->backlog_entries - 1);
    if (config->backlog_preallocate > config->backlog_entries)
    {
        config->backlog_preallocate = config->backlog_entries;
    }
}

err_t sys_calls_initialize(sys_calls_config_t * config)
{
    if (config != NULL)
    {
        sys_calls_config = *config;
    }

    sys_calls_config_prepare(&sys_calls_config);

    sys_calls_used = 0;

    return 0;
}

void sys_calls_terminate(void)
{
    SYS_TEST(
        sys_calls_used == 0,
        EBUSY,
        E(),
        ASSERT("Calls infrastructure is still in use")
    );
}

err_t sys_calls_buffer_allocate(sys_calls_node_t ** buffer, uintptr_t size)
{
    return SYS_ALLOC_ALIGNED(
               buffer, size * sizeof(uintptr_t), CACHE_LINE_SIZE,
               sys_mt_sys_calls_t,
               E()
           );
}

sys_calls_node_t * sys_calls_buffer_allocate_force(uintptr_t size)
{
    sys_calls_node_t * buffer;

    SYS_ALLOC_ALIGNED(
        &buffer, size * sizeof(uintptr_t), CACHE_LINE_SIZE, sys_mt_sys_calls_t,
        E(),
        NO_FAIL()
    );

    return buffer;
}

void sys_calls_buffer_free(sys_calls_node_t * buffer)
{
    SYS_FREE_ALIGNED(buffer);
}

err_t sys_calls_buffer_initialize(sys_calls_config_t * config,
                                  sys_calls_end_t * head,
                                  sys_calls_end_t * tail)
{
    sys_calls_config_t local;
    sys_calls_node_t ** list, * buffer;
    err_t error;
    uint32_t i;

    if (config == NULL)
    {
        config = &sys_calls_config;
    }
    local = *config;

    sys_calls_config_prepare(&local);

    SYS_CALL(
        sys_calls_buffer_allocate, (&buffer, local.size),
        E(),
        RETERR()
    );

    buffer->callback = SYS_CALLS_NULL;

    SYS_CALLOC0(
        &list, local.backlog_entries,
        sys_mt_sys_calls_t,
        E(),
        GOTO(failed, &error)
    );

    for (i = 0; i < local.backlog_preallocate; i++)
    {
        SYS_CALL(
            sys_calls_buffer_allocate, (&list[i], local.size),
            E(),
            GOTO(failed_list, &error)
        );
    }

    head->position = tail->position = 0;
    head->size = tail->size = local.size;
    head->buffer = tail->buffer = buffer;
    head->free = (uintptr_t *)buffer + local.size;
    tail->free = (uintptr_t *)head;
    head->free_position = 0;
    tail->free_position = local.backlog_preallocate;
    head->free_mask = tail->free_mask = local.backlog_entries - 1;
    head->free_buffers = tail->free_buffers = list;
    head->allocations = head->processed = head->callbacks = 0;
    tail->allocations = tail->processed = tail->callbacks = 0;

    atomic_inc(&sys_calls_used, memory_order_seq_cst);

    return 0;

failed_list:
    while (i-- > 0)
    {
        sys_calls_buffer_free(list[i]);
    }
    SYS_FREE(list);
failed:
    sys_calls_buffer_free(buffer);

    return error;
}

void sys_calls_buffer_terminate(sys_calls_end_t * head, sys_calls_end_t * tail)
{
    uint32_t i;

    SYS_TEST(
        (head->processed == tail->processed),
        EBUSY,
        C(),
        ASSERT("Callback buffer is not empty.")
    );

    for (i = 0; i <= head->free_mask; i++)
    {
        sys_calls_buffer_free(head->free_buffers[i]);
    }
    SYS_FREE(head->free_buffers);
    sys_calls_buffer_free(head->buffer);

    atomic_dec(&sys_calls_used, memory_order_seq_cst);
}

sys_calls_node_t * sys_calls_buffer_get(sys_calls_end_t * tail)
{
    sys_calls_node_t * buffer;
    uint64_t pos, size;

    pos = tail->free_position;
    size = tail->size;

    buffer = atomic_load(&tail->free_buffers[pos], memory_order_acquire);

    if (unlikely(buffer == NULL))
    {
        tail->allocations++;
        buffer = sys_calls_buffer_allocate_force(size);
    }
    else
    {
        atomic_store(&tail->free_buffers[pos], NULL, memory_order_release);

        tail->free_position = (pos + SYS_CALLS_INC) & tail->free_mask;
    }

    return buffer;
}

void sys_calls_buffer_put(sys_calls_end_t * head, sys_calls_node_t * buffer)
{
    sys_calls_node_t * tmp;
    uint32_t pos;

    pos = head->free_position;
    tmp = atomic_load(&head->free_buffers[pos], memory_order_acquire);
    if (unlikely(tmp != NULL))
    {
        head->allocations++;
        sys_calls_buffer_free(buffer);
    }
    else
    {
        atomic_store(&head->free_buffers[pos], buffer, memory_order_release);

        head->free_position = (pos + SYS_CALLS_INC) & head->free_mask;
    }
}

err_t sys_calls_create(sys_calls_t ** calls, sys_calls_config_t * config)
{
    sys_calls_t * tmp;
    err_t error;

    SYS_MALLOC0_ALIGNED(
        &tmp, CACHE_LINE_SIZE, sys_mt_sys_calls_t,
        E(),
        RETERR()
    );

    SYS_CALL(
        sys_calls_buffer_initialize, (config, &tmp->head, &tmp->tail),
        E(),
        GOTO(failed, &error)
    );

    *calls = tmp;

    return 0;

failed:
    SYS_FREE_ALIGNED(tmp);

    return error;
}

void sys_calls_destroy(sys_calls_t * calls)
{
    sys_calls_buffer_terminate(&calls->head, &calls->tail);
    SYS_FREE_ALIGNED(calls);
}

uintptr_t * sys_calls_prepare(sys_calls_end_t * tail, uint32_t extra,
                              uint32_t size)
{
    sys_calls_node_t * node;
    uintptr_t pos, length;
    uintptr_t * data;

    tail->processed++;

    pos = tail->position;
    node = (sys_calls_node_t *)((uintptr_t *)tail->buffer + pos);

    extra += SYS_CALLS_NODE_SIZE;
    length = size + extra;
    pos += length;

    if (unlikely(tail->size < pos + SYS_CALLS_NODE_SIZE))
    {
        sys_calls_node_t * tmp;

        tmp = (sys_calls_node_t *)sys_calls_buffer_get(tail);
        tail->buffer = tmp;
        node->next = tmp;

        tmp->callback = SYS_CALLS_NULL;

        smp_wmb();

        atomic_store(&node->callback, SYS_CALLS_SWAP, memory_order_release);

        node = tmp;
        pos = length;
    }

    node->size = length;
    node->extra = extra;
    node->xl = THIS;

    data = (uintptr_t *)node;
    ((sys_calls_node_t *)(data + length))->callback = SYS_CALLS_NULL;

    tail->position = pos;

    return data + SYS_CALLS_NODE_SIZE;
}

uintptr_t * sys_calls_add(sys_calls_end_t * tail, sys_callback_f callback,
                          uint32_t extra, uint32_t size, uint32_t refs)
{
    sys_calls_node_t * node;
    uintptr_t pos, length;
    uintptr_t * data;

    tail->processed++;

    pos = tail->position;
    node = (sys_calls_node_t *)((uintptr_t *)tail->buffer + pos);

    extra += SYS_CALLS_NODE_SIZE;

    length = size + extra;
    pos += length;

    if (unlikely(tail->size < pos + SYS_CALLS_NODE_SIZE))
    {
        sys_calls_node_t * tmp;

        tmp = (sys_calls_node_t *)sys_calls_buffer_get(tail);
        tail->buffer = tmp;
        node->next = tmp;

        tmp->callback = SYS_CALLS_NULL;

        atomic_store(&node->callback, SYS_CALLS_SWAP, memory_order_release);

        node = tmp;
        pos = length;
    }

    node->size = length;
    node->extra = extra;
    node->refs = refs + 1;
    node->flags = 0;
    node->error = 0;
    node->xl = THIS;

    data = (uintptr_t *)node;
    ((sys_calls_node_t *)(data + length))->callback = SYS_CALLS_NULL;

    atomic_store(&node->callback, callback, memory_order_release);

    logD("SYS-CALLS: add %p (%d)", node, node->refs);

    tail->position = pos;

    return data + SYS_CALLS_NODE_SIZE;
}

uintptr_t * sys_calls_get(sys_calls_end_t * head)
{
    sys_calls_node_t * node, * buffer;
    sys_callback_f callback;
    uintptr_t pos;

    pos = head->position;
    buffer = head->buffer;
    node = (sys_calls_node_t *)((uintptr_t *)buffer + pos);

    callback = atomic_load(&node->callback, memory_order_acquire);
    if (callback != SYS_CALLS_NULL)
    {
        if (unlikely(callback == SYS_CALLS_SWAP))
        {
            sys_calls_node_t * tmp;

            tmp = node->next;
            callback = atomic_load(&buffer->callback, memory_order_acquire);
            if ((callback == SYS_CALLS_EMPTY) && (buffer->next == node))
            {
                sys_calls_buffer_put(head, buffer);
            }
            else
            {
                node->next = buffer;
                atomic_store(&node->callback, SYS_CALLS_END,
                             memory_order_release);
            }

            head->buffer = tmp;

            node = tmp;
            callback = atomic_load(&node->callback, memory_order_acquire);
            pos = 0;
        }

        head->position = pos + node->size;

        if (callback != SYS_CALLS_NULL)
        {
            logD("SYS-CALLS: get %p", node);
            return (uintptr_t *)node + SYS_CALLS_NODE_SIZE;
        }
    }

    return NULL;
}

void sys_calls_release(sys_calls_end_t * head, uintptr_t * data)
{
    sys_calls_node_t * node, * next, * buffer;
    sys_callback_f callback;

    node = (sys_calls_node_t *)(data - SYS_CALLS_NODE_SIZE);
    logD("SYS-CALLS: release %p (%d)", node, node->refs);
    if (--node->refs > 0)
    {
        return;
    }

    next = (sys_calls_node_t *)((uintptr_t *)node + node->size);
    callback = atomic_load(&next->callback, memory_order_acquire);
    while (callback == SYS_CALLS_EMPTY)
    {
        next = next->next;
        callback = atomic_load(&next->callback, memory_order_acquire);
    }
    if (callback == SYS_CALLS_END)
    {
        buffer = next->next;
        if ((buffer == node) || (buffer->next == node))
        {
            sys_calls_buffer_put(head, buffer);

            return;
        }
    }

    node->next = next;
    atomic_store(&node->callback, SYS_CALLS_EMPTY, memory_order_release);
}

bool sys_calls_process(sys_calls_end_t * head)
{
    sys_calls_node_t * node, * buffer;
    sys_callback_f callback;
    uintptr_t pos;

    pos = head->position;
    buffer = head->buffer;
    node = (sys_calls_node_t *)((uintptr_t *)buffer + pos);

    callback = atomic_load(&node->callback, memory_order_acquire);
    if (callback != SYS_CALLS_NULL)
    {
        if (unlikely(callback == SYS_CALLS_SWAP))
        {
            node = node->next;
            sys_calls_buffer_put(head, buffer);
            head->buffer = node;

            callback = node->callback;
            pos = 0;
        }

        head->position = pos + node->size;

        if (callback != SYS_CALLS_NULL)
        {
            logD("SYS-CALLS: process %p", node);
            THIS = node->xl;
            callback((uintptr_t *)node + node->extra);

            head->callbacks++;
            head->processed++;

            return true;
        }
    }

    return false;
}
/*
void __sys_calls_cancellable_release(sys_calls_end_t * head,
                                     sys_calls_node_t * node,
                                     sys_calls_cancellable_t * item,
                                     uintptr_t * data)
{
    sys_calls_node_t * base;
    sys_callback_f callback;

    if (item->refs > 0)
    {
        return;
    }

    node->callback = SYS_CALLS_EMPTY;

    base = (sys_calls_node_t *)item->buffer;
    item->buffer = data;
    callback = atomic_load(&base->callback, memory_order_acquire);
    if (callback == SYS_CALLS_EMPTY)
    {
        node = base;
        do
        {
            data = (uintptr_t *)node + SYS_CALLS_NODE_SIZE + node->extra -
                                       SYS_CALLS_CANCELLABLE_SIZE;
            item = (sys_calls_cancellable_t *)data;
            node = (sys_calls_node_t *)item->buffer;
            callback = atomic_load(&node->callback, memory_order_acquire);
        } while (callback == SYS_CALLS_EMPTY);
        if (callback == SYS_CALLS_SWAP)
        {
            sys_calls_buffer_put(head, (uintptr_t *)base);
        }
        else
        {
            data = (uintptr_t *)base + SYS_CALLS_NODE_SIZE + node->extra -
                                       SYS_CALLS_CANCELLABLE_SIZE;
            item = (sys_calls_cancellable_t *)data;
            item->buffer = (uintptr_t *)node;
        }
    }
}

void sys_calls_cancellable_release(sys_calls_end_t * head, uintptr_t * data)
{
    sys_calls_cancellable_t * item;
    sys_calls_node_t * node;

    item = (sys_calls_cancellable_t *)data;
    data += SYS_CALLS_CANCELLABLE_SIZE;
    node = *(sys_calls_node_t **)data;
    data += node->size + 1;

    item->refs--;

    __sys_calls_cancellable_release(head, node, item, data);
}

err_t sys_calls_cancellable_process(sys_calls_end_t * head, uintptr_t * data,
                                    err_t error)
{
    sys_calls_cancellable_t * item;
    sys_calls_node_t * node;
    sys_callback_f callback;
    uintptr_t size;

    item = (sys_calls_cancellable_t *)data;
    data += SYS_CALLS_CANCELLABLE_SIZE;
    node = *(sys_calls_node_t **)data;
    data++;

    size = node->size;

    if ((item->refs & SYS_CALLS_NOTIFIED) == 0)
    {
        item->refs += 0xFFFF;
        item->error = error;

        callback = atomic_load(&node->callback, memory_order_acquire);
        sys_thread_set_error(error);
        callback(data, size);
        head->callbacks[error != 0]++;
    }
    else
    {
        item->refs += 0xFFFF;
        error = item->error;
    }

    __sys_calls_cancellable_release(head, node, item, data + size);

    return error;
}
*/
