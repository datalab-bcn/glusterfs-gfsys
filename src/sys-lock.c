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

void sys_lock_initialize(sys_lock_t * lock)
{
    lock->first = NULL;
    lock->plast = &lock->first;
}

void sys_lock_terminate(sys_lock_t * lock)
{
    SYS_ASSERT(lock->first == NULL, "Destroying a lock in use");
}

SYS_ASYNC_CREATE(sys_lock_release, ((sys_lock_t *, lock)))
{
    sys_lock_item_t * entry, * next;

    entry = lock->first;
    do
    {
        next = entry->next;
        lock->first = next;
        if (next == NULL)
        {
            if (unlikely(!atomic_cmpxchg(&lock->plast, &entry->next,
                                         &lock->first, memory_order_seq_cst,
                                         memory_order_seq_cst)))
            {
                lock->first = entry;
                SYS_ASYNC(sys_lock_release, (lock));
            }
            else
            {
                sys_delay_release((uintptr_t *)entry);
            }

            return;
        }

        sys_delay_release((uintptr_t *)entry);

        entry = next;
    } while (!sys_delay_execute((uintptr_t *)entry, 0));
}
