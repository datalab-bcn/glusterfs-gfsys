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

void sys_dirent_copy(gf_dirent_t ** dst, gf_dirent_t * src)
{
    gf_dirent_t * tmp;

retry:
    SYS_PTR(
        &tmp, gf_dirent_for_name, (src->d_name),
        ENOMEM,
        E(),
        GOTO(failed)
    );

    tmp->d_ino = src->d_ino;
    tmp->d_off = src->d_off;
    tmp->d_len = src->d_len;
    tmp->d_type = src->d_type;
    tmp->d_stat = src->d_stat;
    sys_dict_acquire(&tmp->dict, src->dict);
    sys_inode_acquire(&tmp->inode, src->inode);

    *dst = tmp;

    return;

failed:
    sys_memory_emergency_use();

    goto retry;
}

void sys_dirent_acquire(gf_dirent_t * dst, gf_dirent_t * src)
{
    gf_dirent_t * item, * tmp;

    INIT_LIST_HEAD(&dst->list);

    if (src != NULL)
    {
        list_for_each_entry(item, &src->list, list)
        {
            sys_dirent_copy(&tmp, item);
            list_add_tail(&tmp->list, &dst->list);
        }
    }
}

void sys_dirent_release(gf_dirent_t * src)
{
/*
    if (src != NULL)
    {
        gf_dirent_free(src);
    }
*/
}
