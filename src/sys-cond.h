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

#ifndef __SYS_COND_H__
#define __SYS_COND_H__

#include <pthread.h>

#include "sys-base.h"
#include "sys-error.h"
#include "sys-time.h"
#include "sys-mutex.h"

#define SYS_COND_INITIALIZER PTHREAD_COND_INITIALIZER

typedef pthread_cond_t sys_cond_t;

#define sys_cond_wait_expr(_cond, _mutex, _expr) \
    do \
    { \
        while (!(_expr)) \
        { \
            sys_cond_wait(_cond, _mutex); \
        } \
    } while (0)

#define __sys_cond_timedwait_expr(_err, _aux, _ptr, _cond, _mutex, _time, \
                                  _expr) \
    ({ \
        err_t _err = 0; \
        sys_time_t _aux; \
        sys_time_t * _ptr = (_time); \
        sys_time_fix(&_ptr, &_aux); \
        while (likely(_err == 0) && !(_expr)) \
        { \
            _err = sys_cond_timedwait(_cond, _mutex, _ptr); \
        } \
        _err; \
    })
#define _sys_cond_timedwait_expr(_args...) __sys_cond_timedwait_expr(_args)
#define sys_cond_timedwait_expr(_cond, _mutex, _time, _expr) \
    _sys_cond_timedwait_expr(SYS_TMP(3), _cond, _mutex, _time, _expr)

err_t sys_cond_initialize(sys_cond_t * cond);
err_t sys_cond_mutex_initialize(sys_cond_t * cond, sys_mutex_t * mutex);
void sys_cond_terminate(sys_cond_t * cond);
void sys_cond_mutex_terminate(sys_cond_t * cond, sys_mutex_t * mutex);
void sys_cond_wait(sys_cond_t * cond, sys_mutex_t * mutex);
err_t sys_cond_timedwait(sys_cond_t * cond, sys_mutex_t * mutex, \
                         sys_time_t * time);
void sys_cond_signal(sys_cond_t * cond);
void sys_cond_broadcast(sys_cond_t * cond);

#endif /* __SYS_COND_H__ */
