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

#include <sched.h>
#include <signal.h>

typedef struct _sys_thread_data
{
    int32_t           (* initializer)(void *);
    void *            (* func)(void *);
    void *               arg;
    int32_t              core;
    int32_t              error;
    uint64_t             signals;
    pthread_barrier_t    barrier;
} sys_thread_data_t;

static int32_t sys_thread_core_size;
static cpu_set_t * sys_thread_core_conf_set;
static cpu_set_t * sys_thread_core_onln_set;

static __thread sigset_t sys_thread_current_mask;

__thread err_t sys_thread_error = 0;

int32_t sys_thread_core_conf;
int32_t sys_thread_core_onln;

int32_t sys_thread_initialize(void)
{
    int32_t i, error;
    pthread_t self;

    SYS_ERRNO(
        &sys_thread_core_conf,
        sysconf, (_SC_NPROCESSORS_CONF),
        E(),
        RETERR()
    );

    sys_thread_core_size = CPU_ALLOC_SIZE(sys_thread_core_conf);

    logD("Number of cores: %d", sys_thread_core_conf);

    SYS_PTR(
        &sys_thread_core_conf_set,
        CPU_ALLOC, (sys_thread_core_conf),
        ENOMEM,
        E(),
        RETERR()
    );
    SYS_PTR(
        &sys_thread_core_onln_set,
        CPU_ALLOC, (sys_thread_core_conf),
        ENOMEM,
        E(),
        GOTO(failed, &error)
    );
    CPU_ZERO_S(sys_thread_core_size, sys_thread_core_onln_set);

    self = pthread_self();
    sys_thread_core_onln = 0;
    for (i = 0; i < sys_thread_core_conf; i++)
    {
        CPU_ZERO_S(sys_thread_core_size, sys_thread_core_conf_set);
        CPU_SET_S(i, sys_thread_core_size, sys_thread_core_conf_set);
        SYS_FUNC(
            &error,
            pthread_setaffinity_np, (self, sys_thread_core_size,
                                     sys_thread_core_conf_set),
            (error == 0) || (error == EINVAL),
            error,
            E(),
            LOG(W(), "Failed to migrate thread to core %u", i)
        );
        if (error == 0)
        {
            CPU_SET_S(i, sys_thread_core_size, sys_thread_core_onln_set);
            sys_thread_core_onln++;
        }
        else
        {
            logW("Core %u is offline or not available", i);
        }
    }
    CPU_ZERO_S(sys_thread_core_size, sys_thread_core_conf_set);
    for (i = 0; i < sys_thread_core_conf; i++)
    {
        CPU_SET_S(i, sys_thread_core_size, sys_thread_core_conf_set);
    }

    SYS_CALL(
        pthread_setaffinity_np, (self, sys_thread_core_size,
                                 sys_thread_core_conf_set),
        E(),
        GOTO(failed_onln, &error)
    );

    return 0;

failed_onln:
    CPU_FREE(sys_thread_core_onln_set);
failed:
    CPU_FREE(sys_thread_core_conf_set);

    return error;
}

void sys_thread_terminate(void)
{
    CPU_FREE(sys_thread_core_onln_set);
    CPU_FREE(sys_thread_core_conf_set);
}

int32_t sys_thread_core_count(void)
{
    return sys_thread_core_conf;
}

int32_t sys_thread_core_next_online(int32_t current)
{
    int32_t i;

    for (i = (current < 0) ? 0 : current + 1; i < sys_thread_core_conf; i++)
    {
        if (CPU_ISSET(i, sys_thread_core_onln_set))
        {
            return i;
        }
    }

    return -1;
}

static inline pthread_t __sys_thread_dereference(pthread_t * thread)
{
    return (thread != NULL) ? *thread : pthread_self();
}

void __sys_thread_priority_translate(int32_t priority,
                                     struct sched_param * sp,
                                     int32_t * policy)
{
    int32_t min, max;

    memset(sp, 0, sizeof(*sp));
    if (priority == 0)
    {
        *policy = SCHED_OTHER;
    }
    else
    {
        if (priority > 0)
        {
            *policy = SCHED_FIFO;
        }
        else
        {
            *policy = SCHED_RR;
            priority = -priority;
        }
        SYS_ERRNO(
            &min,
            sched_get_priority_min, (*policy),
            E(),
            ASSERT("Failed to get minimum priority for policy %d", *policy)
        );
        SYS_ERRNO(
            &max,
            sched_get_priority_max, (*policy),
            E(),
            ASSERT("Failed to get maximum priority for policy %d", *policy)
        );
        priority += min - 1;
        if (priority > max)
        {
            priority = max;
        }
        sp->sched_priority = priority;
    }
}

int32_t __sys_thread_set_translate(cpu_set_t ** set, int32_t core)
{
    if (core < 0)
    {
        *set = sys_thread_core_conf_set;

        return 0;
    }

    SYS_TEST(
        core < sys_thread_core_conf,
        EINVAL,
        Es(),
        ASSERT("Number of core out of range (%d/%d)",
               core, sys_thread_core_conf)
    );

    SYS_PTR(
        set,
        CPU_ALLOC, (sys_thread_core_size),
        ENOMEM,
        E(),
        RETERR()
    );
    CPU_ZERO_S(sys_thread_core_size, *set);
    CPU_SET_S(core, sys_thread_core_size, *set);

    return 0;
}

void __sys_thread_set_free(cpu_set_t * set)
{
    if (set != sys_thread_core_conf_set)
    {
        CPU_FREE(set);
    }
}

void sys_thread_priority(pthread_t * thread, int32_t priority)
{
    pthread_t thr;
    struct sched_param sp;
    int32_t policy;

    thr = __sys_thread_dereference(thread);
    __sys_thread_priority_translate(priority, &sp, &policy);

    SYS_CALL(
        pthread_setschedparam, (thr, policy, &sp),
        Es(),
        ASSERT("Unexpected error setting thread priority (%d, %d)", policy,
               sp.sched_priority)
    );
}

int32_t sys_thread_migrate(pthread_t * thread, int32_t target)
{
    pthread_t thr;
    cpu_set_t * set;

    SYS_CALL(
        __sys_thread_set_translate, (&set, target),
        E(),
        RETERR()
    );
    thr = __sys_thread_dereference(thread);
    SYS_CALL(
        pthread_setaffinity_np, (thr, sys_thread_core_size, set),
        E(),
        ASSERT("Unexpected error setting thread affinity (target=%d)", target)
    );
    __sys_thread_set_free(set);

    sys_tsc_thread_initialize();

    return 0;
}

int32_t sys_thread_where(void)
{
    int32_t core;

    SYS_ERRNO(
        &core,
        sched_getcpu, (),
        E(),
        ASSERT("Unexpected error getting current cpu")
    );

    return core;
}

bool __sys_thread_barrier_wait(pthread_barrier_t * barrier)
{
    int32_t error;

    SYS_FUNC(
        &error,
        pthread_barrier_wait, (barrier),
        (error == 0) || (error == PTHREAD_BARRIER_SERIAL_THREAD),
        error,
        Es(),
        ASSERT("Unexpected error waiting for a barrier")
    );

    return (error != 0);
}

void sys_thread_signals_to_mask(sigset_t * set, uint64_t signals)
{
    int32_t idx;

    SYS_ERRNO0(
        sigfillset, (set),
        Es(),
        ASSERT("Unexpected error configuring a signal mask")
    );

    while (signals != 0)
    {
        idx = sys_bits_first_one_index64(signals);
        SYS_ERRNO0(
            sigdelset, (set, idx + 1),
            E(),
            ASSERT("Unexpected error configuring a signal mask")
        );
        signals ^= 1ULL << idx;
    }
}

uint64_t sys_thread_mask_to_signals(sigset_t * set)
{
    uint64_t signals;
    int32_t idx, res;

    signals = 0;
    for (idx = 0; idx < 64; idx++)
    {
        SYS_ERRNO(
            &res,
            sigismember, (set, idx + 1),
            E(),
            ASSERT("Unexpected error processing a signal mask")
        );
        if (!res)
        {
            signals |= 1ULL << idx;
        }
    }

    return signals;
}

void sys_thread_signals_to_list(sigset_t * set, uint64_t signals)
{
    int32_t idx;

    SYS_ERRNO0(
        sigemptyset, (set),
        Es(),
        ASSERT("Unexpected error configuring a signal mask")
    );

    while (signals != 0)
    {
        idx = sys_bits_first_one_index64(signals);
        SYS_ERRNO0(
            sigaddset, (set, idx + 1),
            E(),
            ASSERT("Unexpected error configuring a signal mask")
        );
        signals ^= 1ULL << idx;
    }
}

uint64_t sys_thread_list_to_signals(sigset_t * set)
{
    uint64_t signals;
    int32_t idx, res;

    signals = 0;
    for (idx = 0; idx < 64; idx++)
    {
        SYS_ERRNO(
            &res,
            sigismember, (set, idx + 1),
            E(),
            ASSERT("Unexpected error processing a signal mask")
        );
        if (res)
        {
            signals |= 1ULL << idx;
        }
    }

    return signals;
}

void sys_thread_prepare_signals(uint64_t signals)
{
    if (signals == -1)
    {
        SYS_CALL(
            pthread_sigmask, (SIG_SETMASK, NULL, &sys_thread_current_mask),
            Es(),
            ASSERT("Unexpected error getting current signal mask")
        );

        return;
    }

    sys_thread_signals_to_mask(&sys_thread_current_mask, signals);

    SYS_CALL(
        pthread_sigmask, (SIG_SETMASK, &sys_thread_current_mask, NULL),
        Es(),
        ASSERT("Unexpected error setting current signal mask")
    );
}

void * __sys_thread_main(void * arg)
{
    sys_thread_data_t * args;
    void * (* func)(void *);
    void * func_arg;
    int32_t core;

    args = (sys_thread_data_t *)arg;
    sys_thread_prepare_signals(args->signals);

    if (args->core >= 0)
    {
        core = sys_thread_where();
        SYS_TEST(
            core == args->core,
            EINVAL,
            Es(),
            ASSERT("Thread not running on desired core (expected=%u, real=%u)",
                   args->core, core)
        );
    }
    sys_tsc_thread_initialize();

    if (args->initializer != NULL)
    {
        SYS_CALL(
            args->initializer, (args->arg),
            W(),
            GOTO(failed, &args->error)
        );
    }

    func = args->func;
    func_arg = args->arg;

    __sys_thread_barrier_wait(&args->barrier);

    return func(func_arg);

failed:
    __sys_thread_barrier_wait(&args->barrier);

    return NULL;
}

int32_t sys_thread_create(pthread_t * thread, int32_t priority, int32_t target,
                          uint32_t stack_size, uint64_t signals,
                          int32_t (* initializer)(void *),
                          void * (* func)(void *), void * arg)
{
    pthread_t thr;
    pthread_attr_t attr;
    cpu_set_t * set;
    sys_thread_data_t data;
    struct sched_param sp;
    int32_t policy, error;

    __sys_thread_priority_translate(priority, &sp, &policy);

    SYS_CALL(
        pthread_attr_init, (&attr),
        E(),
        RETERR()
    );
    SYS_CALL(
        pthread_attr_setdetachstate,
            (&attr, (thread == NULL) ? PTHREAD_CREATE_DETACHED
                                     : PTHREAD_CREATE_JOINABLE),
        E(),
        ASSERT("Unexpected error setting thread attributes (detachstate)")
    );
    SYS_CALL(
        pthread_attr_setinheritsched, (&attr, PTHREAD_EXPLICIT_SCHED),
        E(),
        ASSERT("Unexpected error setting thread attributes (inheritsched)")
    );
    SYS_CALL(
        pthread_attr_setschedpolicy, (&attr, policy),
        E(),
        ASSERT("Unexpected error setting thread attributes (schedpolicy)")
    );
    SYS_CALL(
        pthread_attr_setschedparam, (&attr, &sp),
        E(),
        ASSERT("Unexpected error setting thread attributes (schedparam)")
    );
    if (stack_size > 0)
    {
        SYS_CALL(
            pthread_attr_setstacksize, (&attr, stack_size),
            E(),
            ASSERT("Unexpected error setting thread attributes (stacksize)")
        );
    }
    SYS_CALL(
        pthread_barrier_init, (&data.barrier, NULL, 2),
        E(),
        GOTO(failed, &error)
    );
    SYS_CALL(
        __sys_thread_set_translate, (&set, target),
        E(),
        GOTO(failed_barrier, &error)
    );
    SYS_CALL(
        pthread_attr_setaffinity_np, (&attr, sys_thread_core_size, set),
        E(),
        ASSERT("Unexpected error setting thread attributes (affinity)")
    );
    __sys_thread_set_free(set);

    data.initializer = initializer;
    data.func = func;
    data.arg = arg;
    data.core = target;
    data.signals = signals;
    data.error = 0;
    SYS_CALL(
        pthread_create, (&thr, &attr, __sys_thread_main, &data),
        E(),
        GOTO(failed_barrier, &error)
    );
    __sys_thread_barrier_wait(&data.barrier);
    SYS_CALL(
        pthread_barrier_destroy, (&data.barrier),
        E(),
        ASSERT("Unexpecte error destroying a barrier")
    );

    if (unlikely(data.error != 0))
    {
        logE("Thread failed to initialize");
        error = data.error;

        if (thread != NULL)
        {
            sys_thread_wait(thr, NULL, NULL);
        }

        goto failed;
    }

    SYS_CALL(
        pthread_attr_destroy, (&attr),
        E(),
        ASSERT("Unexpected error destroying thread attributes")
    );

    if (thread != NULL)
    {
        *thread = thr;
    }

    logD("Thread created (detached=%s, priority=%d, target=%X, "
         "stack_size=%u, signals=%lX)", thread ? "no" : "yes",
         priority, target, stack_size, signals);

    return 0;

failed_barrier:
    SYS_CALL(
        pthread_barrier_destroy, (&data.barrier),
        E(),
        ASSERT("Unexpected error destroying a barrier")
    );
failed:
    SYS_CALL(
        pthread_attr_destroy, (&attr),
        E(),
        ASSERT("Unexpected error destroying thread attributes")
    );

    return error;
}

int32_t sys_thread_wait(pthread_t thread, sys_time_t * timeout, void ** result)
{
    sys_time_t aux;
    int32_t error;

    if (timeout == NULL)
    {
        SYS_CALL(
            pthread_join, (thread, result),
            E(),
            ASSERT("Unexpected error joining with a thread")
        );

        return 0;
    }

    sys_time_fix(&timeout, &aux);
    SYS_FUNC(
        &error,
        pthread_timedjoin_np, (thread, result, timeout),
        (error == 0) || (error == ETIMEDOUT),
        error,
        E(),
        ASSERT("Unexpected error joining with a thread")
    );

    return error;
}

/* No thread safe. It should only be called from one place at a time */
void sys_thread_signal_mask(uint64_t unblock, uint64_t block)
{
    int32_t i;

    for (i = 0; i < 64; i++)
    {
        if (((block >> i) & 1) != 0)
        {
            SYS_ERRNO0(
                sigaddset, (&sys_thread_current_mask, i + 1),
                Es(),
                ASSERT("Unexpected error configuring a signal mask")
            );
        }
        else if (((unblock >> i) & 1) != 0)
        {
            SYS_ERRNO0(
                sigdelset, (&sys_thread_current_mask, i + 1),
                Es(),
                ASSERT("Unexpected error configuring a signal mask")
            );
        }
    }

    SYS_ERRNO0(
        pthread_sigmask, (SIG_SETMASK, &sys_thread_current_mask, NULL),
        Es(),
        ASSERT("Unexpected error setting thread signal mask")
    );
}

void sys_thread_signal_cancel(uint64_t signals)
{
    sigset_t set;
    struct timespec timeout;
    int32_t num, error;

    timeout.tv_sec = 0;
    timeout.tv_nsec = 0;
    do
    {
        sys_thread_signals_to_list(&set, signals);

        error = SYS_ERRNO(
                    &num,
                    sigtimedwait, (&set, NULL, &timeout),
                    T()
                );
    } while ((error == 0) || (error == EINTR));

    SYS_TEST(
        error == EAGAIN,
        error,
        Es(),
        ASSERT("Unexpected error waiting for signals (%d)", error)
    );
}

err_t sys_thread_signal_wait(int32_t * signum, uint64_t signals,
                             sys_time_t * timeout)
{
    sigset_t set;
    sys_time_t abs, now, tmp;
    err_t error;
    int32_t num;

    sys_time_copy(&abs, timeout);
    sys_time_abs(&abs);
    do
    {
        sys_thread_signals_to_list(&set, signals);

        sys_time_copy(&tmp, &abs);
        sys_time_now(&now);
        sys_time_sub(&tmp, &now);
        if (unlikely((tmp.tv_sec < 0) || (tmp.tv_nsec < 0)))
        {
            return EAGAIN;
        }

        error = SYS_ERRNO(
                    &num,
                    sigtimedwait, (&set, NULL, &tmp),
                    T()
                );
    } while (error == EINTR);

    if (error == EAGAIN)
    {
        return error;
    }

    SYS_TEST(
        error == 0,
        error,
        Es(),
        ASSERT("Unexpected error waiting for signals (%d)", error)
    );

    if (signum != NULL)
    {
        *signum = num;
    }

    return 0;
}

void sys_thread_signal_kill(pthread_t * thread, int32_t signum)
{
    pthread_t thr;

    thr = __sys_thread_dereference(thread);
    SYS_CALL(
        pthread_kill, (thr, signum),
        Es(),
        ASSERT("Unexpected error sending a signal (%d)", signum)
    );
}
