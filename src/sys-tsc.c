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

#define SYS_TSC_CALIBRATION_LOOPS 100000

#define SYS_TSC_FLAG_PRESENT    1
#define SYS_TSC_FLAG_CONSTANT   2
#define SYS_TSC_FLAG_RDTSCP     4
#define SYS_TSC_FLAG_NONSTOP    8

#define SYS_TSC_MODE_RDTSC      0
#define SYS_TSC_MODE_GETTIME    1

typedef struct _sys_tsc_client
{
    int32_t target;
    int64_t delta;
    int64_t * local;
    int64_t * remote;
} sys_tsc_client_t;

typedef struct _sys_tsc_data
{
    int64_t tsc;
} __align(CACHE_LINE_SIZE) sys_tsc_data_t;

static sys_tsc_config_t sys_tsc_config =
{
    .rdtsc_recheck_ms = 60000,
    .gettime_delay    = 10
};

int64_t (* sys_tsc_time)(void);

static int64_t sys_tsc_start;
static int64_t sys_tsc_start_ms;

static int64_t * sys_tsc_calibrated;
static __thread int64_t sys_tsc_offset;
static __thread int64_t sys_tsc_clocks_per_ms;
static __thread int64_t sys_tsc_rdtsc_recheck_clocks;
static __thread int64_t sys_tsc_next_check;

void __sys_tsc_sync(void)
{
    cpu_sync();
}

int64_t __sys_tsc_read(void)
{
    return cpu_rdtsc();
}

int64_t sys_tsc(void)
{
    return __sys_tsc_read() + sys_tsc_offset;
}

void sys_tsc_thread_initialize(void)
{
    sys_time_t now;
    int64_t clocks;

    sys_time_now(&now);
    __sys_tsc_sync();
    clocks = __sys_tsc_read();
    sys_tsc_clocks_per_ms = (clocks - sys_tsc_start) /
                            (sys_time_serial(&now) - sys_tsc_start_ms);

    sys_tsc_rdtsc_recheck_clocks = sys_tsc_config.rdtsc_recheck_ms *
                                   sys_tsc_clocks_per_ms;
    sys_tsc_next_check = clocks + sys_tsc_rdtsc_recheck_clocks;
}

static inline uint64_t __sys_tsc_wait_while(int64_t * value, int64_t expected)
{
    uint64_t new_value;

    while ((new_value = atomic_load(value, memory_order_acquire)) == expected);

    return new_value;
}

static inline void __sys_tsc_wait_until(int64_t * value, int64_t expected)
{
    while (atomic_load(value, memory_order_acquire) != expected);
}

static inline void __sys_tsc_set(int64_t * value, int64_t new_value)
{
    atomic_store(value, new_value, memory_order_release);
}

void sys_tsc_calibrate_client(int64_t * delta, int64_t * local, int64_t * remote)
{
    int64_t local_tsc1, local_tsc2, remote_tsc1, remote_tsc2;
    int64_t min, mean, count, n;
    int32_t i;

    min = INT64_MAX;
    mean = count = 0;
    for (i = 0; i < SYS_TSC_CALIBRATION_LOOPS; i++)
    {
        __sys_tsc_wait_until(remote, INT64_MAX);
        __sys_tsc_sync();
        __sys_tsc_set(local, INT64_MAX);
        remote_tsc1 = __sys_tsc_wait_while(remote, INT64_MAX);
        __sys_tsc_sync();
        local_tsc1 = __sys_tsc_read();
        __sys_tsc_set(local, local_tsc1);
        remote_tsc2 = __sys_tsc_wait_while(remote, remote_tsc1);
        __sys_tsc_sync();
        local_tsc2 = __sys_tsc_read();
        __sys_tsc_set(local, local_tsc2);

        remote_tsc2 -= remote_tsc1;
        local_tsc2 -= local_tsc1;

        n = remote_tsc2 * remote_tsc2 + local_tsc2 * local_tsc2;
        if (min > n)
        {
            min = n;
            mean = ((remote_tsc2 - local_tsc2) >> 1) +
                   2 * (remote_tsc1 - local_tsc1) + remote_tsc2;
            count = 1;
        }
        else if (min == n)
        {
            mean += ((remote_tsc2 - local_tsc2) >> 1) +
                    2 * (remote_tsc1 - local_tsc1) + remote_tsc2;
            count++;
        }
        __sys_tsc_wait_until(remote, 0);
        __sys_tsc_sync();
        __sys_tsc_set(local, 0);
    }

    *delta = mean / count;
}

void sys_tsc_calibrate_server(int64_t * delta, int64_t * local,
                              int64_t * remote)
{
    int64_t local_tsc1, local_tsc2, remote_tsc1, remote_tsc2;
    int64_t min, mean, count, n;
    int32_t i;

    min = INT64_MAX;
    mean = count = 0;
    for (i = 0; i < SYS_TSC_CALIBRATION_LOOPS; i++)
    {
        __sys_tsc_set(local, INT64_MAX);
        __sys_tsc_wait_until(remote, INT64_MAX);
        __sys_tsc_sync();
        local_tsc1 = __sys_tsc_read();
        __sys_tsc_set(local, local_tsc1);
        remote_tsc1 = __sys_tsc_wait_while(remote, INT64_MAX);
        __sys_tsc_sync();
        local_tsc2 = __sys_tsc_read();
        __sys_tsc_set(local, local_tsc2);
        remote_tsc2 = __sys_tsc_wait_while(remote, remote_tsc1);
        __sys_tsc_set(local, 0);

        remote_tsc2 -= remote_tsc1;
        local_tsc2 -= local_tsc1;

        n = remote_tsc2 * remote_tsc2 + local_tsc2 * local_tsc2;
        if (min > n)
        {
            min = n;
            mean = ((local_tsc2 - remote_tsc2) >> 1) +
                   2 * (local_tsc1 - remote_tsc1) + local_tsc2;
            count = 1;
        }
        else if (min == n)
        {
            mean += ((local_tsc2 - remote_tsc2) >> 1) +
                    2 * (local_tsc1 - remote_tsc1) + local_tsc2;
            count++;
        }

        __sys_tsc_sync();
        __sys_tsc_wait_until(remote, 0);
    }

    *delta = mean / count;
}

int32_t sys_tsc_calibrate_init(void * arg)
{
    sys_tsc_client_t * client;
    int32_t core;

    client = (sys_tsc_client_t *)arg;
    core = sys_thread_where();
    if (core != client->target)
    {
        logE("Thread not running on desired core (expected=%u, real=%u)",
                client->target, core);

        return ESRCH;
    }

    return 0;
}

void * sys_tsc_calibrate_worker(void * arg)
{
    sys_tsc_client_t * client;

    client = (sys_tsc_client_t *)arg;

    sys_tsc_calibrate_client(&client->delta, client->local, client->remote);

    return NULL;
}

int32_t sys_tsc_calibrate_core(int64_t * delta, int32_t target,
                               int64_t * local, int64_t * remote)
{
    sys_tsc_client_t client;
    pthread_t thread;
    int64_t offs;

    client.target = target;
    client.local = remote;
    client.remote = local;
    SYS_CALL(
        sys_thread_create, (&thread, 0 /*INT32_MAX*/, target, 0, 0,
                            sys_tsc_calibrate_init, sys_tsc_calibrate_worker,
                            &client),
        E(),
        RETERR()
    );

    sys_tsc_calibrate_server(&offs, local, remote);

    SYS_CALL(
        sys_thread_wait, (thread, NULL, NULL),
        E(),
        RETERR()
    );

    *delta = (offs + client.delta) >> 1;

    return 0;
}

int32_t sys_tsc_calibrate_from(int64_t * deltas, int32_t core)
{
    sys_tsc_data_t local;
    sys_tsc_data_t remote;
    int32_t current;

    SYS_CALL(
        sys_thread_migrate, (NULL, core),
        E(),
        RETERR()
    );
    current = sys_thread_where();
    if (current != core)
    {
        logE("Thread not running on desired core (expected=%u, real=%u)",
             core, current);

        return ESRCH;
    }

    sys_thread_for_each_online(current)
    {
        if (core != current)
        {
            local.tsc = 0;
            remote.tsc = 0;
            SYS_CALL(
                sys_tsc_calibrate_core, (&deltas[current], current,
                                         &local.tsc, &remote.tsc),
                E(),
                RETERR()
            );
        }
        else
        {
            deltas[current] = 0;
        }

        logD("Calibration of core %u: %ld", current, deltas[current]);
    }

    return 0;
}

const char * sys_tsc_parse_skip_spaces(const char * str)
{
    while (isspace(*str))
    {
        str++;
    }

    return str;
}

const char * sys_tsc_parse_to_space(const char * str)
{
    while (!isspace(*str))
    {
        str++;
    }

    return str;
}

int32_t sys_tsc_parse_check(const char * str, size_t len, const char * text)
{
    if (len != strlen(text))
    {
        return 0;
    }

    return (strncasecmp(str, text, len) == 0);
}

int32_t sys_tsc_cpu_flags_parse(const char * str)
{
    const char * word;
    size_t len;
    int32_t flags;

    flags = 0;

    str = sys_tsc_parse_skip_spaces(str);
    if (*str == ':')
    {
        word = sys_tsc_parse_skip_spaces(str + 1);
        while (*word != 0)
        {
            str = sys_tsc_parse_to_space(word);
            len = str - word;
            if (sys_tsc_parse_check(word, len, "tsc"))
            {
                flags |= SYS_TSC_FLAG_PRESENT;
            }
            else if (sys_tsc_parse_check(word, len, "constant_tsc"))
            {
                flags |= SYS_TSC_FLAG_CONSTANT;
            }
            else if (sys_tsc_parse_check(word, len, "rdtscp"))
            {
                flags |= SYS_TSC_FLAG_RDTSCP;
            }
            else if (sys_tsc_parse_check(word, len, "nonstop_tsc"))
            {
                flags |= SYS_TSC_FLAG_NONSTOP;
            }

            word = sys_tsc_parse_skip_spaces(str);
        }
    }

    return flags;
}

int32_t sys_tsc_detect(void)
{
    FILE * f;
    char * str;
    int32_t error = 0;
    int32_t state = 0;
    int32_t flags = 0;
    int32_t gflags = -1;
    int32_t count = 0;
    char buffer[1024];

    SYS_ERRNO_PTR(
        &f,
        fopen, ("/proc/cpuinfo", "r"),
        W(),
        LOG(I(), "Unable to get cpu information. Using gettime for timer"),
        RETVAL(SYS_TSC_MODE_GETTIME)
    );

    while (!feof(f))
    {
        SYS_PTR(
            &str,
            fgets, (buffer, sizeof(buffer), f),
            ENODATA,
            T(),
            BREAK()
        );

        if (strncasecmp(str, "processor", 9) == 0)
        {
            if (state > 0)
            {
                gflags &= flags;
            }

            flags = 0;
            state = 1;
            count++;
        }
        else if (state == 1)
        {
            if (strncasecmp(str, "flags", 5) == 0)
            {
                flags |= sys_tsc_cpu_flags_parse(str + 5);
            }
        }
    }
    gflags &= flags;

    SYS_ERRNO0(
        fclose, (f),
        E()
    );

    flags = SYS_TSC_FLAG_PRESENT | SYS_TSC_FLAG_CONSTANT;
    if (((gflags & flags) != flags) || (error != 0))
    {
        return SYS_TSC_MODE_GETTIME;
    }

    if ((count > 1) && ((gflags & SYS_TSC_FLAG_NONSTOP) == 0))
    {
        logW("CPU timers may lose synchronization if a CPU goes to sleep");
    }

    return SYS_TSC_MODE_RDTSC;
}

int32_t sys_tsc_calibrate(void)
{
    sys_time_t start, end;
    // volatile needed to avoid internal compiler error. Should be solved in
    // gcc 4.8.2
    volatile int64_t end_tsc;
    int64_t start_tsc, min, max;
    int32_t i, error, count;

    count = sys_thread_core_count();
    SYS_CALLOC(
        &sys_tsc_calibrated,
        count,
        sys_mt_int64_t,
        E(),
        RETERR()
    );
//    sys_thread_priority(NULL, INT32_MAX);

    sys_time_now(&start);
    __sys_tsc_sync();
    start_tsc = __sys_tsc_read();

    error = SYS_CALL(
                sys_tsc_calibrate_from, (sys_tsc_calibrated,
                                         sys_thread_core_next_online(-1)),
                E()
            );

    if (error == 0)
    {
        sys_time_now(&end);
        __sys_tsc_sync();
        end_tsc = __sys_tsc_read();

        sys_time_sub(&end, &start);
        if (sys_time_serial(&end) < 1000)
        {
            sys_time_set_rel(&end, 1, 0);
            sys_time_add(&end, &start);
            while ((clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME,
                                    &end, NULL) != 0) &&
                   (errno == EINTR));

            sys_time_now(&end);
            __sys_tsc_sync();
            end_tsc = __sys_tsc_read();

            sys_time_sub(&end, &start);
        }
        end_tsc -= start_tsc;

        min = INT64_MAX;
        max = INT64_MIN;
        for (i = 0; i < count; i++)
        {
            if (min > sys_tsc_calibrated[i])
            {
                min = sys_tsc_calibrated[i];
            }
            if (max < sys_tsc_calibrated[i])
            {
                max = sys_tsc_calibrated[i];
            }
        }
        max -= min;
        logD("Maximum distance: %ld", max);
        logD("Total cycles: %ld", end_tsc);
        logD("Total time: %lu.%09lu", end.tv_sec, end.tv_nsec);
        logD("Ratio: %lu cycles/ms", end_tsc / sys_time_serial(&end));
        if (max > 0)
        {
            logD("Error: 0.5 ms every %lu hours", end_tsc * 5 / (sys_time_serial(&end) * max * 36));
        }

        sys_tsc_start = start_tsc;
        sys_tsc_start_ms = sys_time_serial(&start);

        sys_tsc_thread_initialize();
    }

    SYS_CALL(
        sys_thread_migrate, (NULL, -1),
        E()
    );
//    sys_thread_priority(NULL, 0);

    SYS_FREE(sys_tsc_calibrated);

    return error;
}

int64_t sys_tsc_time_gettime(void)
{
    sys_time_t tmp;
    static __thread int32_t count = 0;
    static __thread int64_t last_time = 0;

    if (unlikely(count <= 0))
    {
        count = sys_tsc_config.gettime_delay;
        sys_time_now(&tmp);
        last_time = sys_time_serial(&tmp);
    }

    count--;

    return last_time;
}

int64_t sys_tsc_time_rdtsc(void)
{
    int64_t clocks;

    clocks = __sys_tsc_read();
    if (unlikely(clocks >= sys_tsc_next_check))
    {
        sys_tsc_thread_initialize();
        clocks = __sys_tsc_read();
    }

    return (clocks - sys_tsc_start) / sys_tsc_clocks_per_ms + sys_tsc_start_ms;
}

int32_t sys_tsc_initialize(sys_tsc_config_t * config)
{
    if (config != NULL)
    {
        sys_tsc_config = *config;
    }
    if (sys_tsc_config.rdtsc_recheck_ms < 10000)
    {
        sys_tsc_config.rdtsc_recheck_ms = 10000;
    }
    if (sys_tsc_config.gettime_delay < 1)
    {
        sys_tsc_config.gettime_delay = 1;
    }

    switch (sys_tsc_detect())
    {
        case SYS_TSC_MODE_RDTSC:
            if (sys_tsc_calibrate() == 0)
            {
                sys_tsc_time = sys_tsc_time_rdtsc;
                break;
            }
            /* Fall through */
        case SYS_TSC_MODE_GETTIME:
            sys_tsc_time = sys_tsc_time_gettime;
            break;
    }

    return 0;
}

