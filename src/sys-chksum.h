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

#ifndef __SYS_CHKSUM_H__
#define __SYS_CHKSUM_H__

static inline void sys_chksum_acquire(uint8_t ** dst, uint8_t * src)
{
    uint8_t * tmp = NULL;

    if (src != NULL)
    {
        SYS_ALLOC(
            &tmp, 16, sys_mt_uint8_t,
            E(),
            NO_FAIL()
        );
        memcpy(tmp, src, 16);
    }
    *dst = tmp;
}

static inline void sys_chksum_release(uint8_t * src)
{
    SYS_FREE(src);
}

#endif /* __SYS_CHKSUM_H__ */
