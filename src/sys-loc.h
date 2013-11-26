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

#ifndef __SYS_LOC_H__
#define __SYS_LOC_H__

static inline void sys_loc_acquire(loc_t * dst, loc_t * src)
{
    if (src != NULL)
    {
again:
        SYS_CODE(
            loc_copy, (dst, src),
            ENOMEM,
            E(),
            GOTO(retry)
        );
    }
    else
    {
        memset(dst, 0, sizeof(*dst));
    }

    return;

retry:
    sys_memory_emergency_use();

    goto again;
}

static inline void sys_loc_release(loc_t * src)
{
    loc_wipe(src);
}

#endif /* __SYS_LOC_H__ */
