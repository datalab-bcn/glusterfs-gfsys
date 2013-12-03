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

SYS_ASYNC_CREATE(test, ((int32_t, depth)))
{
//    if (depth < 67108864)
//    logD("test: %u", depth);
    if (depth < 25)
    {
        SYS_ASYNC(test, (depth + 1));
        SYS_ASYNC(test, (depth + 1));
    }
}

SYS_DELAY_CREATE(timer, ((int64_t, ms), (uintptr_t *, data)))
{
    err_t error;
    static int count = 7;

    error = sys_thread_get_error();
    if (error == ETIMEDOUT)
    {
        logD("Timer fired (%ld, %p)", ms, data);

        if (--count == 0)
        {
            logD("Cancelling check");
            sys_delay_cancel(data, true);
        }

        SYS_DELAY(ms, timer, (ms, data));
    }
    else
    {
        logD("Timer ended: error=%d", error);
    }
}

SYS_DELAY_CREATE(check_timer, ())
{
    err_t error;

    error = sys_thread_get_error();
    if (error == ETIMEDOUT)
    {
        logD("Time check");
    }
    else
    {
        logD("Cancelled: %d", error);
    }
}

static sys_lock_t lock;

SYS_DELAY_CREATE(unlock, ((int, id)))
{
    logD("%u: Unlocking", id);

    SYS_UNLOCK(&lock);
}

SYS_LOCK_CREATE(lock_func, ((int, id)))
{
    logD("%u: Lock acquired", id);

    SYS_DELAY(1000, unlock, (id));
}

SYS_DELAY_CREATE(lock_func_failed, ())
{
    logD("Lock acquisition failed");
}

SYS_DELAY_CREATE(try_lock, ((int, id)))
{
    logD("%u: Trying lock", id);

    SYS_LOCK(&lock, lock_func, (id), SYS_DELAY(2000, lock_func_failed, (), 1));
}
/*
SYS_DELAY_CREATE(sm_test, (SYS_SM_STATE(state)))
{
    static int32_t stop = 0;
    switch (state)
    {
        case -1:
            logD("sm_test: stop request");
            stop = 1;
            break;
        case 0:
            logD("sm_test: State 0");
            if (stop)
            {
                SYS_SM_ASYNC(10);
            }
            else
            {
                SYS_SM_LOCK(&lock, 5000, 1, 10);
            }
            break;
        case 1:
            logD("sm_test: State 1");
            if (stop)
            {
                SYS_SM_ASYNC(9);
            }
            else
            {
                SYS_SM_DELAY(1000, 2, 9);
            }
            break;
        case 2:
            logD("sm_test: State 2");
            SYS_UNLOCK(&lock);
            if (stop)
            {
                SYS_SM_ASYNC(10);
            }
            else
            {
                SYS_SM_DELAY(1000, 0, 10);
            }
            break;
        case 9:
            logD("sm_test: State 9");
            SYS_UNLOCK(&lock);
        case 10:
            logD("sm_test: State 10");
            sys_sm_completed(sys_thread_get_error());
            break;
        default:
            logD("sm_test: Invalid state %d", state);
            sys_sm_completed(EINVAL);
    }
}

SYS_DELAY_CREATE(sm_speed, (SYS_SM_STATE(state)))
{
    static uint64_t count = 0;
    static int32_t stop = 0;

    if (state == -3)
    {
        sys_sm_completed(0);
        return;
    }
    if (state == -2)
    {
        logD("SM count: %lu", count);
        if (!stop)
        {
            SYS_SM_DELAY(1000, -2, -1);
        }
        else
        {
            SYS_SM_DELAY(1000, -3, -3);
        }
        return;
    }
    if (state == -1)
    {
        logD("SM shutdown");
        stop = 1;
        return;
    }
    if (!stop)
    {
        if (state == 0)
        {
            logD("SM initialized");
            SYS_SM_DELAY(1000, -2, -1);
        }
        count++;
        SYS_SM_ASYNC(count);
    }
    else
    {
        logD("SM Shutting down");
    }
}

SYS_DELAY_CREATE(sm_done, ())
{
    logD("State machine finished: %d", sys_thread_get_error());
}
*/

SYS_DELAY_DECLARE(io_done, ((uintptr_t *, io), (int32_t, arg1), (int32_t, arg2)))

SYS_ASYNC_CREATE(io_launch, ((int32_t, time), (uintptr_t *, io), (int32_t, arg1), (int32_t, arg2)))
{
    logD("executing: %u, %p, %u, %u", time, io, arg1, arg2);

    SYS_DELAY(time, io_done, (io, 23, 14));
}

SYS_IO_CREATE(io_func, op, ((int32_t, time)), ((uintptr_t *, io), (int32_t, arg1), (int32_t, arg2)))
{
    logD("Initiating I/O: %u", time);

    SYS_ASYNC(io_launch, (time, op, 23, 14));
}

SYS_IO_CBK_CREATE(io_func, void, ((uintptr_t *, io), (int32_t, arg1), (int32_t, arg2)))
{
    logD("I/O done: %p, %u, %u", io, arg1, arg2);

    SYS_IO_RESUME(io_func, io, 0, io, arg1, arg2);
}

SYS_DELAY_DEFINE(io_done, ((uintptr_t *, io), (int32_t, arg1), (int32_t, arg2)))
{
    logD("io_done: %p, %u, %u", io, arg1, arg2);

    recv_io_func(io, arg1, arg2);
}

SYS_DELAY_CREATE(io_notify, ())
{
    logD("I/O notification: %u", sys_thread_get_error());
}

SYS_CBK_CREATE(io_cbk, data, ((int32_t, arg1)))
{
    logD("%u: I/O callback: %p", arg1, data);

    sys_io_release(data);
}

static struct list_head list;

static struct list_head items[1000];

SYS_ASYNC_CREATE(rcu_check, ())
{
    struct list_head * item;
    int32_t count;

    count = 0;

    sys_rcu_read_lock();

    for (item = sys_rcu_dereference(list.next); item != &list;
         item = sys_rcu_dereference(item->next))
    {
        count++;
    }

    sys_rcu_read_unlock();

//    logD("RCU check: count = %u", count);

    if (count > 0)
    {
        SYS_ASYNC(rcu_check, ());
    }
    else
    {
        logD("RCU check completed");
    }
}

SYS_RCU_CREATE(__rcu_del, ((struct list_head *, item)))
{
    item->next = NULL;
    item->prev = NULL;
}

SYS_ASYNC_CREATE(rcu_del, ())
{
    struct list_head * item, * next;

    item = sys_rcu_dereference(list.next);
    if (item != &list)
    {
        next = sys_rcu_dereference(item->next);
        next->prev = &list;
        sys_rcu_assign_pointer(list.next, next);

        SYS_RCU(__rcu_del, (item));

        SYS_ASYNC(rcu_del, ());
    }
}

SYS_ASYNC_CREATE(start_timer, ((int64_t, ms)))
{
    uintptr_t * data;
    int32_t i;

    logD("Starting timer");

    data = SYS_DELAY(8000, check_timer, (), 1);
    logD("timer = %p", data);
    SYS_LOCK(&lock, lock_func, (1), SYS_DELAY(2000, lock_func_failed, (), 1));
    SYS_DELAY(ms, timer, (ms, data));
    SYS_DELAY(250, try_lock, (2));
    SYS_DELAY(500, try_lock, (3));
    SYS_DELAY(750, try_lock, (4));
    SYS_DELAY(1000, try_lock, (5));
    SYS_DELAY(1250, try_lock, (6));
    SYS_IO(io_func, (10000), SYS_CBK(io_cbk, (13)), SYS_DELAY(5000, io_notify, (), 1));
    SYS_IO(io_func, (8000), SYS_CBK(io_cbk, (50)), SYS_DELAY(15000, io_notify, (), 1));

    INIT_LIST_HEAD(&list);
    for (i = 0; i < 1000; i++)
    {
        list_add_tail(&items[i], &list);
    }

    SYS_ASYNC(rcu_check, ());
    SYS_ASYNC(rcu_check, ());
    SYS_ASYNC(rcu_check, ());
    SYS_ASYNC(rcu_check, ());

    SYS_ASYNC(rcu_del, ());

//    logD("Initiating SM");
//    SYS_SM(sm_test, (0), 20000, sm_done, ());
}
/*
SYS_ASYNC_CREATE(speed_test, ())
{
    SYS_SM(sm_speed, (0), 10000, sm_done, ());
}
*/

SYS_ASYNC_CREATE(test_calls, ((void, data, CALLS)))
{
    sys_calls_node_t * node;

    node = (sys_calls_node_t *)(data - SYS_CALLS_NODE_SIZE);
    logD("node: %p", node);
    logD("Calls: %p", data);
    logD("%p - %p", node->callback, SYS_PROXY(test_calls));
}

int main(int argc, char * argv[])
{
    err_t err;
//    int32_t i;
//    sys_async_thread_t * thread[5];

    err = 1;

    SYS_CALL(
        gfsys_initialize, (NULL, true),
        E(),
        RETVAL(1)
    );

    sys_lock_initialize(&lock);

    sleep(5);

    sys_async_dump();
/*
    for (i = 0; i < 4; i++)
    {
        SYS_CALL(
            sys_async_thread_create, (&thread[i], i + 2, (i < 3) ? i + 1 : -1),
            E(),
            GOTO(async_terminate)
        );

        sleep(1);

        sys_async_dump();
    }
*/
    sleep(5);

//    SYS_ASYNC(speed_test, ());

    sleep(10);

    logI("Initiating test");
    SYS_ASYNC(test_calls, (NULL));
    SYS_ASYNC(test, (0));
    SYS_ASYNC(start_timer, (1000));

    sleep(60);

    logI("Showing statistics");
    sys_async_statistics();

    sleep(15);

    logI("Done");

    *(int *)0 = 0;

    err = 0;

//async_terminate:
//    sys_async_terminate();
thread_terminate:
    sys_thread_terminate();
gf_terminate:
    sys_gf_terminate();

    return err;
}
