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

#ifndef __SYS_NUMFMT_H__
#define __SYS_NUMFMT_H__

err_t sys_numfmt_dec(char * buffer, int32_t * size,
                     uint64_t number, char group);
err_t sys_numfmt_units(char * buffer, int32_t * size, int32_t * scale,
                       int32_t min_scale, int32_t max_scale, int32_t factor,
                       uint64_t number, char group, int32_t decimals,
                       char point);

#endif /* __SYS_NUMFMT_H__ */
