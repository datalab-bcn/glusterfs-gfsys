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

err_t gfsys_initialize(gfsys_config_t * config, bool init_gf)
{
    if (init_gf)
    {
        SYS_CALL(
            sys_gf_initialize, (config ? &config->gf : NULL),
            E(),
            RETERR()
        );
    }

    SYS_CALL(
        sys_thread_initialize, (),
        E(),
        RETERR()
    );
    SYS_CALL(
        sys_tsc_initialize, (config ? &config->tsc : NULL),
        E(),
        RETERR()
    );
    SYS_CALL(
        sys_async_initialize, (config ? &config->async : NULL),
        E(),
        RETERR()
    );
    SYS_CALL(
        sys_io_initialize, (config ? &config->io : NULL),
        E(),
        RETERR()
    );

    return 0;
}

