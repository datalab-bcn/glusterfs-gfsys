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

#ifndef __SYS_IO_H__
#define __SYS_IO_H__

#define SYS_IO_ARG(_name) (void, _name, CALLS)
#define SYS_CBK_ARG(_name) (uintptr_t *, _name)

#define SYS_IO_PROXY(_name) SYS_GLUE(ioproxy$, _name)

#define SYS_IO_TYPE(_name) SYS_ARGS_TYPE(send_##_name)
#define SYS_IO_CBK_TYPE(_name) SYS_ARGS_TYPE(recv_##_name)

#define _SYS_IO_DECLARE(_name, _send, _recv) \
    SYS_ARGS_TYPE_DEFINE(send_##_name, _send); \
    SYS_ARGS_TYPE_DEFINE(recv_##_name, _recv); \
    void SYS_IO_PROXY(send_##_name)(uintptr_t * data); \
    SYS_CALLS_WRAPPER(send_##_name, _send, ((uintptr_t *, __sys_io_cbk), \
                                            (uintptr_t *, __sys_io_delay), \
                                            (sys_callback_f, __sys_io_free))) \
    { \
        uintptr_t * __sys_io_data; \
        logT("SYS-IO: submitting '" #_name "'"); \
        __sys_io_data = sys_io_add(SYS_IO_PROXY(send_##_name), 0, \
                                   SYS_MAX(SYS_CALLS_SIZE(send_##_name), \
                                           SYS_CALLS_SIZE(recv_##_name)), \
                                   __sys_io_cbk, __sys_io_delay, \
                                   __sys_io_free); \
        SYS_MARSHALL(__sys_io_data, send_##_name, _send); \
        sys_io_commit(__sys_io_data); \
        return __sys_io_data; \
    } \
    static inline void \
    SYS_GLUE(resume$, _name)(uintptr_t * __sys_resume_data, \
                             err_t __sys_resume_error \
                             SYS_MARG(SYS_ARGS_DECL(_recv))) \
    { \
        logT("SYS-IO: resuming '" #_name "'"); \
        sys_io_item_t * __sys_resume_item; \
        sys_async_queue_t * __sys_resume_queue; \
        uintptr_t __sys_resume_owner, * __sys_resume_args; \
        __sys_resume_item = (sys_io_item_t *)(__sys_resume_data - \
                                              SYS_IO_SIZE); \
        __sys_resume_args = sys_io_args(__sys_resume_data); \
        SYS_ARGS_FREE((SYS_ARGS_TYPE(send_##_name) *)__sys_resume_args, \
                      _send); \
        if (__sys_resume_item->cbk != NULL) \
        { \
            SYS_MARSHALL(__sys_resume_args, recv_##_name, _recv); \
            __sys_resume_owner = \
                sys_calls_owned_owner(__sys_resume_item->cbk); \
            __sys_resume_queue = sys_async_queue_get(__sys_resume_owner); \
            SYS_ASYNC_TO(__sys_resume_queue, sys_io_callback, \
                         (__sys_resume_item)); \
        } \
        else \
        { \
            logT("ARGS: free %p", __sys_resume_args); \
            _sys_io_release(__sys_resume_data); \
        } \
        sys_io_complete(__sys_resume_data, __sys_resume_error); \
    } \
    void SYS_GLUE(free$, _name)(uintptr_t * args); \
    void SYS_GLUE(send_##_name)(SYS_ARGS_DECL(_send))
#define SYS_IO_DECLARE(_name, _io, _send, _recv) \
    _SYS_IO_DECLARE(_name, (SYS_IO_ARG(_io) SYS_MARG(SYS_EXPAND(_send))), \
                    _recv)

#define _SYS_IO_DEFINE(_name, _send, _recv) \
    void SYS_GLUE(free$, _name)(uintptr_t * args) \
    { \
        logT("ARGS: free %p", args); \
        SYS_ARGS_FREE((SYS_ARGS_TYPE(recv_##_name) *)args, _recv); \
    } \
    void SYS_IO_PROXY(send_##_name)(uintptr_t * data) \
    { \
        __unused SYS_ARGS_TYPE(send_##_name) * args; \
        logT("SYS-IO: proxy to '" #_name "'"); \
        args = (SYS_ARGS_TYPE(send_##_name) *)data; \
        send_##_name(SYS_ARGS_LOAD(args, _send)); \
    } \
    void send_##_name(SYS_ARGS_DECL(_send))
#define SYS_IO_DEFINE(_name, _io, _send, _recv) \
    _SYS_IO_DEFINE(_name, (SYS_IO_ARG(_io) SYS_MARG(SYS_EXPAND(_send))), _recv)

#define SYS_IO_CBK(_name) SYS_GLUE(recv_, _name)

#define SYS_IO_CBK_DEFINE(_name, _type, _recv) \
    _type SYS_IO_CBK(_name)(SYS_ARGS_DECL(_recv))

#define SYS_IO_CREATE(_name, _io, _send, _recv) \
    SYS_IO_DECLARE(_name, _io, _send, _recv); \
    SYS_IO_DEFINE(_name, _io, _send, _recv)

#define SYS_IO_CBK_CREATE(_name, _type, _recv) \
    SYS_IO_CBK_DEFINE(_name, _type, _recv)

#define SYS_IO(_name, _args, _cbk, _delay...) \
    SYS_WRAP(send_##_name, (_cbk, \
                            SYS_SELECT(_delay, NULL, ## _delay), \
                            SYS_GLUE(free$, _name), \
                            NULL), \
             _args)

#define SYS_IO_SIZE SYS_CALLS_ADJUST_SIZE(sizeof(sys_io_item_t))

#define SYS_IO_RESUME(_name, _data, _error, _args...) \
    SYS_GLUE(resume$, _name)(_data, _error, ## _args)

#define SYS_IO_RELEASE(_name, _data) SYS_GLUE(free$, _name)(_data)

#define _SYS_CBK_DECLARE(_name, _args) SYS_OWNED_DECLARE(_name, _args)
#define SYS_CBK_DECLARE(_name, _data, _args) \
    _SYS_CBK_DECLARE(_name, (SYS_CBK_ARG(_data) SYS_MARG(SYS_EXPAND(_args))))

#define _SYS_CBK_DEFINE(_name, _args) SYS_OWNED_DEFINE(_name, _args)
#define SYS_CBK_DEFINE(_name, _data, _args) \
    _SYS_CBK_DEFINE(_name, (SYS_CBK_ARG(_data) SYS_MARG(SYS_EXPAND(_args))))

#define SYS_CBK_CREATE(_name, _data, _args) \
    SYS_CBK_DECLARE(_name, _data, _args); \
    SYS_CBK_DEFINE(_name, _data, _args)

#define SYS_CBK(_name, _args, _more...) \
    SYS_WRAP(_name, (&sys_async_owned_calls->tail, sys_async_self->head.id, \
                     0, NULL), \
             _args)

/*
int32_t callback(int a, int b)
{
    uintptr_t * data;

    data = (uintptr_t *)cookie;
    cookie = sys_io_data(data);
    SYS_RESUME(data, ...);

    return 0;
}
*/
typedef struct _sys_io_config
{
   int32_t            min_workers;
   int32_t            max_workers;
   int32_t            worker_timeout;
   int32_t            loops;
   sys_calls_config_t queue;
} sys_io_config_t;

typedef struct _sys_io_item
{
    struct _sys_io_item * next;
    uintptr_t *           cbk;
    uintptr_t *           delay;
    uintptr_t             data;
    sys_callback_f        free;
} sys_io_item_t;

typedef struct _sys_io
{
    sys_io_item_t *  first;
    sys_io_item_t ** plast;
} sys_io_t;

extern sys_io_t sys_io_queue;
extern uint64_t sys_io_sleeping;
extern pthread_t * sys_io_threads;

SYS_ASYNC_TO_DECLARE(sys_io_callback, ((sys_io_item_t *, item)));

static inline uintptr_t * sys_io_add(sys_callback_f callback, uint32_t extra,
                                     uint32_t size, uintptr_t * cbk,
                                     uintptr_t * delay, sys_callback_f free)
{
    sys_io_item_t * item;
    uintptr_t * data;

    data = sys_calls_owned_add(&sys_async_owned_calls->tail, callback,
                               sys_async_self->head.id, extra + SYS_IO_SIZE,
                               size, 0);

//    logD("sys_io_add: %p, %p", cbk, delay);
    item = (sys_io_item_t *)data;
    item->cbk = cbk;
    item->delay = delay;
    item->free = free;

    return data + SYS_IO_SIZE;
}

static inline uintptr_t * sys_io_args(uintptr_t * data)
{
    return sys_calls_owned_args(data - SYS_IO_SIZE);
}

static inline void sys_io_wake(void)
{
    uint64_t bit;

    bit = atomic_bit_next(&sys_io_sleeping, memory_order_seq_cst,
                          memory_order_seq_cst);
    if (bit != 0)
    {
        bit = sys_bits_first_one_index64(bit);
        sys_thread_signal_kill(&sys_io_threads[bit], SIGCONT);
    }
}

static inline void sys_io_commit(uintptr_t * data)
{
    sys_io_item_t * io, **plast;

    io = (sys_io_item_t *)(data - SYS_IO_SIZE);
    io->next = NULL;
    plast = atomic_xchg(&sys_io_queue.plast, &io->next, memory_order_seq_cst);
    *plast = io;

    sys_io_wake();
}

static inline void sys_io_complete(uintptr_t * data, err_t error)
{
    sys_io_item_t * item;

    item = (sys_io_item_t *)(data - SYS_IO_SIZE);
    if (item->delay != NULL)
    {
//        logD("sys_io_complete: %p, %u", item->delay, error);
        sys_delay_execute(item->delay, error);
    }
}

SYS_ASYNC_TO_DECLARE(__sys_io_release, ((sys_io_item_t *, item)));

static inline void _sys_io_release(uintptr_t * data)
{
    sys_io_item_t * item;
    uintptr_t owner;

    data -= SYS_IO_SIZE;
    item = (sys_io_item_t *)data;
    owner = sys_calls_owned_owner(data);
    if (sys_async_self->head.id == owner)
    {
        sys_calls_owned_release(&sys_async_owned_calls->head, data);
    }
    else
    {
        SYS_ASYNC_TO(sys_async_queue_get(owner), __sys_io_release, (item));
    }
}

static inline void sys_io_release(uintptr_t * data)
{
    sys_io_item_t * item;

    item = (sys_io_item_t *)(data - SYS_IO_SIZE);
    item->free(data);
    _sys_io_release(data);
}

static inline uintptr_t * sys_io_data(uintptr_t * data)
{
    sys_io_item_t * item;

    item = (sys_io_item_t *)(data - SYS_IO_SIZE);
    return &item->data;
}

err_t sys_io_initialize(sys_io_config_t * config);
void sys_io_terminate(void);

#endif /* __SYS_IO_H__ */
