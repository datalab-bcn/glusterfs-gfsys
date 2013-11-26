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

err_t sys_cond_initialize(sys_cond_t * cond)
{
    return SYS_CALL(
               pthread_cond_init, (cond, NULL),
               Ws()
           );
}

err_t sys_cond_mutex_initialize(sys_cond_t * cond, sys_mutex_t * mutex)
{
    err_t error;

    SYS_CALL(
        sys_mutex_initialize, (mutex),
        Ws(),
        RETERR()
    );
    SYS_CALL(
        sys_cond_initialize, (cond),
        Ws(),
        GOTO(failed, &error)
    );

    return 0;

failed:
    sys_mutex_terminate(mutex);

    return error;
}

void sys_cond_terminate(sys_cond_t * cond)
{
    SYS_CALL(
        pthread_cond_destroy, (cond),
        Es(),
        ASSERT("Condition is invalid or it's in an invalid state")
    );
}

void sys_cond_mutex_terminate(sys_cond_t * cond, sys_mutex_t * mutex)
{
    sys_cond_terminate(cond);
    sys_mutex_terminate(mutex);
}

void sys_cond_wait(sys_cond_t * cond, sys_mutex_t * mutex)
{
    SYS_CALL(
        pthread_cond_wait, (cond, mutex),
        Es(),
        ASSERT("Condition is invalid or it's in an invalid state")
    );
}

err_t sys_cond_timedwait(sys_cond_t * cond, sys_mutex_t * mutex, \
                         sys_time_t * time)
{
    sys_time_t timeout;
    err_t error;

    sys_time_fix(&time, &timeout);

    SYS_FUNC(
        &error,
        pthread_cond_timedwait, (cond, mutex, time),
        (error == 0) || (error == ETIMEDOUT),
        error,
        Es(),
        ASSERT("Condition is invalid or it's in an invalid state")
    );

    return error;
}

void sys_cond_signal(sys_cond_t * cond)
{
    SYS_CALL(
        pthread_cond_signal, (cond),
        Es(),
        ASSERT("Condition is invalid or it's in an invalid state")
    );
}

void sys_cond_broadcast(sys_cond_t * cond)
{
    SYS_CALL(
        pthread_cond_broadcast, (cond),
        Es(),
        ASSERT("Condition is invalid or it's in an invalid state")
    );
}
