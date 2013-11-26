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

#ifndef __SYS_TSC_H__
#define __SYS_TSC_H__

#include "sys-time.h"

typedef struct _sys_tsc_config
{
    int64_t rdtsc_recheck_ms;
    int32_t gettime_delay;
} sys_tsc_config_t;

extern int64_t (* sys_tsc_time)(void);

int32_t sys_tsc_initialize(sys_tsc_config_t * config);

int64_t sys_tsc(void);
void sys_tsc_thread_initialize(void);
int32_t sys_tsc_calibrate(void);

#endif /* __SYS_TSC_H__ */
