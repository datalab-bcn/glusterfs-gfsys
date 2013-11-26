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

uintptr_t sys_time_steps;

void sys_time_copy(sys_time_t * dst, sys_time_t * src)
{
    dst->tv_sec = src->tv_sec;
    dst->tv_nsec = src->tv_nsec;
}

void sys_time_adjust(sys_time_t * time)
{
    if (time->tv_sec >= 0)
    {
        if (time->tv_nsec >= 1000000000)
        {
            time->tv_nsec -= 1000000000;
            time->tv_sec++;
        }
        else
        {
            while ((time->tv_sec > 0) && (time->tv_nsec < 0))
            {
                time->tv_nsec += 1000000000;
                time->tv_sec--;
            }

        }
    }
    else
    {
        if (time->tv_nsec <= -1000000000)
        {
            time->tv_nsec += 1000000000;
            time->tv_sec--;
        }
        else
        {
            while ((time->tv_sec < 0) && (time->tv_nsec > 0))
            {
                time->tv_nsec -= 1000000000;
                time->tv_sec++;
            }
        }
    }

    SYS_TEST(
        SYS_TIME_IS_REL(time) || ((time->tv_nsec >= 0) && (time->tv_sec >= 0)),
        EINVAL,
        E(),
        ASSERT("Invalid absolute time (%ld, %ld)", time->tv_sec, time->tv_nsec)
    );
}

void sys_time_add(sys_time_t * dst, sys_time_t * src)
{
    if (src != NULL)
    {
        SYS_TEST(
            (dst->tv_nsec | src->tv_nsec) & 1,
            EINVAL,
            E(),
            ASSERT("Trying to add two absolute times (%ld, %ld) + (%ld, %ld)",
                   dst->tv_sec, dst->tv_nsec, src->tv_sec, src->tv_nsec)
        );

        dst->tv_nsec += src->tv_nsec & ~1;
        dst->tv_nsec &= src->tv_nsec | ~1;
        dst->tv_sec += src->tv_sec;

        sys_time_adjust(dst);
    }
}

void sys_time_sub(sys_time_t * dst, sys_time_t * src)
{
    if (src != NULL)
    {
        SYS_TEST(
            SYS_TIME_IS_ABS(dst) || SYS_TIME_IS_REL(src),
            EINVAL,
            E(),
            ASSERT("Trying to substract an absolute time from a relative "
                   "time (%ld, %ld) - (%ld, %ld)", dst->tv_sec, dst->tv_nsec,
                   src->tv_sec, src->tv_nsec)
        );

        dst->tv_nsec -= src->tv_nsec & ~1;
        dst->tv_nsec |= (src->tv_nsec & 1) ^ 1;
        dst->tv_sec -= src->tv_sec;

        sys_time_adjust(dst);
    }
}

int32_t sys_time_cmp(sys_time_t * dst, sys_time_t * src)
{
    SYS_TEST(
        SYS_TIME_IS_ABS(dst) ^ SYS_TIME_IS_REL(src),
        EINVAL,
        E(),
        ASSERT("Comparing absolute and relative times")
    );

    if (dst->tv_sec < src->tv_sec)
    {
        return -1;
    }
    if (dst->tv_sec > src->tv_sec)
    {
        return 1;
    }
    if (dst->tv_nsec < src->tv_nsec)
    {
        return -1;
    }
    if (dst->tv_nsec > src->tv_nsec)
    {
        return 1;
    }
    return 0;
}

#ifdef _USE_TSC

static uint64_t sys_time_calibrate_nsec = 0;
static uint64_t sys_time_calibrate_ticks = 0;
static uint64_t sys_time_calibrate_ticks_per_ms = 0;

void __sys_time_calibrate(sys_time_t * time)
{
    uint64_t ticks, nsec, ms;

    ticks = sys_tsc() - sys_time_calibrate_ticks;
    nsec = (uint64_t)time->tv_sec * 1000000000ULL + time->tv_nsec - sys_time_calibrate_nsec;

    if (nsec >= 1000000000ULL)
    {
        ms = ticks * 1000000 / nsec;
        if (sys_time_calibrate_ticks_per_ms != ms)
        {
            logW("Unstable TSC (%lu - %lu)", sys_time_calibrate_ticks_per_ms, ms);
            sys_time_calibrate_ticks_per_ms = ms;
        }
        sys_time_calibrate_nsec = nsec;
        sys_time_calibrate_ticks = ticks;
    }
}

#else
#define __sys_time_calibrate(_time) do { } while (0)
#endif

void sys_time_now(sys_time_t * time)
{
    SYS_ERRNO0(
        clock_gettime, (CLOCK_MONOTONIC, time),
        E(),
        ASSERT("System time is unavailable")
    );
    __sys_time_calibrate(time);
    time->tv_nsec &= ~1;
}

void sys_time_abs(sys_time_t * time)
{
    sys_time_t now;

    if ((time != NULL) && SYS_TIME_IS_REL(time))
    {
        sys_time_now(&now);
        sys_time_add(time, &now);
    }
}

void sys_time_set_abs(sys_time_t * time, int64_t sec, int32_t msec)
{
    SYS_TEST(
        (sec >= 0) && (msec >= 0) && (msec < 1000),
        EINVAL,
        E(),
        ASSERT("Invalid absolute time (%ld, %d)", sec, msec)
    );

    time->tv_sec = sec;
    time->tv_nsec = msec * 1000000;
}

void sys_time_set_rel(sys_time_t * time, int64_t sec, int32_t msec)
{
    SYS_TEST(
        (msec > -1000) && (msec < 1000) &&
        ((sec >= 0) || (msec <= 0)) &&
        ((sec <= 0) || (msec >= 0)),
        EINVAL,
        E(),
        ASSERT("Invalid relative time (%ld, %d)", sec, msec)
    );

    time->tv_sec = sec;
    time->tv_nsec = msec * 1000000 + 1;
}

void sys_time_fix(sys_time_t ** time, sys_time_t * aux)
{
    if ((*time != NULL) && SYS_TIME_IS_REL(*time))
    {
        sys_time_copy(aux, *time);
        sys_time_abs(aux);
        *time = aux;
    }
}

void sys_time_fix_null(sys_time_t ** time, sys_time_t * aux)
{
    if (*time != NULL)
    {
        if (SYS_TIME_IS_REL(*time))
        {
            sys_time_copy(aux, *time);
            sys_time_abs(aux);

            *time = aux;
        }
    }
    else
    {
        aux->tv_sec = LONG_MAX;
        aux->tv_nsec = 0;

        *time = aux;
    }
}

int64_t sys_time_serial(sys_time_t * time)
{
    return time->tv_sec * 1000 + time->tv_nsec / 1000000;
}

void sys_time_from_serial_rel(sys_time_t * time, int64_t serial)
{
    sys_time_set_rel(time, serial / 1000, serial % 1000);
}

void sys_time_from_serial_abs(sys_time_t * time, int64_t serial)
{
    sys_time_set_abs(time, serial / 1000, serial % 1000);
}

/*
void sys_time_calibrate(void)
{
    uint64_t start_ticks, start_nsec, ticks, nsec, delay;
    int64_t finish;
    sys_time_t time;

    $E0(
        clock_gettime, (CLOCK_MONOTONIC, &time),
        E(),
        ASSERT("System time is unavailable")
    );
    start_ticks = sys_tsc();
    start_nsec = time.tv_sec * 1000000000 + time.tv_nsec;

    sleep(1);

    $E0(
        clock_gettime, (CLOCK_MONOTONIC, &time),
        E(),
        ASSERT("System time is unavailable")
    );
    ticks = sys_tsc() - start_ticks;
    nsec = time.tv_sec * 1000000000 + time.tv_nsec - start_nsec;

    delay = ticks * 1000000 / nsec;

    finish = delay * 2000;
    while (sys_tsc() - start_ticks < finish);

    $E0(
        clock_gettime, (CLOCK_MONOTONIC, &time),
        E(),
        ASSERT("System time is unavailable")
    );
    nsec = time.tv_sec * 1000000000 + time.tv_nsec - start_nsec;
    nsec = (nsec + 500000) / 1000000;
}
*/
