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

#ifndef __SYS_TIME_H__
#define __SYS_TIME_H__

#include <time.h>

#include "sys-base.h"

#define SYS_TIME_IS_ABS(_time) (((_time)->tv_nsec & 1) == 0)
#define SYS_TIME_IS_REL(_time) (((_time)->tv_nsec & 1) == 1)

#define SYS_TIME_REL_INITIALIZER(_sec, _msec) \
    { .tv_sec = _sec, .tv_nsec = (_msec) * 1000000 + 1 }
#define SYS_TIME_ABS_INITIALIZER(_sec, _msec) \
    { .tv_sec = _sec, .tv_nsec = (_msec) * 1000000 + 0 }

extern uintptr_t sys_time_steps;

typedef struct timespec sys_time_t;

void sys_time_copy(sys_time_t * dst, sys_time_t * src);
void sys_time_add(sys_time_t * dst, sys_time_t * src);
void sys_time_sub(sys_time_t * dst, sys_time_t * src);
int32_t sys_time_cmp(sys_time_t * dst, sys_time_t * src);
void sys_time_now(sys_time_t * time);
void sys_time_abs(sys_time_t * time);
void sys_time_set_abs(sys_time_t * time, int64_t sec, int32_t msec);
void sys_time_set_rel(sys_time_t * time, int64_t sec, int32_t msec);
void sys_time_fix(sys_time_t ** time, sys_time_t * aux);
void sys_time_fix_null(sys_time_t ** time, sys_time_t * aux);
int64_t sys_time_serial(sys_time_t * time);
void sys_time_from_serial_rel(sys_time_t * time, int64_t serial);
void sys_time_from_serial_abs(sys_time_t * time, int64_t serial);

#ifdef _USE_TSC

void sys_time_fast_mark(void);

#else


#endif

#endif /* __SYS_TIME_H__ */
