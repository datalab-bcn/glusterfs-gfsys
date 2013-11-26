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

/*
 * WARNING:
 *
 *   By default, Linux uses an optimistic memory allocation strategy that
 *   allows most of the memory allocation calls to succeed as long as the
 *   virtual memory space is not exhausted, even if there isn't enough
 *   physical memory at the moment. This is known as memory overcommit.
 *
 *   The idea behind this is that the memory is not really allocated when
 *   it is requested, only the gap on the virtual address space is reserved.
 *   When the application accesses any reserved but unallocated space for
 *   the first time, the kernel tries to find a free memory (or swap) page
 *   to store the data. If that is not possible because there is not enough
 *   memory, it invokes the OOM-Killer that selects a task and kills it to
 *   free some memory and be able to satisfy the request (this task could
 *   be the very process that is trying to access the memory it has
 *   allocated).
 *
 *   In normal conditions this could be very useful, but probably we won't
 *   have any easily detectable warning of the low memory condition before
 *   the OOM-Killer kills us.
 *
 *   To take advantage of the emergency memory pool features it would be
 *   necessary to set /proc/sys/vm/overcommit_memory to 2 (and optionally
 *   configure the value of /proc/sys/vm/overcommit_ratio). This will disable
 *   the kernel overcommit feature and the OOM-Killer, returning NULL in
 *   memory allocation requests that cannot be satisfied at the time of the
 *   call.
 *
 *   Detecting these conditions will allow the emergency pool to fire events
 *   when resources are getting very low. Using these events, other system
 *   components can take actions to try to reduce memory usage, restart the
 *   application if the condition is critical, or try to do an as clean as
 *   possible shutdown, without risking beeing killed or killing other
 *   important processes.
 */

/*
 * This emergency memory pool is very simple: it initially allocates a chunk
 * of memory. When any of the generic allocation functions detects an error
 * requesting memory, it will release half of the memory and retry the
 * operation until the emergency pool is exhausted. At this point the
 * process will be killed.
 *
 * Every time that half of the emergency pool is released, an event is fired
 * to let other components know that the memory condition is dangerous.
 *
 * At the time, the emergency memory pool is not recovered once it begins
 * to release some of its parts, even if a lot of memory is released.
 */

SYS_EVENT_DEFINE(sys_evt_low_memory);
SYS_EVENT_DEFINE(sys_evt_very_low_memory);
SYS_EVENT_DEFINE(sys_evt_critical_memory);

static sys_mutex_t sys_memory_lock;
static sys_cond_t sys_memory_cond;

static void * sys_memory_emergency = NULL;
static size_t sys_memory_size = 0;
static int32_t sys_memory_level = 0;
static int32_t sys_memory_running = 0;

static sys_event_t * sys_memory_events[] =
{
    &sys_evt_low_memory,
    &sys_evt_very_low_memory,
    &sys_evt_critical_memory
};

err_t sys_memory_emergency_initialize(size_t size)
{
    err_t error;

    SYS_ALLOC(
        &sys_memory_emergency, size, sys_mt_emergency,
        E(),
        RETERR()
    );

    memset(sys_memory_emergency, 0xaa, size);

    SYS_CALL(
        sys_cond_mutex_initialize, (&sys_memory_cond, &sys_memory_lock),
        E(),
        GOTO(failed, &error)
    );

    sys_memory_size = size;
    sys_memory_running = 0;
    sys_memory_level = 0;

    return 0;

failed:
    SYS_FREE(sys_memory_emergency);

    return error;
}

void sys_memory_emergency_terminate(void)
{
    sys_cond_mutex_terminate(&sys_memory_cond, &sys_memory_lock);
    SYS_FREE(sys_memory_emergency);
}

void sys_memory_emergency_use(void)
{
    int32_t level;

    sys_mutex_lock(&sys_memory_lock);

    if (sys_memory_running == 1)
    {
        sys_cond_wait_expr(&sys_memory_cond, &sys_memory_lock,
                           sys_memory_running == 0);

        sys_mutex_unlock(&sys_memory_lock);

        return;
    }
    else
    {
        sys_memory_running = 1;
    }

    sys_mutex_unlock(&sys_memory_lock);

    sys_memory_size >>= 1;
    SYS_TEST(
        sys_memory_size >= 4096,
        ENOMEM,
        C(),
        ASSERT("Emergency memory pool is empty.")
    );
    SYS_RESIZE(
        &sys_memory_emergency, sys_memory_size, sys_mt_emergency,
        C(),
        ASSERT("Emergency memory pool resize failed.")
    );

    sys_mutex_lock(&sys_memory_lock);

    sys_memory_running = 0;
    sys_cond_broadcast(&sys_memory_cond);

    sys_mutex_unlock(&sys_memory_lock);

    level = atomic_inc(&sys_memory_level, memory_order_seq_cst);
    if (level < count_of(sys_memory_events))
    {
        sys_event_fire(sys_memory_events[level]);
    }
}
