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

#ifndef __SYS_INODE_H__
#define __SYS_INODE_H__

static inline void sys_inode_acquire(inode_t ** dst, inode_t * src)
{
    if (src != NULL)
    {
        src = inode_ref(src);
    }
    *dst = src;
}

static inline void sys_inode_release(inode_t * src)
{
    if (src != NULL)
    {
        inode_unref(src);
    }
}

#endif /* __SYS_INODE_H__ */
