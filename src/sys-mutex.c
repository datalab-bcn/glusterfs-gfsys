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

int32_t sys_mutex_initialize(sys_mutex_t * mutex)
{
    pthread_mutexattr_t attr;
    int32_t error = 0;

    SYS_CALL(
        pthread_mutexattr_init, (&attr),
        Ws(),
        RETERR()
    );
    SYS_CHAIN(
        &error,
        SYS_CALL(
            pthread_mutexattr_settype, (&attr, PTHREAD_MUTEX_ADAPTIVE_NP),
            Ws()
        )
    );
    SYS_CHAIN(
        &error,
        SYS_CALL(
            pthread_mutexattr_setprotocol, (&attr, PTHREAD_PRIO_INHERIT),
            Ws()
        )
    );
    SYS_CHAIN(
        &error,
        SYS_CALL(
            pthread_mutex_init, (mutex, &attr),
            Ws()
        )
    );
    SYS_CALL(
        pthread_mutexattr_destroy, (&attr),
        Es(),
        ASSERT("Invalid mutex attributes")
    );

    return error;
}

void sys_mutex_terminate(sys_mutex_t * mutex)
{
    SYS_CALL(
        pthread_mutex_destroy, (mutex),
        Es(),
        ASSERT("Mutex is invalid or it's in an invalid state")
    );
}

void sys_mutex_lock(sys_mutex_t * mutex)
{
    SYS_CALL(
        pthread_mutex_lock, (mutex),
        Es(),
        ASSERT("Mutex is invalid or it's in an invalid state")
    );
}

int32_t sys_mutex_timedlock(sys_mutex_t * mutex, sys_time_t * time)
{
    sys_time_t timeout;
    int32_t error;

    sys_time_fix(&time, &timeout);

    return SYS_FUNC(
               &error,
               pthread_mutex_timedlock, (mutex, time),
               (error == 0) || (error == ETIMEDOUT),
               error,
               Es(),
               ASSERT("Mutex is invalid or it's in an invalid state")
           );
}

void sys_mutex_unlock(sys_mutex_t * mutex)
{
    SYS_CALL(
        pthread_mutex_unlock, (mutex),
        Es(),
        ASSERT("Mutex is invalid or it's in an invalid state")
    );
}
