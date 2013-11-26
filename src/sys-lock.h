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

#ifndef __SYS_LOCK_H__
#define __SYS_LOCK_H__

typedef struct _sys_lock_item
{
    struct _sys_lock_item * next;
} sys_lock_item_t;

typedef struct _sys_lock
{
    sys_lock_item_t *  first;
    sys_lock_item_t ** plast;
} sys_lock_t;

#define SYS_LOCK_SIZE SYS_CALLS_ADJUST_SIZE(sizeof(sys_lock_item_t))

#define SYS_LOCK_DECLARE(_name, _args) \
    SYS_CALLS_DECLARE(_name, _args); \
    SYS_CALLS_WRAPPER(_name, _args, ((sys_lock_t *, __sys_lock_lock), \
                                     (int64_t, __sys_lock_ms))) \
    { \
        uintptr_t * __sys_lock_data; \
        __sys_lock_data = sys_delay_prepare(SYS_PROXY(_name), SYS_LOCK_SIZE, \
                                            SYS_CALLS_SIZE(_name), 1); \
        SYS_MARSHALL(__sys_lock_data + SYS_LOCK_SIZE, _name, _args); \
        sys_lock_acquire(__sys_lock_lock, __sys_lock_ms, __sys_lock_data); \
        return __sys_lock_data + SYS_LOCK_SIZE; \
    }

#define SYS_LOCK_DEFINE(_name, _args) SYS_CALLS_DEFINE(_name, _args)

#define SYS_LOCK_CREATE(_name, _args) \
    SYS_LOCK_DECLARE(_name, _args); \
    SYS_LOCK_DEFINE(_name, _args)

#define SYS_LOCK(_lock, _ms, _name, _args) \
    SYS_WRAP(_name, (_lock, _ms), _args)

#define SYS_UNLOCK(_lock) sys_lock_release(_lock)

void sys_lock_initialize(sys_lock_t * lock);
void sys_lock_terminate(sys_lock_t * lock);

static inline void sys_lock_acquire(sys_lock_t * lock, int64_t ms,
                                    uintptr_t * data)
{
    sys_lock_item_t * item, ** plast;

    item = (sys_lock_item_t *)data;
    item->next = NULL;
    plast = atomic_xchg(&lock->plast, &item->next, memory_order_seq_cst);
    *plast = item;

    if (plast == &lock->first)
    {
        __sys_delay_execute(data, 0);
    }
    else
    {
        sys_delay_commit(data, ms);
    }
}

void sys_lock_release(sys_lock_t * lock);

#endif /* __SYS_LOCK_H__ */
