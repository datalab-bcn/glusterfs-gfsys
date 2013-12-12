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

#ifndef __SYS_H__
#define __SYS_H__

typedef struct _gfsys_config
{
    sys_gf_config_t    gf;
    sys_tsc_config_t   tsc;
    sys_async_config_t async;
    sys_io_config_t    io;

} gfsys_config_t;

err_t gfsys_initialize(gfsys_config_t * config, bool init_gf);
void gfsys_terminate(void);

#endif /* __SYS_H__ */
