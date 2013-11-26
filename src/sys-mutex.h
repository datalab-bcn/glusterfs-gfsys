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

#ifndef __SYS_MUTEX_H__
#define __SYS_MUTEX_H__

#include <pthread.h>

#include "sys-error.h"
#include "sys-time.h"

#define SYS_MUTEX_INITIALIZER PTHREAD_MUTEX_INITIALIZER

typedef pthread_mutex_t sys_mutex_t;

int32_t sys_mutex_initialize(sys_mutex_t * mutex);
void sys_mutex_terminate(sys_mutex_t * mutex);
void sys_mutex_lock(sys_mutex_t * mutex);
int32_t sys_mutex_timedlock(sys_mutex_t * mutex, sys_time_t * time);
void sys_mutex_unlock(sys_mutex_t * mutex);

#endif /* __SYS_MUTEX_H__ */
