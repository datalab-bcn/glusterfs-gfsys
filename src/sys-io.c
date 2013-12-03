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

/*
typedef struct _sys_io_stats
{
    uint64_t callbacks;        // number of callbacks executed
    uint64_t timers;           // number of timers executed
    uint64_t rcus;             // number of RCUs executed
    uint64_t sleeps;           // number of times the worker gone to sleep
    uint64_t sleeps_cancelled; // number of times the sleeping was cancelled
    uint64_t stops_cancelled;  // number of times the stopping was cancelled
} sys_io_stats_t;
*/
sys_mutex_t sys_io_mutex = SYS_MUTEX_INITIALIZER;

static sys_io_config_t sys_io_config =
{
    .min_workers             = 16,
    .max_workers             = 16,
    .worker_timeout          = 10000,
    .loops                   = 50,
    .queue =
    {
        .size                = 65536,
        .backlog_entries     = 8,
        .backlog_preallocate = 2
    }
};

/// Number of I/O worker threads running.
static int32_t sys_io_count = 0;
static bool sys_io_busy = false;

uint64_t sys_io_sleeping = 0;

static __thread int32_t sys_io_id = 0;

sys_io_t sys_io_queue;

pthread_t * sys_io_threads = NULL;

//static __thread sys_io_stats_t sys_io_stats = { 0, };

static __thread bool sys_io_exit = false;

SYS_ASYNC_TO_DEFINE(sys_io_callback, ((sys_io_item_t *, item)))
{
//    logD("sys_io_callback: %p", item->cbk);
    *(uintptr_t **)item->cbk = (uintptr_t *)item + SYS_IO_SIZE;
    sys_calls_owned_execute(item->cbk);
    sys_calls_owned_release(&sys_async_owned_calls->head, item->cbk);
    sys_io_release((uintptr_t *)item + SYS_IO_SIZE);
}

sys_io_item_t * sys_io_get(int32_t loops)
{
    sys_io_item_t * item, * next;

    sys_mutex_lock(&sys_io_mutex);

    do
    {
        item = sys_io_queue.first;
        if (item != NULL)
        {
            next = item->next;
            sys_io_queue.first = next;
            if (next == NULL)
            {
                if (unlikely(!atomic_cmpxchg(&sys_io_queue.plast, &item->next,
                                             &sys_io_queue.first,
                                             memory_order_seq_cst,
                                             memory_order_seq_cst)))
                {
                    sys_io_queue.first = item;
                    item = NULL;
                    continue;
                }
            }

            break;
        }
    } while (--loops > 0);

    sys_mutex_unlock(&sys_io_mutex);

    return item;
}

sys_io_item_t * sys_io_sleep(int64_t delay)
{
    sys_time_t timeout;
    sys_io_item_t * item;
    err_t error;

    sys_time_from_serial_rel(&timeout, delay);
    sys_time_abs(&timeout);

    item = NULL;
    do
    {
        atomic_bit_set(&sys_io_sleeping, sys_io_id, memory_order_seq_cst);
        sys_thread_signal_cancel(SYS_SIG(SIGCONT));

        if (((sys_io_sleeping >> sys_io_id) & 1) != 0)
        {
            error = sys_thread_signal_wait(NULL, SYS_SIG(SIGCONT), &timeout);
            atomic_bit_clear(&sys_io_sleeping, sys_io_id,
                             memory_order_seq_cst);
            if (error != 0)
            {
                break;
            }
        }

        item = sys_io_get(1);
    } while (item == NULL);

    sys_thread_signal_cancel(SYS_SIG(SIGCONT));

    return item;
}

SYS_ASYNC_TO_DEFINE(__sys_io_release, ((sys_io_item_t *, item)))
{
    logT("SYS-IO: release %p", item);
    sys_calls_owned_release(&sys_async_owned_calls->head, (uintptr_t *)item);
}

void __sys_io_core(void)
{
    sys_io_item_t * item;

    item = sys_io_get(sys_io_config.loops);
    if (item == NULL)
    {
        item = sys_io_sleep(sys_io_config.worker_timeout);
        if (item == NULL)
        {
            logD("I/O Worker %u: No work to do", sys_io_id);

            if (sys_io_id == sys_io_count)
            {
                sys_io_exit = true;
            }

            return;
        }
    }

    logT("SYS-IO: execute %p", item);
    sys_calls_owned_execute((uintptr_t *)item);
}

void sys_io_loop(void)
{
    do
    {
        __sys_io_core();
    } while (!sys_io_exit);
}

void * sys_io_worker(void * arg);

void __sys_io_worker_create(void)
{
    sys_io_count++;

    SYS_CALL(
        sys_thread_create, (NULL, 0, -1, 0, 0, NULL, sys_io_worker, NULL),
        E(),
        LOG(E(), "Unable to create a thread for an I/O worker"),
        GOTO(failed)
    );

    return;

failed:
    sys_io_count--;

    atomic_store(&sys_io_busy, false, memory_order_seq_cst);
}

void sys_io_worker_create(void)
{
    if (sys_io_count >= sys_io_config.max_workers)
    {
        return;
    }

    if (atomic_xchg(&sys_io_busy, true, memory_order_seq_cst) != false)
    {
        logD("I/O worker creation cancelled. Another start/stop operation is "
             "being processed");

        return;
    }

    __sys_io_worker_create();
}

void * sys_io_worker(void * arg)
{
    sys_io_id = sys_io_count;
    sys_io_threads[sys_io_id] = pthread_self();

    logD("I/O worker %u: Ready", sys_io_id);

    if (sys_io_count < sys_io_config.min_workers)
    {
        __sys_io_worker_create();
    }
    else
    {
        atomic_store(&sys_io_busy, false, memory_order_seq_cst);
    }

    sys_io_loop();

    logD("I/O worker %u: Terminated", sys_io_id);

    return NULL;
}

static char * units[] = { "Bytes", "KiB", "MiB" };

err_t sys_io_initialize(sys_io_config_t * config)
{
    int32_t len, idx;
    uint64_t value;
    err_t error;
    char str[48];

    if (config != NULL)
    {
        sys_io_config = *config;
    }
    if (sys_io_config.max_workers <= 0)
    {
        sys_io_config.max_workers = 1;
    }
    if (sys_io_config.min_workers < 1)
    {
        sys_io_config.min_workers = 1;
    }
    if (sys_io_config.min_workers > sys_io_config.max_workers)
    {
        sys_io_config.min_workers = sys_io_config.max_workers;
    }
    sys_calls_config_prepare(&sys_io_config.queue);
    if (sys_io_config.worker_timeout < 5000)
    {
        sys_io_config.worker_timeout = 5000;
    }

    nlogD("sys-io", "I/O workers: min=%d, max=%d, timeout=%d, "
                    "queue size=%d, queues backlog=%d, "
                    "backlog preallocate=%d",
                    sys_io_config.min_workers,
                    sys_io_config.max_workers,
                    sys_io_config.worker_timeout,
                    sys_io_config.queue.size,
                    sys_io_config.queue.backlog_entries,
                    sys_io_config.queue.backlog_preallocate);

    value = sys_io_config.queue.backlog_preallocate *
            sys_io_config.queue.size *
            sys_io_config.min_workers * sys_io_config.min_workers;
    len = sizeof(str);
    sys_numfmt_units(str, &len, &idx, 0, 2, 1024, value, '.', 3, ',');
    nlogD("sys-io", "Minimum memory used for queues: %s %s",
                    str, units[idx]);

    value = sys_io_config.queue.backlog_entries *
            sys_io_config.queue.size *
            sys_io_config.max_workers * sys_io_config.max_workers;
    len = sizeof(str);
    sys_numfmt_units(str, &len, &idx, 0, 2, 1024, value, '.', 3, ',');
    nlogD("sys-io", "Maximum memory used for queues: %s %s",
                    str, units[idx]);

    sys_io_queue.first = NULL;
    sys_io_queue.plast = &sys_io_queue.first;

    SYS_CALLOC(
        &sys_io_threads, sys_io_config.max_workers + 1, sys_mt_pthread_t,
        E(),
        RETERR()
    );

    sys_io_count = 1;

    SYS_CALL(
        sys_thread_create, (NULL, 0, -1, 0, 0, NULL, sys_io_worker, NULL),
        E(),
        GOTO(failed, &error)
    );

    return 0;

failed:
    SYS_FREE(sys_io_threads);

    sys_io_count = 0;

    return error;
}
