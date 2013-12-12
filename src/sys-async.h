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

#ifndef __SYS_ASYNC_H__
#define __SYS_ASYNC_H__

#include <signal.h>

#define SYS_DELAY_DONE_BIT 0
#define SYS_DELAY_DONE (1 << SYS_DELAY_DONE_BIT)

#define SYS_ASYNC_DECLARE(_name, _args) \
    SYS_CALLS_DECLARE(_name, _args); \
    SYS_CALLS_WRAPPER(_name, _args, ()) \
    { \
        uintptr_t * __sys_async_data; \
        sys_async_queue_t * __sys_async_queue; \
        __sys_async_data = sys_async_prepare(0, SYS_CALLS_SIZE(_name), \
                                             &__sys_async_queue); \
        SYS_MARSHALL(__sys_async_data, _name, _args); \
        sys_async_commit(__sys_async_data, __sys_async_queue, \
                         SYS_PROXY(_name)); \
        return __sys_async_data; \
    }

#define SYS_ASYNC_DEFINE(_name, _args) SYS_CALLS_DEFINE(_name, _args)

#define SYS_ASYNC_CREATE(_name, _args) \
    SYS_ASYNC_DECLARE(_name, _args); \
    SYS_ASYNC_DEFINE(_name, _args)

#define SYS_ASYNC_TO_DECLARE(_name, _args) \
    SYS_CALLS_DECLARE(_name, _args); \
    SYS_CALLS_WRAPPER(_name, _args, \
        ((sys_async_queue_t *, __sys_async_to_queue))) \
    { \
        uintptr_t * __sys_async_to_data; \
        __sys_async_to_data = sys_async_to_prepare(0, SYS_CALLS_SIZE(_name), \
                                                   __sys_async_to_queue); \
        SYS_MARSHALL(__sys_async_to_data, _name, _args); \
        sys_async_to_commit(__sys_async_to_data, __sys_async_to_queue, \
                            SYS_PROXY(_name)); \
        return __sys_async_to_data; \
    }

#define SYS_ASYNC_TO_DEFINE(_name, _args) SYS_CALLS_DEFINE(_name, _args)

#define SYS_ASYNC_TO_CREATE(_name, _args) \
    SYS_ASYNC_TO_DECLARE(_name, _args); \
    SYS_ASYNC_TO_DEFINE(_name, _args)

#define SYS_DELAY_DECLARE(_name, _args) \
    SYS_CALLS_DECLARE(_name, _args); \
    SYS_CALLS_WRAPPER(_name, _args, ((uint32_t, __sys_delay_refs), \
                                     (int64_t, __sys_delay_ms), \
                                     (size_t, __sys_delay_extra))) \
    { \
        uintptr_t * __sys_delay_data; \
        __sys_delay_data = sys_delay_add(SYS_PROXY(_name), __sys_delay_extra, \
                                         SYS_CALLS_SIZE(_name), \
                                         __sys_delay_refs, __sys_delay_ms); \
        logT("SYS-DELAY: delay %ld (%lu, %u) '" #_name "' %p", \
             __sys_delay_ms, __sys_delay_extra, __sys_delay_refs, \
             __sys_delay_data); \
        SYS_MARSHALL(__sys_delay_data + __sys_delay_extra, _name, _args); \
        return __sys_delay_data; \
    }

#define SYS_DELAY_DEFINE(_name, _args) SYS_CALLS_DEFINE(_name, _args)

#define SYS_DELAY_CREATE(_name, _args) \
    SYS_DELAY_DECLARE(_name, _args); \
    SYS_DELAY_DEFINE(_name, _args)

#define SYS_RCU_DECLARE(_name, _args) \
    SYS_CALLS_DECLARE(_name, _args); \
    SYS_CALLS_WRAPPER(_name, _args, ()) \
    { \
        uintptr_t * __sys_rcu_data; \
        __sys_rcu_data = sys_calls_owned_add(&sys_async_owned_calls->tail, \
                                             SYS_PROXY(_name), \
                                             sys_async_self->head.id, \
                                             SYS_RCU_SIZE, \
                                             SYS_CALLS_SIZE(_name), 0); \
        SYS_MARSHALL(__sys_rcu_data + SYS_RCU_SIZE, _name, _args); \
        return sys_rcu_add(__sys_rcu_data); \
    }

#define SYS_RCU_DEFINE(_name, _args) SYS_CALLS_DEFINE(_name, _args)

#define SYS_RCU_CREATE(_name, _args) \
    SYS_RCU_DECLARE(_name, _args); \
    SYS_RCU_DEFINE(_name, _args)

#define SYS_ASYNC(_call, _args) SYS_WRAP(_call, (), _args)

#define SYS_ASYNC_TO(_queue, _call, _args) SYS_WRAP(_call, (_queue), _args)

#define __SYS_DELAY(_delay, _extra, _call, _args, _refs...) \
    SYS_WRAP(_call, (_refs + 0, _delay, _extra), _args)

#define SYS_DELAY(_delay, _call, _args, _refs...) \
    __SYS_DELAY(_delay, 0, _call, _args, ## _refs)

#define SYS_RCU(_call, _args) SYS_WRAP(_call, (), _args)

#define sys_rcu_read_lock() do { } while (0)
#define sys_rcu_read_unlock() do { } while (0)

// TODO: use proper barriers and optimizations (see linux kernel)
#define sys_rcu_assign_pointer(_ptr, _value) \
    do \
    { \
        smp_wmb(); \
        ACCESS_ONCE(_ptr) = _value; \
    } while (0);

#define sys_rcu_dereference(_ptr) ACCESS_ONCE(_ptr)

#define SYS_DELAY_SIZE SYS_CALLS_ADJUST_SIZE(sizeof(sys_async_delay_t))
#define SYS_RCU_SIZE SYS_CALLS_ADJUST_SIZE(sizeof(sys_rcu_t))

#define SYS_ASYNC_FLAG_ACTIVE   1
#define SYS_ASYNC_FLAG_SLEEPING 2
#define SYS_ASYNC_FLAG_LAST     4

typedef struct _sys_async_config
{
    int32_t            min_workers;
    int32_t            max_workers;
    int32_t            worker_timeout;
    int32_t            calls_loops;
    int32_t            full_loops;
    bool               bind_to_core;
    int32_t            min_timers;
    int32_t            rcu_grace_period;
    int32_t            rcu_threshold;
    sys_calls_config_t queue;
    sys_calls_config_t timer;
    sys_calls_config_t owned;
} sys_async_config_t;

typedef struct _sys_async_delay
{
    sys_heap_item_t heap;
    int32_t         flags;
} sys_async_delay_t;

typedef struct _sys_async_end
{
    sys_calls_end_t  calls;
    struct list_head list;
    pthread_t        thread;
    int32_t          id;
    int32_t          state;
} __align(CACHE_LINE_SIZE) sys_async_end_t;

typedef struct _sys_async_queue
{
    sys_async_end_t head;
    sys_async_end_t tail;
} sys_async_queue_t;

typedef struct _sys_async_rcu
{
    struct list_head list;
    uintptr_t *      timer;
    uint64_t         count;
} sys_async_rcu_t;

typedef struct _sys_rcu
{
    struct list_head list;
} sys_rcu_t;

extern sys_async_config_t sys_async_config;

extern sys_mutex_t sys_async_mutex;

extern sys_async_queue_t * sys_async_shared;

extern __thread sys_async_queue_t * sys_async_self;
extern __thread sys_async_queue_t * sys_async_current;
extern __thread sys_calls_t * sys_async_owned_calls;
extern __thread sys_calls_t * sys_async_timer_calls;
extern __thread sys_async_rcu_t sys_async_rcu;

extern __thread sys_heap_t sys_async_timer_heap;

extern sys_async_queue_t *** sys_async_queue_map;

uintptr_t * sys_async_add(uint32_t extra, uint32_t size,
                          sys_async_queue_t ** queue);

static inline sys_async_queue_t * sys_async_queue_get(int32_t id)
{
    return sys_async_queue_map[sys_async_self->head.id][id];
}

static inline void sys_async_wake(sys_async_queue_t * queue)
{
    if ((queue->tail.state & SYS_ASYNC_FLAG_SLEEPING) != 0)
    {
        sys_thread_signal_kill(&queue->tail.thread, SIGCONT);
    }
}

static inline uintptr_t * sys_async_prepare(uint32_t extra, uint32_t size,
                                            sys_async_queue_t ** queue)
{
    uintptr_t * data;

    if (sys_async_current == NULL)
    {
        sys_mutex_lock(&sys_async_mutex);

        data = sys_async_add(extra, size, &sys_async_shared);
        *queue = sys_async_shared;

        sys_mutex_unlock(&sys_async_mutex);
    }
    else
    {
        data = sys_async_add(extra, size, &sys_async_current);
        *queue = sys_async_current;
    }

    return data;
}

static inline void sys_async_commit(uintptr_t * data,
                                    sys_async_queue_t * queue,
                                    sys_callback_f callback)
{
    sys_calls_commit(data, callback);

    sys_async_wake(queue);
}

uintptr_t * sys_async_to_add(uint32_t extra, uint32_t size,
                             sys_async_queue_t * queue);

static inline uintptr_t * sys_async_to_prepare(uint32_t extra, uint32_t size,
                                               sys_async_queue_t * queue)
{
    uintptr_t * data;

    if (sys_async_current == NULL)
    {
        sys_mutex_lock(&sys_async_mutex);

        data = sys_async_to_add(extra, size, queue);

        sys_mutex_unlock(&sys_async_mutex);
    }
    else
    {
        data = sys_async_to_add(extra, size, queue);
    }

    return data;
}

static inline void sys_async_to_commit(uintptr_t * data,
                                       sys_async_queue_t * queue,
                                       sys_callback_f callback)
{
    sys_async_commit(data, queue, callback);
}

static inline uintptr_t * sys_delay_prepare(sys_callback_f callback,
                                            uint32_t extra, uint32_t size,
                                            uint32_t refs)
{
    sys_async_delay_t * delay;
    uintptr_t * data;

    data = sys_calls_owned_add(&sys_async_timer_calls->tail, callback,
                               sys_async_self->head.id, extra + SYS_DELAY_SIZE,
                               size, refs);

    delay = (sys_async_delay_t *)data;
    delay->heap.index = -1;
    delay->flags = 0;

    return data + SYS_DELAY_SIZE;
}

static inline void sys_delay_commit(uintptr_t * data, int64_t ms)
{
    sys_async_delay_t * delay;

    delay = (sys_async_delay_t *)(data - SYS_DELAY_SIZE);
    ms += sys_tsc_time();
    sys_heap_insert(&sys_async_timer_heap, &delay->heap, ms, k1, k2, k1 - k2);
}

static inline uintptr_t * sys_delay_add(sys_callback_f callback,
                                        uint32_t extra, uint32_t size,
                                        uint32_t refs, int64_t ms)
{
    uintptr_t * data;

    data = sys_delay_prepare(callback, extra, size, refs);
    sys_delay_commit(data, ms);

    return data;
}

SYS_ASYNC_TO_DECLARE(__sys_delay_release, ((uintptr_t *, data)));

static inline void sys_delay_release(uintptr_t * data)
{
    uintptr_t owner, * delay;

    logT("SYS-DELAY: release %p", data);
    delay = data - SYS_DELAY_SIZE;
    owner = sys_calls_owned_owner(delay);
    if (owner == sys_async_self->head.id)
    {
        sys_calls_owned_release(&sys_async_timer_calls->head, delay);
    }
    else
    {
        SYS_ASYNC_TO(sys_async_queue_get(owner), __sys_delay_release, (data));
    }
}

SYS_ASYNC_TO_DECLARE(__sys_delay_cancel, ((uintptr_t *, data),
                                          (bool, notify)));

static inline bool sys_delay_cancel(uintptr_t * data, bool notify)
{
    uintptr_t owner, * delay;
    uint8_t * flags;

    logT("SYS-DELAY: cancel %p", data);
    delay = data - SYS_DELAY_SIZE;
    flags = sys_calls_owned_flags(delay);
    if (atomic_bit_set(flags, SYS_DELAY_DONE_BIT, memory_order_seq_cst) != 0)
    {
        sys_delay_release(data);

        return false;
    }
    owner = sys_calls_owned_owner(delay);
    // use always an asynchronous call, even if the current thread is the
    // owner. If you want to synchronously call the delayed function and
    // you are sure you are in the same thread, use __sys_delay_cancel().
    SYS_ASYNC_TO(sys_async_queue_get(owner),
                 __sys_delay_cancel, (data, notify));

    return true;
}

SYS_ASYNC_TO_DECLARE(__sys_delay_execute, ((uintptr_t *, data),
                                           (err_t, error)));

static inline bool sys_delay_execute(uintptr_t * data, err_t error)
{
    uintptr_t owner, * delay;
    uint8_t * flags;

    logT("SYS-DELAY: execute %p", data);
    delay = data - SYS_DELAY_SIZE;
    flags = sys_calls_owned_flags(delay);
    if (atomic_bit_set(flags, SYS_DELAY_DONE_BIT, memory_order_seq_cst) != 0)
    {
        logT("SYS-DELAY: already completed %p", data);
        sys_delay_release(data);

        return false;
    }
    owner = sys_calls_owned_owner(delay);
    // use always an asynchronous call, even if the current thread is the
    // owner. If you want to synchronously call the delayed function and
    // you are sure you are in the same thread, use __sys_delay_execute().
    SYS_ASYNC_TO(sys_async_queue_get(owner),
                 __sys_delay_execute, (data, error));

    return true;
}

SYS_DELAY_DECLARE(sys_rcu_process, ());

static inline uintptr_t * sys_rcu_add(uintptr_t * data)
{
    sys_rcu_t * rcu;

    rcu = (sys_rcu_t *)data;
    list_add_tail(&rcu->list, &sys_async_rcu.list);
    if (sys_async_rcu.count++ == 0)
    {
        sys_async_rcu.timer = SYS_DELAY(sys_async_config.rcu_grace_period,
                                        sys_rcu_process, (), 1);
    }
    if (sys_async_rcu.count >= sys_async_config.rcu_threshold)
    {
        sys_delay_execute(sys_async_rcu.timer, 0);
    }

    return data + SYS_RCU_SIZE;
}

err_t sys_async_initialize(sys_async_config_t * config);
void sys_async_terminate(void);

void sys_async_statistics(void);
void sys_async_dump(void);

#endif /* __SYS_ASYNC_H__ */
