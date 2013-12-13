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

#include <string.h>
#include <signal.h>

//static pthread_t ids[5];

#define sys_async_next_send(_queue) \
    list_entry((_queue)->tail.list.next, sys_async_queue_t, tail.list)
#define sys_async_prev_send(_queue) \
    list_entry((_queue)->tail.list.prev, sys_async_queue_t, tail.list)
#define sys_async_next_recv(_queue) \
    list_entry((_queue)->head.list.next, sys_async_queue_t, head.list)
#define sys_async_prev_recv(_queue) \
    list_entry((_queue)->head.list.prev, sys_async_queue_t, head.list)

typedef struct _sys_async_stats
{
    uint64_t callbacks;        // number of callbacks executed
    uint64_t timers;           // number of timers executed
    uint64_t rcus;             // number of RCUs executed
    uint64_t sleeps;           // number of times the worker gone to sleep
    uint64_t sleeps_cancelled; // number of times the sleeping was cancelled
    uint64_t stops_cancelled;  // number of times the stopping was cancelled
} sys_async_stats_t;

typedef struct _sys_async_pair
{
    sys_async_queue_t receive;
    sys_async_queue_t send;
} sys_async_pair_t;

sys_mutex_t sys_async_mutex = SYS_MUTEX_INITIALIZER;

sys_async_config_t sys_async_config =
{
    .min_workers             = 2,
    .max_workers             = 0,
    .worker_timeout          = 10000,
    .calls_loops             = 50,
    .full_loops              = 10,
    .bind_to_core            = true,
    .min_timers              = 256,
    .rcu_grace_period        = 200,
    .rcu_threshold           = 50,
    .queue =
    {
        .size                = 65536,
        .backlog_entries     = 8,
        .backlog_preallocate = 2
    },
    .timer =
    {
        .size                = 65536,
        .backlog_entries     = 8,
        .backlog_preallocate = 2
    },
    .owned =
    {
        .size                = 65536,
        .backlog_entries     = 8,
        .backlog_preallocate = 2
    }
};

/// Number of asynchronous worker threads running.
static int32_t sys_async_count = 0;
static bool sys_async_busy = false;

/// Main asynchronous worker (first one).
static sys_async_pair_t sys_async_worker_main;

/// Queue to send new requests from threads not being asynchronous workers.
sys_async_queue_t * sys_async_shared =
    &sys_async_worker_main.receive;

/// Queue to send new requests from asynchronous workers.
__thread sys_async_queue_t * sys_async_current = NULL;

/** Queue corresponding to the first worker for each thread.
 *
 * It is initialized to the 'receive' queue of the first worker because this
 * will be the entry point for all threads not being an asynchronous worker
 * thread.
 */
static __thread sys_async_queue_t * sys_async_first =
    &sys_async_worker_main.receive;

/// Queue corresponding to the worker associated with each thread.
__thread sys_async_queue_t * sys_async_self = &sys_async_worker_main.receive;

/// Asynchronous worker statistics.
static __thread sys_async_stats_t sys_async_stats = { 0, };

static __thread bool sys_async_exit = false;

__thread sys_calls_t * sys_async_owned_calls = NULL;
__thread sys_calls_t * sys_async_timer_calls = NULL;
__thread sys_heap_t sys_async_timer_heap;
__thread sys_async_rcu_t sys_async_rcu;

sys_async_queue_t *** sys_async_queue_map = NULL;

void sys_async_worker_create(void);

err_t sys_async_queue_initialize(sys_async_queue_t * queue, int32_t id)
{
    SYS_CALL(
        sys_calls_buffer_initialize,
            (&sys_async_config.queue, &queue->head.calls,
             &queue->tail.calls),
        E(),
        RETERR()
    );

    INIT_LIST_HEAD(&queue->head.list);
    INIT_LIST_HEAD(&queue->tail.list);
    queue->head.thread = queue->tail.thread = pthread_self();
    queue->head.id = queue->tail.id = id;
    queue->head.state = queue->tail.state = SYS_ASYNC_FLAG_SLEEPING |
                                            SYS_ASYNC_FLAG_LAST;

    return 0;
}

void sys_async_queue_terminate(sys_async_queue_t * queue)
{
    SYS_TEST(
        list_empty(&queue->head.list) && list_empty(&queue->tail.list),
        EINVAL,
        C(),
        ASSERT("Destroying a queue attached to the asynchronous pool")
    );

    sys_calls_buffer_terminate(&queue->head.calls,
                               &queue->tail.calls);
}

err_t sys_async_pair_initialize(sys_async_pair_t * pair, int32_t id)
{
    err_t error;

    SYS_CALL(
        sys_async_queue_initialize, (&pair->receive, id),
        E(),
        RETERR()
    );

    SYS_CALL(
        sys_async_queue_initialize, (&pair->send, id),
        E(),
        GOTO(failed, &error)
    );

    return 0;

failed:
    sys_async_queue_terminate(&pair->receive);

    return error;
}

void sys_async_pair_terminate(sys_async_pair_t * pair)
{
    sys_async_queue_terminate(&pair->receive);
    sys_async_queue_terminate(&pair->send);
}

err_t sys_async_pairs_initialize(sys_async_pair_t * pairs)
{
    err_t error;
    int32_t i, count;

    error = 0;
    count = pairs->receive.head.id;
    for (i = 0; i < count; i++)
    {
        SYS_CALL(
            sys_async_pair_initialize, (&pairs[i], count),
            E(),
            BREAK(&error)
        );
    }
    if (i < count)
    {
        while (--i >= 0)
        {
            sys_async_pair_terminate(&pairs[i]);
        }
    }

    return error;
}

void sys_async_pairs_terminate(sys_async_pair_t * pairs)
{
    int32_t i, count;

    count = pairs[0].send.head.id;
    for (i = 0; i < count; i++)
    {
        sys_async_pair_terminate(&pairs[i]);
    }
}

err_t sys_async_pairs_allocate(sys_async_pair_t ** pairs, int32_t count)
{
    sys_async_pair_t * tmp;

    SYS_CALLOC_ALIGNED(
        &tmp, count, CACHE_LINE_SIZE,
        sys_mt_sys_async_pair_t,
        E(),
        RETERR()
    );

    tmp->receive.head.id = tmp->receive.tail.id = count;
    tmp->send.head.id = tmp->send.tail.id = count;

    *pairs = tmp;

    return 0;
}

void sys_async_pairs_destroy(sys_async_pair_t * pairs)
{
    SYS_FREE_ALIGNED(pairs);
}

err_t sys_async_queue_map_initialize(void)
{
    sys_async_queue_t *** tmp;
    int32_t i;
    err_t error;

    SYS_CALLOC(
        &tmp, sys_async_config.max_workers + 1, sys_mt_sys_async_queue_t,
        E(),
        RETERR()
    );

    for (i = 0; i <= sys_async_config.max_workers; i++)
    {
        SYS_CALLOC0(
            &tmp[i], sys_async_config.max_workers + 1, sys_mt_sys_async_queue_t,
            E(),
            GOTO(failed, &error)
        );
    }

    sys_async_queue_map = tmp;

    return 0;

failed:
    while (i > 0)
    {
        SYS_FREE(tmp[--i]);
    }
    SYS_FREE(tmp);

    return error;
}

void sys_async_queue_map_terminate(void)
{
    int32_t i;

    for (i = 0; i <= sys_async_config.max_workers; i++)
    {
        SYS_FREE(sys_async_queue_map[i]);
    }
    SYS_FREE(sys_async_queue_map);
}

err_t __sys_async_to_add(uint32_t extra, uint32_t size,
                         sys_async_queue_t * queue, uintptr_t ** data)
{
    if (unlikely((queue->tail.state & SYS_ASYNC_FLAG_ACTIVE) == 0))
    {
        return EBUSY;
    }

    *data = sys_calls_prepare(&queue->tail.calls, extra, size);

    return 0;
}

uintptr_t * sys_async_to_add(uint32_t extra, uint32_t size,
                             sys_async_queue_t * queue)
{
    uintptr_t * data;

    SYS_CALL(
        __sys_async_to_add, (extra, size, queue, &data),
        E(),
        ASSERT("Worker %u should not be inactive.", queue->tail.id)
    );

    return data;
}

SYS_ASYNC_TO_DECLARE(sys_async_worker_destroy, ((sys_async_pair_t *, pair),
                                                (sys_async_pair_t *, worker)));

uintptr_t * sys_async_add(uint32_t extra, uint32_t size,
                          sys_async_queue_t ** queue)
{
    sys_async_queue_t * first, * current;
    uintptr_t * data;

    current = first = *queue;
    do
    {
        current = sys_async_next_send(current);
//        logD("Trying %p: %d, %d", current, current->tail.state, current->tail.id);
        if ((current->tail.state & (SYS_ASYNC_FLAG_SLEEPING |
                                    SYS_ASYNC_FLAG_LAST)) !=
            (SYS_ASYNC_FLAG_SLEEPING | SYS_ASYNC_FLAG_LAST))
        {
//            logD("In");
            SYS_CALL(
                __sys_async_to_add, (extra, size, current, &data),
                T(),
                CONTINUE()
            );

            *queue = current;
//            logD("Done");
            return data;
        }
    } while (current != first);

    sys_panic("All asynchronous workers are inactive.");

    return NULL;
}

SYS_ASYNC_TO_DEFINE(__sys_delay_release, ((uintptr_t *, data)))
{
    logT("SYS-DELAY: release %p", data);
    data -= SYS_DELAY_SIZE;
    sys_calls_owned_release(&sys_async_timer_calls->head, data);
}

SYS_ASYNC_TO_DEFINE(__sys_delay_cancel, ((uintptr_t *, data),
                                         (bool, notify)))
{
    sys_async_delay_t * delay;
    int64_t key;

    logT("SYS-DELAY: cancel %p", data);
    data -= SYS_DELAY_SIZE;
    delay = (sys_async_delay_t *)data;
    if (delay->heap.index != -1)
    {
        sys_heap_delete(&sys_async_timer_heap, delay->heap.index, &key,
                        k1, k2, k1 - k2);
        delay->heap.index = -1;
        sys_calls_owned_release(&sys_async_timer_calls->head, data);
    }
    if (notify)
    {
        sys_thread_set_error(ECANCELED);
        sys_calls_owned_execute(data);
    }
    sys_calls_owned_release(&sys_async_timer_calls->head, data);
    sys_async_stats.timers++;
}

SYS_ASYNC_TO_DEFINE(__sys_delay_execute, ((uintptr_t *, data),
                                          (err_t, error)))
{
    sys_async_delay_t * delay;
    int64_t key;

    logT("SYS-DELAY: execute %p", data);
    data -= SYS_DELAY_SIZE;
    delay = (sys_async_delay_t *)data;
    if (delay->heap.index != -1)
    {
        sys_heap_delete(&sys_async_timer_heap, delay->heap.index, &key,
                        k1, k2, k1 - k2);
        delay->heap.index = -1;
        sys_calls_owned_release(&sys_async_timer_calls->head, data);
    }
    sys_thread_set_error(error);
    sys_calls_owned_execute(data);
    sys_calls_owned_release(&sys_async_timer_calls->head, data);
    sys_async_stats.timers++;
}

SYS_ASYNC_TO_DECLARE(sys_rcu_quiesce, ((sys_async_queue_t *, queue),
                                       (struct list_head *, items)))

void __sys_rcu_process(struct list_head * items)
{
    struct list_head * first;
    sys_rcu_t * rcu;

    first = items;
    do
    {
        rcu = list_entry(items, sys_rcu_t, list);
        items = items->next;

        sys_calls_owned_execute((uintptr_t *)rcu);
        sys_calls_owned_release(&sys_async_owned_calls->head,
                                (uintptr_t *)rcu);
    } while (items != first);
}

void __sys_rcu_quiesce(sys_async_queue_t * queue, struct list_head * items)
{
    sys_async_queue_t * next;

    next = sys_async_next_send(sys_async_self);
    while (next != queue)
    {
        if ((next->tail.state & SYS_ASYNC_FLAG_ACTIVE) != 0)
        {
            SYS_ASYNC_TO(next, sys_rcu_quiesce, (queue, items));
            return;
        }
        next = sys_async_next_send(next);
    }

    __sys_rcu_process(items);
}

SYS_ASYNC_TO_DEFINE(sys_rcu_quiesce, ((sys_async_queue_t *, queue),
                                      (struct list_head *, items)))
{
    if (queue == sys_async_self)
    {
        __sys_rcu_process(items);
    }
    else
    {
        __sys_rcu_quiesce(queue, items);
    }
}

SYS_DELAY_DEFINE(sys_rcu_process, ((void, data, CALLS)))
{
    struct list_head * items;

    SYS_ASSERT(
        !list_empty(&sys_async_rcu.list),
        "RCU list is empty"
    );

    if (sys_thread_get_error() == ETIMEDOUT)
    {
        sys_delay_release(data);
    }

    items = sys_async_rcu.list.next;
    list_del_init(&sys_async_rcu.list);
    sys_async_rcu.count = 0;

    __sys_rcu_quiesce(sys_async_self, items);
}

uint64_t sys_async_process(int32_t loops)
{
    sys_async_queue_t * current, * first;
    uint64_t global, processed, mask;

    global = -1;
    do
    {
        processed = 0;
        mask = 1;
        first = &container_of(sys_async_self, sys_async_pair_t, send)->receive;
        current = first;
        do
        {
            if (sys_calls_process(&current->head.calls))
            {
                processed |= mask;
                sys_async_stats.callbacks++;
            }
            mask <<= 1;
            current = sys_async_next_recv(current);
        } while (current != first);
        global &= processed;
    } while (--loops > 0);

    return global;
}

int64_t sys_async_process_timers(void)
{
    int64_t ms;
    sys_heap_item_t * item;
    sys_async_delay_t * delay;
    uintptr_t * data;
    uint8_t * flags;

    do
    {
        item = sys_heap_peek(&sys_async_timer_heap, &ms);
        if (item != NULL)
        {
            ms -= sys_tsc_time();
            if (ms > 0)
            {
                return ms;
            }
            sys_heap_get(&sys_async_timer_heap, &ms, k1, k2, k1 - k2);
            delay = container_of(item, sys_async_delay_t, heap);
            delay->heap.index = -1;
            data = (uintptr_t *)delay;
            flags = sys_calls_owned_flags(data);
            if (atomic_bit_set(flags, SYS_DELAY_DONE_BIT,
                               memory_order_seq_cst) == 0)
            {
                sys_thread_set_error(ETIMEDOUT);
                sys_calls_owned_execute(data);
                sys_calls_owned_release(&sys_async_timer_calls->head, data);
                sys_async_stats.timers++;
            }
        }
    } while (item != NULL);

    return 0;
}

void sys_async_set_state(int32_t clear, int32_t set)
{
    sys_async_queue_t * queue;
    int32_t state;

    queue = sys_async_self;
    do
    {
        state = queue->tail.state & ~clear;
        atomic_store(&queue->tail.state, state | set, memory_order_release);
        queue = sys_async_next_recv(queue);
    } while (queue != sys_async_self);
}

uint64_t sys_async_sleep(int64_t delay)
{
    sys_time_t timeout;
    err_t error;
    uint64_t processed;

    sys_async_stats.sleeps++;

//    logD("Worker %u: trying to sleep", sys_async_self->head.id);

    sys_async_set_state(0, SYS_ASYNC_FLAG_SLEEPING);
    sys_thread_signal_cancel(SYS_SIG(SIGCONT));

    error = 0;
    sys_time_from_serial_rel(&timeout, delay);
    sys_time_abs(&timeout);
    processed = sys_async_process(1);
    while (processed == 0)
    {
        error = sys_thread_signal_wait(NULL, SYS_SIG(SIGCONT), &timeout);
        if (error != 0)
        {
            break;
        }
        processed = sys_async_process(1);
    }
    if (error == 0)
    {
//        logD("Worker %u: cancelled", sys_async_self->head.id);
        sys_async_stats.sleeps_cancelled++;
    }

    sys_async_set_state(SYS_ASYNC_FLAG_SLEEPING, 0);
    sys_thread_signal_cancel(SYS_SIG(SIGCONT));

    return processed;
}

uint64_t sys_async_stop(void)
{
    sys_async_pair_t * pair;
    sys_time_t timeout;
    uint64_t processed;

    if (atomic_xchg(&sys_async_busy, true, memory_order_seq_cst) != false)
    {
        logD("Worker %u: Keep running because there is another start/stop "
             "operation", sys_async_self->head.id);

        return sys_async_process(sys_async_config.calls_loops);
    }

    logD("Worker %u: initiating shutdown", sys_async_self->head.id);

    sys_async_set_state(SYS_ASYNC_FLAG_ACTIVE, SYS_ASYNC_FLAG_SLEEPING);
    sys_thread_signal_cancel(SYS_SIG(SIGCONT));

    processed = sys_async_process(1);
    if (processed != 0)
    {
        sys_async_set_state(SYS_ASYNC_FLAG_SLEEPING, SYS_ASYNC_FLAG_ACTIVE);
        sys_thread_signal_cancel(SYS_SIG(SIGCONT));

        atomic_store(&sys_async_busy, false, memory_order_seq_cst);

        logD("Worker %u: shutdown cancelled", sys_async_self->head.id);

        sys_async_stats.stops_cancelled++;

        return processed;
    }

    pair = container_of(sys_async_self, sys_async_pair_t, send);

    sys_async_first = &sys_async_worker_main.receive;
    sys_async_current = NULL;

    SYS_ASYNC_TO(sys_async_first, sys_async_worker_destroy, (pair + 1, pair));

    sys_time_set_rel(&timeout, 1, 0);
    SYS_RETRY(
        30, 1, 1, 1,
        SYS_CALL(
            sys_thread_signal_wait, (NULL, SYS_SIG(SIGCONT), &timeout),
            D(),
            LOG(W(), "Asynchronous worker termination is taking too long")
        ),
        C(),
        ASSERT("Asynchronous worker pool seems to be broken.")
    );

    sys_async_set_state(SYS_ASYNC_FLAG_SLEEPING, 0);
    sys_thread_signal_cancel(SYS_SIG(SIGCONT));

    atomic_store(&sys_async_busy, 0, memory_order_seq_cst);

    sys_async_self = sys_async_first;

    sys_async_exit = true;

    return 0;
}

uint64_t __sys_async_core(void)
{
//    sys_async_bucket_t * bucket;
    uint64_t processed;
    int64_t ms;
    int32_t loops;

    loops = sys_async_config.full_loops;
    do
    {
        processed = sys_async_process(sys_async_config.calls_loops);
        ms = sys_async_process_timers();
        if (processed != 0)
        {
            return processed;
        }
    } while (--loops > 0);

    if (ms != 0)
    {
        processed = sys_async_sleep(ms);
        if (processed == 0)
        {
            sys_async_process_timers();
        }
        return processed;
    }

    processed = sys_async_sleep(sys_async_config.worker_timeout);
    if (processed == 0)
    {
        logD("Worker %u: No work to do", sys_async_self->head.id);
        if ((sys_async_self->tail.state & SYS_ASYNC_FLAG_LAST) != 0)
        {
            processed = sys_async_stop();
        }
    }

    return processed;
}

void sys_async_loop(void)
{
    do
    {
        __sys_async_core();
    } while (!sys_async_exit);
}

void sys_async_loop_main(void)
{
    uint64_t processed;
    int32_t counter, bits;

    counter = 0;
    processed = 0;
    do
    {
        if (!sys_async_busy)
        {
            bits = sys_bits_count64(processed >> 1);
            if (unlikely(sys_async_count < sys_async_config.min_workers))
            {
                logD("Starting minimum worker");
                sys_async_worker_create();
                counter = 0;
            }
            else if (unlikely(bits == sys_async_count))
            {
                if (++counter >= 8)
                {
//                    logD("Need more workers");
                    sys_async_worker_create();
                    counter = 0;
                }
            }
            else if (unlikely(bits == sys_async_count - 1) &&
                     ((processed >> 1) == (1 << bits) - 1))
            {
                sys_async_queue_t * q = sys_async_prev_send(sys_async_first);
                if ((q->tail.state & SYS_ASYNC_FLAG_ACTIVE) != 0)
                {
//                    logD("Waking worker %u", q->tail.id);
                    sys_async_wake(sys_async_prev_send(sys_async_first));
                }
                counter = 0;
            }
            else if (counter > 0)
            {
                logD("Reset counter: %u, %lX", counter, processed);
                counter = 0;
            }
        }
        else
        {
//            logD("Main is busy");
        }
        processed = __sys_async_core();
//        logD("Processed: %lX", processed);
    } while (!sys_async_exit);
}

void sys_async_worker_remove(sys_async_pair_t * pair)
{
    list_del_init(&pair->send.head.list);
    list_del_init(&pair->send.tail.list);
    list_del_init(&pair->receive.head.list);
    list_del_init(&pair->receive.tail.list);

    if (sys_async_current == &pair->receive)
    {
        sys_async_current = sys_async_first;
    }
}

void sys_async_worker_shared_add(sys_async_pair_t * item,
                                 sys_async_pair_t * list)
{
    sys_mutex_lock(&sys_async_mutex);

    list_add(&item->receive.tail.list, &list->receive.tail.list);

    sys_mutex_unlock(&sys_async_mutex);
}

void sys_async_worker_shared_remove(sys_async_pair_t * pair)
{
    sys_mutex_lock(&sys_async_mutex);

    if (sys_async_shared == &pair->receive)
    {
        sys_async_shared = sys_async_first;
    }
    list_del_init(&pair->receive.tail.list);

    sys_mutex_unlock(&sys_async_mutex);
}

SYS_ASYNC_TO_CREATE(sys_async_worker_add, ((sys_async_pair_t *, pair),
                                           (sys_async_pair_t *, new)))
{
    sys_async_pair_t * local;
    sys_async_queue_t * next;
    int32_t id;

    local = container_of(sys_async_self, sys_async_pair_t, send);

    id = sys_async_self->head.id;

    sys_async_queue_map[id][new->send.head.id] = &pair->receive;
    sys_async_queue_map[new->send.head.id][id] = &pair->send;

    pair->send.tail.state = sys_async_self->tail.state;

    pair->receive.head.id = sys_async_self->head.id;
    pair->send.tail.id = sys_async_self->head.id;
    pair->send.tail.thread = pthread_self();

//    logD("Adding worker: %d, %p, %p, %p, %p, %p, %u", sys_async_self->head.id,
//        sys_async_self, sys_async_first, sys_async_current, pair, new, sys_async_self->tail.state);
    list_add_tail(&pair->send.head.list, &local->receive.head.list);
    list_add_tail(&pair->send.tail.list, &new->send.tail.list);
    list_add_tail(&pair->receive.head.list, &new->send.head.list);
    list_add_tail(&pair->receive.tail.list, &sys_async_first->tail.list);

    next = sys_async_next_send(sys_async_self);
    if (next == &pair->receive)
    {
        sys_async_worker_shared_add(new, local);

        sys_async_set_state(SYS_ASYNC_FLAG_LAST, 0);

//        logD("State: %u", sys_async_self->tail.state);

        sys_async_wake(&new->send);
    }
    else
    {
        SYS_ASYNC_TO(next, sys_async_worker_add, (pair + 1, new));
    }
}

int32_t sys_async_worker_init(void * arg)
{
    sys_async_pair_t * pairs;
    err_t error;
    int32_t id;

    pairs = (sys_async_pair_t *)arg;

    SYS_CALL(
        sys_async_pairs_initialize, (pairs),
        E(),
        RETERR()
    );

    SYS_CALL(
        sys_calls_create, (&sys_async_owned_calls, &sys_async_config.owned),
        E(),
        GOTO(failed, &error)
    );

    SYS_CALL(
        sys_calls_create, (&sys_async_timer_calls, &sys_async_config.timer),
        E(),
        GOTO(failed_owned, &error)
    );

    SYS_CALL(
        sys_heap_initialize, (&sys_async_timer_heap,
                              sys_async_config.min_timers),
        E(),
        GOTO(failed_timer, &error)
    );

    sys_async_self = sys_async_current = &pairs[0].send;

    pairs[0].receive.head.id = 0;

    id = sys_async_self->head.id;

    sys_async_queue_map[0][id] = &pairs[0].receive;
    sys_async_queue_map[id][id] = &pairs[0].send;

    list_add(&pairs[0].send.head.list, &pairs[0].receive.head.list);

    INIT_LIST_HEAD(&sys_async_rcu.list);
    sys_async_rcu.timer = NULL;
    sys_async_rcu.count = 0;

    logD("Worker %u: initialized %lu", id, pthread_self());

//    ids[sys_async_self->head.id] = pthread_self();

    return 0;

failed_timer:
    sys_calls_destroy(sys_async_timer_calls);
failed_owned:
    sys_calls_destroy(sys_async_owned_calls);
failed:
    sys_async_pairs_terminate(pairs);

    return error;
}

void sys_async_worker_insert(sys_async_pair_t * pairs)
{
    sys_time_t timeout;

    SYS_ASYNC_TO(sys_async_first, sys_async_worker_add, (pairs + 1, pairs));

    sys_time_set_rel(&timeout, 1, 0);

    SYS_RETRY(
        30, 1, 1, 1,
        SYS_CALL(
            sys_thread_signal_wait, (NULL, SYS_SIG(SIGCONT), &timeout),
            D(),
            LOG(W(), "Asynchronous worker initialization is taking too long")
        ),
        C(),
        ASSERT("Asynchronous worker pool seems to be broken.")
    );
}

void __sys_async_show_stats(void)
{
    char nums[6][35];
    int32_t len;

    len = sizeof(nums[0]);
    sys_numfmt_dec(nums[0], &len, sys_async_stats.callbacks, '.');
    len = sizeof(nums[1]);
    sys_numfmt_dec(nums[1], &len, sys_async_stats.timers, '.');
    len = sizeof(nums[2]);
    sys_numfmt_dec(nums[2], &len, sys_async_stats.rcus, '.');
    len = sizeof(nums[3]);
    sys_numfmt_dec(nums[3], &len, sys_async_stats.sleeps, '.');
    len = sizeof(nums[4]);
    sys_numfmt_dec(nums[4], &len, sys_async_stats.sleeps_cancelled, '.');
    len = sizeof(nums[5]);
    sys_numfmt_dec(nums[5], &len, sys_async_stats.stops_cancelled, '.');

    logD("Worker %2u: C: %18s, T: %18s, R: %18s, S: %18s(%18s), X: %18s",
        sys_async_self->head.id,
        nums[0], nums[1], nums[2], nums[3], nums[4], nums[5]);
}

void sys_async_worker_common(void)
{
    int32_t flags;

    flags = SYS_ASYNC_FLAG_SLEEPING;
    if (sys_async_self->head.id <= sys_async_config.min_workers)
    {
        flags |= SYS_ASYNC_FLAG_LAST;
    }

    sys_async_set_state(flags, SYS_ASYNC_FLAG_ACTIVE);
    sys_thread_signal_cancel(SYS_SIG(SIGCONT));

    logD("Worker %u: Ready", sys_async_self->head.id);

    atomic_store(&sys_async_busy, false, memory_order_seq_cst);

    if (sys_async_self->head.id == 1)
    {
        sys_async_loop_main();
    }
    else
    {
        sys_async_loop();
    }

    __sys_async_show_stats();
}

void * sys_async_worker(void * arg)
{
    sys_async_pair_t * pairs;

    pairs = (sys_async_pair_t *)arg;

    logD("Worker %u: Starting", pairs->send.head.id);

    sys_async_worker_insert(pairs);

    sys_async_first = &pairs[1].send;

    sys_async_worker_common();

    logD("Worker %u: Terminated", pairs->send.head.id);

    return NULL;
}

void * sys_async_main(void * arg)
{
    logD("Worker 1: Starting");

    sys_async_first = sys_async_self;

    sys_async_worker_common();

    logD("Worker 1: Terminated");

    return NULL;
}

void sys_async_worker_create(void)
{
    sys_async_pair_t * pairs;
    int32_t core;

    if (sys_async_count >= sys_async_config.max_workers)
    {
        return;
    }

    if (atomic_xchg(&sys_async_busy, true, memory_order_seq_cst) != false)
    {
        logD("Delaying creation of a new worker because another start/stop "
             "operation is being processed");

        return;
    }

    SYS_CALL(
        sys_async_pairs_allocate, (&pairs, sys_async_count + 1),
        E(),
        LOG(E(), "Unable to allocate memory for an asynchronous worker"),
        GOTO(failed)
    );

    core = sys_async_config.bind_to_core ? sys_async_count : -1;

    SYS_CALL(
        sys_thread_create, (NULL, 0, core, 0, 0, sys_async_worker_init,
                            sys_async_worker, pairs),
        E(),
        LOG(E(), "Unable to create a thread for an asynchronous worker"),
        GOTO(failed_pairs)
    );

    sys_async_count++;

    return;

failed_pairs:
    sys_async_pairs_destroy(pairs);
failed:
    atomic_store(&sys_async_busy, false, memory_order_seq_cst);
}

SYS_CALLS_DEFINE(sys_async_worker_destroy, ((sys_async_pair_t *, pair),
                                            (sys_async_pair_t *, worker)))
{
    sys_async_queue_t * next;

    logD("Worker %u: Destroy %u", sys_async_self->head.id, worker->send.tail.id);

    if (sys_async_self == sys_async_first)
    {
        sys_async_count--;
    }

    sys_async_worker_remove(pair);

    next = sys_async_next_send(sys_async_self);
    if (next != sys_async_first)
    {
        SYS_ASYNC_TO(next, sys_async_worker_destroy, (pair + 1, worker));
    }
    else
    {
        sys_async_worker_shared_remove(worker);

        if (sys_async_self->head.id > sys_async_config.min_workers)
        {
            sys_async_set_state(0, SYS_ASYNC_FLAG_LAST);
        }

        sys_async_wake(&worker->receive);
    }
}

static char * units[] = { "Bytes", "KiB", "MiB" };

err_t sys_async_initialize(sys_async_config_t * config)
{
    int32_t core, len, idx;
    uint64_t value;
    err_t error;
    char str[48];

    if (config != NULL)
    {
        sys_async_config = *config;
    }
    if (sys_async_config.max_workers <= 0)
    {
        sys_async_config.max_workers = sys_thread_core_onln +
                                       sys_async_config.max_workers;
        if (sys_async_config.max_workers < 1)
        {
            sys_async_config.max_workers = 1;
        }
    }
    else if (sys_async_config.max_workers > sys_thread_core_onln)
    {
        sys_async_config.max_workers = sys_thread_core_onln;
    }
    if (sys_async_config.min_workers < 1)
    {
        sys_async_config.min_workers = 1;
    }
    if (sys_async_config.min_workers > sys_async_config.max_workers)
    {
        sys_async_config.min_workers = sys_async_config.max_workers;
    }
    sys_calls_config_prepare(&sys_async_config.queue);
    if (sys_async_config.worker_timeout < 5000)
    {
        sys_async_config.worker_timeout = 5000;
    }

    nlogD("sys-async", "Asynchronous workers: min=%d, max=%d, timeout=%d, "
                       "core binding=%s, queue size=%d, queues backlog=%d, "
                       "backlog preallocate=%d",
                       sys_async_config.min_workers,
                       sys_async_config.max_workers,
                       sys_async_config.worker_timeout,
                       sys_async_config.bind_to_core ? "yes" : "no",
                       sys_async_config.queue.size,
                       sys_async_config.queue.backlog_entries,
                       sys_async_config.queue.backlog_preallocate);

    value = sys_async_config.queue.backlog_preallocate *
            sys_async_config.queue.size *
            sys_async_config.min_workers * sys_async_config.min_workers;
    len = sizeof(str);
    sys_numfmt_units(str, &len, &idx, 0, 2, 1024, value, '.', 3, ',');
    nlogD("sys-async", "Minimum memory used for queues: %s %s",
                       str, units[idx]);

    value = sys_async_config.queue.backlog_entries *
            sys_async_config.queue.size *
            sys_async_config.max_workers * sys_async_config.max_workers;
    len = sizeof(str);
    sys_numfmt_units(str, &len, &idx, 0, 2, 1024, value, '.', 3, ',');
    nlogD("sys-async", "Maximum memory used for queues: %s %s",
                       str, units[idx]);

    SYS_CALL(
        sys_async_queue_map_initialize, (),
        E(),
        RETERR()
    );

    sys_async_count = 1;

    sys_async_worker_main.receive.head.id =
    sys_async_worker_main.receive.tail.id = 1;
    sys_async_worker_main.send.head.id =
    sys_async_worker_main.send.tail.id = 1;

    core = sys_async_config.bind_to_core ? 0 : -1;

    SYS_CALL(
        sys_thread_create, (NULL, 0, core, 0, 0, sys_async_worker_init,
                            sys_async_main, &sys_async_worker_main),
        E(),
        GOTO(failed, &error)
    );

    return 0;

failed:
    sys_async_queue_map_terminate();

    return error;
}

SYS_ASYNC_TO_CREATE(sys_async_show_stats, ())
{
    sys_async_queue_t * q;

    __sys_async_show_stats();

    q = sys_async_next_send(sys_async_self);
    if (q != sys_async_first)
    {
        SYS_ASYNC_TO(q, sys_async_show_stats, ());
    }
}

void sys_async_statistics(void)
{
    SYS_ASYNC_TO(sys_async_first, sys_async_show_stats, ());
}

SYS_ASYNC_TO_CREATE(sys_async_worker_dump, ())
{
    sys_async_queue_t * q;

    logD("Worker %u: %u", sys_async_self->head.id, sys_async_self->tail.state);
    logD("  sys_async_self:    %p(%p)", sys_async_self, sys_async_self->head.calls.buffer);
    logD("  sys_async_first:   %p(%p)", sys_async_first, sys_async_first->head.calls.buffer);
    logD("  sys_async_current: %p(%p)", sys_async_current, sys_async_current->head.calls.buffer);
    logD("  receive:");
    q = sys_async_self;
    do
    {
        logD("    %u: %p(%p)", q->head.id, q, q->head.calls.buffer);
        q = sys_async_next_recv(q);
    } while (q != sys_async_self);
    logD("  send:");
    q = sys_async_self;
    do
    {
        logD("    %u: %p(%p)", q->tail.id, q, q->tail.calls.buffer);
        q = sys_async_next_send(q);
    } while (q != sys_async_self);

    q = sys_async_next_send(sys_async_self);
    if (q != sys_async_first)
    {
        SYS_ASYNC_TO(q, sys_async_worker_dump, ());
    }
}

void sys_async_dump(void)
{
    SYS_ASYNC_TO(sys_async_first, sys_async_worker_dump, ());
}
