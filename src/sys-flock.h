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

#ifndef __SYS_FLOCK_H__
#define __SYS_FLOCK_H__

static inline void sys_flock_acquire(struct gf_flock * dst,
                                     const struct gf_flock * src)
{
    if (src != NULL)
    {
        dst->l_type = src->l_type;
        dst->l_whence = src->l_whence;
        dst->l_start = src->l_start;
        dst->l_len = src->l_len;
        dst->l_pid = src->l_pid;
        dst->l_owner.len = src->l_owner.len;
        memcpy(dst->l_owner.data, src->l_owner.data, src->l_owner.len);
    }
    else
    {
        dst->l_type = 0;
        dst->l_whence = 0;
        dst->l_start = 0;
        dst->l_len = 0;
        dst->l_pid = 0;
        dst->l_owner.len = 0;
    }
}

static inline void sys_flock_release(struct gf_flock * src)
{
}

#endif /* __SYS_FLOCK_H__ */
