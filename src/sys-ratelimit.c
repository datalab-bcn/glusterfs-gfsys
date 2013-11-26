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

// burst and interval fields are considered aproximate. If there is contention
// this function could return 1 more than burst times in the given interval.
int32_t sys_ratelimit(sys_ratelimit_t * rl)
{
    int32_t count;

    if (rl->finish > sys_time_steps)
    {
        count = atomic_dec_return(&rl->counter, memory_order_seq_cst) > 0;
        return --count;
    }
    count = -atomic_xchg(&rl->counter, rl->max, memory_order_seq_cst);
    if (count < 0)
    {
        count = 0;
    }
    rl->finish = sys_time_steps + rl->steps;

    return count;
}
