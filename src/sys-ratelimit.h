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

#ifndef __SYS_RATELIMIT_H__
#define __SYS_RATELIMIT_H__

#include "sys-base.h"

typedef struct _sys_ratelimit
{
    int32_t   lock;
    int32_t   steps;
    int32_t   max;
    int32_t   counter;
    uintptr_t finish;
} sys_ratelimit_t;

#define SYS_RATELIMIT_INITIALIZER(_steps, _max) \
    { \
        .steps   = (_steps), \
        .max     = (_max) - 1, \
        .counter = 0, \
        .finish  = 0 \
    }

#define __SYS_RL(_rl, _steps, _max, _count, _acts...) \
    do \
    { \
        static sys_ratelimit_t _rl = SYS_RATELIMIT_INITIALIZER(_steps, _max); \
        if ((*(_count) = sys_ratelimit(&_rl)) < 0) \
        { \
            SYS_ACTIONS_DO(EBUSY, _acts); \
        } \
    } while (0)

#define _SYS_RL(_args...) __SYS_RL(_args)
#define SYS_RL(_steps, _max, _count, _acts...) \
    _SYS_RL(SYS_TMP(1), _steps, _max, _count, ## _acts)

#define __SYS_ONCE(_once, _acts...) \
    do \
    { \
        static bool _once = false; \
        if (!_once) \
        { \
            _once = true; \
        } \
        else \
        { \
            SYS_ACTIONS_DO(EBUSY, _acts); \
        } \
    } while (0)

#define _SYS_ONCE(_args...) __SYS_ONCE(_args)
#define SYS_ONCE(_acts...) _SYS_ONCE(SYS_TMP(1), ## _acts)

int32_t sys_ratelimit(sys_ratelimit_t * rl);

#endif /* __SYS_RATELIMIT_H */
