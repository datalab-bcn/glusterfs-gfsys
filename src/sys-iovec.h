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

#ifndef __SYS_IOVEC_H__
#define __SYS_IOVEC_H__

typedef struct _sys_iovec
{
    int32_t        count;
    struct iovec * iovec;
} sys_iovec_t;

static inline void sys_iovec_acquire(sys_iovec_t * dst, struct iovec * src, int32_t count)
{
    if (src != NULL)
    {
        dst->count = count;
        SYS_ALLOC(
            &dst->iovec, count * sizeof(struct iovec), sys_mt_iovec,
            E(),
            NO_FAIL()
        );

        memcpy(dst->iovec, src, count * sizeof(struct iovec));
    }
    else
    {
        dst->count = 0;
        dst->iovec = NULL;
    }
}

static inline void sys_iovec_release(sys_iovec_t * src)
{
    if ((src != NULL) && (src->iovec != NULL))
    {
        SYS_FREE(src->iovec);
    }
}

#endif /* __SYS_IOVEC_H__ */
