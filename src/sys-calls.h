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

#ifndef __SYS_CALLS_H__
#define __SYS_CALLS_H__

typedef struct _sys_calls_config
{
    int32_t size;
    int32_t backlog_entries;
    int32_t backlog_preallocate;
} sys_calls_config_t;

typedef void (* sys_callback_f)(uintptr_t * data);

typedef struct _sys_calls_node
{
    sys_callback_f                callback;
    union
    {
        struct
        {
            uint8_t               size;
            uint8_t               extra;
            uint8_t               refs;
            uint8_t               flags;
            err_t                 error;
        };
        struct _sys_calls_node *  next;
    };
} sys_calls_node_t;

typedef struct _sys_calls_owned
{
    uintptr_t owner;
} sys_calls_owned_t;

typedef struct _sys_calls_end
{
    uint32_t            position;
    uint32_t            size;
    sys_calls_node_t *  buffer;
    uintptr_t *         free;

    uint32_t            free_position;
    uint32_t            free_mask;
    sys_calls_node_t ** free_buffers;

    uint64_t            allocations;
    uint64_t            processed;
    uint64_t            callbacks;
} __align(CACHE_LINE_SIZE) sys_calls_end_t;

typedef struct _sys_calls
{
    sys_calls_end_t head;
    sys_calls_end_t tail;
} sys_calls_t;

#define SYS_ARG_CALLS_TYPE(_arg, _more...) uintptr_t *
#define SYS_ARG_CALLS_NAME(_arg, _more...) SYS_ARG_NAME(_arg, ## _more)
#define SYS_ARG_CALLS_FIELD(_arg, _more...) \
    uintptr_t * SYS_ARG_NAME(_arg, ## _more)
#define SYS_ARG_CALLS_DECL(_arg, _more...) \
    uintptr_t * SYS_ARG_NAME(_arg, ## _more)
#define SYS_ARG_CALLS_SAVE(_arg, _dst, _more...) \
    (_dst)->SYS_ARG_NAME(_arg, ## _more) = NULL
#define SYS_ARG_CALLS_LOAD(_arg, _src, _more...) \
    (uintptr_t *)container_of(&(_src)->SYS_ARG_NAME(_arg, ## _more), \
                              typeof(*(_src)), \
                              SYS_ARG_NAME(_arg, ## _more))
#define SYS_ARG_CALLS_FREE(_arg, _src, _more...) \
    SYS_ARG_FREE(_arg, _src, ## _more)

#define SYS_CALLS_ADJUST_SIZE(_size) \
    (((_size) + sizeof(uintptr_t) - 1) / sizeof(uintptr_t))

#define SYS_CALLS_NODE_SIZE SYS_CALLS_ADJUST_SIZE(sizeof(sys_calls_node_t))
#define SYS_CALLS_OWNED_SIZE SYS_CALLS_ADJUST_SIZE(sizeof(sys_calls_owned_t))

#define SYS_PROXY(_name) SYS_GLUE(proxy$, _name)

#define SYS_MARSHALL(_data, _name, _args) \
    SYS_ARGS_SAVE((SYS_ARGS_TYPE(_name) *)(_data), _args)

#define SYS_CALLS_DECLARE(_name, _args) \
    SYS_ARGS_TYPE_DEFINE(_name, _args); \
    void SYS_PROXY(_name)(uintptr_t * data); \
    void _name(SYS_ARGS_DECL(_args))

#define SYS_CALLS_DEFINE(_name, _args) \
    void SYS_PROXY(_name)(uintptr_t * data) \
    { \
        logD("SYS-CALLS: proxy to '" #_name "'"); \
        __unused SYS_ARGS_TYPE(_name) * args = (SYS_ARGS_TYPE(_name) *)data; \
        _name(SYS_ARGS_LOAD(args, _args)); \
        SYS_ARGS_FREE(args, _args); \
    } \
    void _name(SYS_ARGS_DECL(_args))

#define SYS_CALLS_SIZE(_name) \
    SYS_CALLS_ADJUST_SIZE(sizeof(SYS_ARGS_TYPE(_name)))

#define SYS_CALLS_WRAPPER(_name, _args, _more...) \
    static inline uintptr_t * \
    SYS_GLUE(wrapper$, _name)(SYS_ARGS_DECL(SYS_ARGS_COMBINE(_more, _args)))

#define SYS_WRAP(_name, _extra, _args) \
    SYS_GLUE(wrapper$, _name) SYS_ARGS_COMBINE(_extra, _args)

#define SYS_OWNED_DECLARE(_name, _args) \
    SYS_CALLS_DECLARE(_name, _args); \
    SYS_CALLS_WRAPPER(_name, _args, ((sys_calls_end_t *, __sys_owned_end), \
                                     (uintptr_t, __sys_owned_owner), \
                                     (uint32_t, __sys_owned_refs))) \
    { \
        logD("SYS-CALLS: submitting '" #_name "'"); \
        uintptr_t * __sys_owned_data; \
        __sys_owned_data = sys_calls_owned_add(__sys_owned_end, \
                                               SYS_PROXY(_name), \
                                               __sys_owned_owner, 0, \
                                               SYS_CALLS_SIZE(_name), \
                                               __sys_owned_refs); \
        SYS_MARSHALL(__sys_owned_data, _name, _args); \
        return __sys_owned_data; \
    }

#define SYS_OWNED_DEFINE(_name, _args) SYS_CALLS_DEFINE(_name, _args)

#define SYS_OWNED_CREATE(_name, _args) \
    SYS_OWNED_DECLARE(_name, _args); \
    SYS_OWNED_DEFINE(_name, _args)

void sys_calls_config_prepare(sys_calls_config_t * config);

err_t sys_calls_initialize(sys_calls_config_t * config);
void sys_calls_terminate(void);

err_t sys_calls_buffer_initialize(sys_calls_config_t * config,
                                  sys_calls_end_t * head,
                                  sys_calls_end_t * tail);
void sys_calls_buffer_terminate(sys_calls_end_t * head,
                                sys_calls_end_t * tail);

err_t sys_calls_create(sys_calls_t ** calls, sys_calls_config_t * config);
void sys_calls_destroy(sys_calls_t * calls);

uintptr_t * sys_calls_prepare(sys_calls_end_t * tail, uint32_t extra,
                              uint32_t size);

static inline void sys_calls_commit(uintptr_t * data, sys_callback_f callback)
{
    sys_calls_node_t * node;

    SYS_ASSERT(callback != NULL, "Callback is NULL");

    node = (sys_calls_node_t *)(data - SYS_CALLS_NODE_SIZE);

    smp_wmb();

    atomic_store(&node->callback, callback, memory_order_release);
}

uintptr_t * sys_calls_add(sys_calls_end_t * tail, sys_callback_f callback,
                          uint32_t extra, uint32_t size, uint32_t refs);
void sys_calls_release(sys_calls_end_t * head, uintptr_t * data);

uintptr_t * sys_calls_get(sys_calls_end_t * head);

static inline void sys_calls_execute(uintptr_t * data)
{
    sys_calls_node_t * node;

    node = (sys_calls_node_t *)(data - SYS_CALLS_NODE_SIZE);
    node->callback((uintptr_t *)node + node->extra);
}

bool sys_calls_process(sys_calls_end_t * head);

static inline bool sys_calls_empty(sys_calls_end_t * head)
{
    sys_calls_node_t * node;

    node = (sys_calls_node_t *)(head->buffer + head->position);
    return (node->callback == NULL);
}

static inline uint8_t * sys_calls_flags(uintptr_t * data)
{
    sys_calls_node_t * node;

    node = (sys_calls_node_t *)(data - SYS_CALLS_NODE_SIZE);
    return &node->flags;
}

static inline uintptr_t * sys_calls_owned_prepare(sys_calls_end_t * tail,
                                                  uintptr_t owner,
                                                  uint32_t extra,
                                                  uint32_t size)
{
    sys_calls_owned_t * owned;
    uintptr_t * data;

    data = sys_calls_prepare(tail, extra + SYS_CALLS_OWNED_SIZE, size);
    owned = (sys_calls_owned_t *)data;
    owned->owner = owner;

    return data + SYS_CALLS_OWNED_SIZE;
}

static inline void sys_calls_owned_commit(uintptr_t * data,
                                          sys_callback_f callback)
{
    sys_calls_commit(data - SYS_CALLS_OWNED_SIZE, callback);
}

static inline uintptr_t * sys_calls_owned_add(sys_calls_end_t * tail,
                                              sys_callback_f callback,
                                              uintptr_t owner,
                                              uint32_t extra, uint32_t size,
                                              uint32_t refs)
{
    sys_calls_owned_t * owned;
    uintptr_t * data;

    data = sys_calls_add(tail, callback, extra + SYS_CALLS_OWNED_SIZE, size,
                         refs);
    owned = (sys_calls_owned_t *)data;
    owned->owner = owner;

    return data + SYS_CALLS_OWNED_SIZE;
}

static inline void sys_calls_owned_release(sys_calls_end_t * head,
                                           uintptr_t * data)
{
    sys_calls_release(head, data - SYS_CALLS_OWNED_SIZE);
}

static inline uintptr_t * sys_calls_owned_get(sys_calls_end_t * head)
{
    return sys_calls_get(head) + SYS_CALLS_OWNED_SIZE;
}

static inline void sys_calls_owned_execute(uintptr_t * data)
{
    sys_calls_execute(data - SYS_CALLS_OWNED_SIZE);
}

static inline bool sys_calls_owned_process(sys_calls_end_t * head)
{
    return sys_calls_process(head);
}

static inline uint8_t * sys_calls_owned_flags(uintptr_t * data)
{
    return sys_calls_flags(data - SYS_CALLS_OWNED_SIZE);
}

static inline uintptr_t sys_calls_owned_owner(uintptr_t * data)
{
    sys_calls_owned_t * owned;

    owned = (sys_calls_owned_t *)(data - SYS_CALLS_OWNED_SIZE);
    return owned->owner;
}

static inline uintptr_t * sys_calls_args(uintptr_t * data)
{
    sys_calls_node_t * node;

    node = (sys_calls_node_t *)(data - SYS_CALLS_NODE_SIZE);
    return (uintptr_t *)node + node->extra;
}

static inline uintptr_t * sys_calls_owned_args(uintptr_t * data)
{
    return sys_calls_args(data - SYS_CALLS_OWNED_SIZE);
}

/*
static inline uintptr_t * sys_calls_cancellable_prepare(
                                sys_callback_f callback, uintptr_t extra,
                                uintptr_t size, int32_t refs,
                                sys_calls_end_t * tail)
{
    sys_calls_cancellable_t * item;
    uintptr_t * data;

    data = sys_calls_add(tail, callback, extra + SYS_CALLS_CANCELLABLE_SIZE,
                         size);

    item = (sys_calls_cancellable_t *)data;
    item->refs = refs;
    item->error = 0;

    return data + SYS_CALLS_CANCELLABLE_SIZE;
}


static inline void sys_calls_cancellable_commit(sys_callback_f callback,
                                                uintptr_t * data, int32_t refs,
                                                sys_calls_end_t * tail)
{
}

void sys_calls_cancellable_release(sys_calls_end_t * head, uintptr_t * data);

err_t sys_calls_cancellable_process(sys_calls_end_t * head, uintptr_t * data,
                                    err_t error);

static inline err_t sys_calls_cancellable_cancel(sys_calls_end_t * head,
                                                 uintptr_t * data)
{
    return sys_calls_cancellable_process(head, data, ECANCELED);
}

static inline bool sys_calls_cancellable_empty(sys_calls_end_t * head,
                                               sys_calls_end_t * tail)
{
    return (head->callbacks[0] == tail->callbacks[0] + tail->callbacks[1]);
}

static inline void sys_call(uintptr_t * data, err_t error)
{
    sys_calls_node_t * node;

    sys_thread_set_error(error);

    node = (sys_calls_node_t *)(data - SYS_CALLS_NODE_SIZE);
    node->callback(data + node->extra + 1, node->size);
}
*/
#endif /* __SYS_CALLS_H__ */
