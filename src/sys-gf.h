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

#ifndef __SYS_GF_H__
#define __SYS_GF_H__

#define SYS_GF_HANDLERS_DECLARE(_fop) \
    void sys_gf_handler_##_fop##_wind(call_frame_t * frame, void * cookie, \
                                      xlator_t * xl, uintptr_t * cbk, \
                                      uintptr_t * to, uintptr_t * data); \
    void sys_gf_handler_##_fop##_wind_tail(call_frame_t * frame, \
                                           xlator_t * xl, uintptr_t * cbk, \
                                           uintptr_t * to, uintptr_t * data); \
    void sys_gf_handler_call_##_fop##_wind(call_frame_t * frame, \
                                           void * cookie, xlator_t * xl, \
                                           uintptr_t * cbk, uintptr_t * to, \
                                           uintptr_t * data); \
    void sys_gf_handler_call_##_fop##_wind_tail(call_frame_t * frame, \
                                                xlator_t * xl, \
                                                uintptr_t * cbk, \
                                                uintptr_t * to, \
                                                uintptr_t * data); \
    void sys_gf_handler_##_fop##_unwind(call_frame_t * frame, int32_t op_ret, \
                                        int32_t op_errno, uintptr_t * cbk, \
                                        uintptr_t * to, uintptr_t * data); \
    void sys_gf_handler_##_fop##_unwind_error(call_frame_t * frame, \
                                              int32_t op_errno, \
                                              dict_t * xdata, \
                                              uintptr_t * cbk, \
                                              uintptr_t * to, \
                                              uintptr_t * data); \
    void sys_gf_handler_call_##_fop##_unwind(call_frame_t * frame, \
                                             int32_t op_ret, \
                                             int32_t op_errno, \
                                             uintptr_t * cbk, uintptr_t * to, \
                                             uintptr_t * data); \
    void sys_gf_handler_call_##_fop##_unwind_error(call_frame_t * frame, \
                                                   int32_t op_errno, \
                                                   dict_t * xdata, \
                                                   uintptr_t * cbk, \
                                                   uintptr_t * to, \
                                                   uintptr_t * data); \
    void sys_gf_##_fop##_wind(call_frame_t * frame, fop_##_fop##_cbk_t rfn, \
                              void * cookie, xlator_t * xl, \
                              SYS_ARGS_DECL((SYS_GF_ARGS_##_fop))); \
    void sys_gf_##_fop##_wind_tail(call_frame_t * frame, xlator_t * xl, \
                                   SYS_ARGS_DECL((SYS_GF_ARGS_##_fop))); \
    void sys_gf_##_fop##_unwind(call_frame_t * frame, int32_t op_ret, \
                                int32_t op_errno, \
                                SYS_ARGS_DECL((SYS_GF_ARGS_##_fop##_cbk))); \
    void sys_gf_##_fop##_unwind_error(call_frame_t * frame, int32_t op_errno, \
                                      dict_t * xdata); \
    extern sys_gf_handlers_t sys_gf_handlers_fop_##_fop; \
    extern sys_gf_handlers_t sys_gf_handlers_fop_call_##_fop; \
    extern sys_gf_handlers_t sys_gf_handlers_cbk_##_fop; \
    extern sys_gf_handlers_t sys_gf_handlers_cbk_call_##_fop

#define SYS_GF_SIZE SYS_CALLS_ADJUST_SIZE(sizeof(sys_gf_args_t))

#define SYS_GF_FOP_SIZE(_fop) \
    SYS_CALLS_ADJUST_SIZE(sizeof(SYS_GF_FOP_TYPE(_fop)))

#define SYS_GF_CBK_SIZE(_fop) \
    SYS_CALLS_ADJUST_SIZE(sizeof(SYS_GF_CBK_TYPE(_fop)))

#define SYS_GF_FOP_CALL_SIZE(_fop) \
    SYS_CALLS_ADJUST_SIZE(sizeof(SYS_GF_FOP_CALL_TYPE(_fop)))

#define SYS_GF_CBK_CALL_SIZE(_fop) \
    SYS_CALLS_ADJUST_SIZE(sizeof(SYS_GF_CBK_CALL_TYPE(_fop)))

#define SYS_GF_FOP(_fop, _extra...) \
    ({ \
        uintptr_t * __sys_gf_data; \
        logT("SYS-GF: fop '" #_fop "'"); \
        sys_gf_args_t * __sys_gf_args; \
        __sys_gf_data = sys_calls_owned_add(&sys_async_owned_calls->tail, \
                                            sys_gf_fop_##_fop##_free, \
                                            sys_async_self->head.id, \
                                            _extra + SYS_GF_SIZE, \
                                            SYS_GF_FOP_SIZE(_fop), 0); \
        SYS_MARSHALL(__sys_gf_data + (_extra + 0) + SYS_GF_SIZE, \
                     sys_gf_fop_##_fop, (SYS_GF_ARGS_##_fop)); \
        __sys_gf_args = (sys_gf_args_t *)__sys_gf_data; \
        __sys_gf_args->ops = &sys_gf_handlers_fop_##_fop; \
        __sys_gf_data + SYS_GF_SIZE; \
    })

#define SYS_GF_CBK(_fop, _extra...) \
    ({ \
        uintptr_t * __sys_gf_data; \
        logT("SYS-GF: cbk '" #_fop "'"); \
        sys_gf_args_t * __sys_gf_args; \
        __sys_gf_data = sys_calls_owned_add(&sys_async_owned_calls->tail, \
                                            sys_gf_cbk_##_fop##_free, \
                                            sys_async_self->head.id, \
                                            _extra + SYS_GF_SIZE, \
                                            SYS_GF_CBK_SIZE(_fop), 0); \
        SYS_MARSHALL(__sys_gf_data + (_extra + 0) + SYS_GF_SIZE, \
                     sys_gf_cbk_##_fop, (SYS_GF_ARGS_##_fop##_cbk)); \
        __sys_gf_args = (sys_gf_args_t *)__sys_gf_data; \
        __sys_gf_args->ops = &sys_gf_handlers_cbk_##_fop; \
        __sys_gf_data + SYS_GF_SIZE; \
    })

#define SYS_GF_FOP_CALL(_fop, _extra...) \
    ({ \
        uintptr_t * __sys_gf_data; \
        logT("SYS-GF: fop call '" #_fop "'"); \
        sys_gf_args_t * __sys_gf_args; \
        __sys_gf_data = sys_calls_owned_add(&sys_async_owned_calls->tail, \
                                            sys_gf_fop_call_##_fop##_free, \
                                            sys_async_self->head.id, \
                                            _extra + SYS_GF_SIZE, \
                                            SYS_GF_FOP_CALL_SIZE(_fop), 0); \
        SYS_MARSHALL(__sys_gf_data + (_extra + 0) + SYS_GF_SIZE, \
                     sys_gf_fop_call_##_fop, (SYS_GF_ARGS_FOP, \
                                              SYS_GF_ARGS_##_fop)); \
        __sys_gf_args = (sys_gf_args_t *)__sys_gf_data; \
        __sys_gf_args->ops = &sys_gf_handlers_fop_call_##_fop; \
        __sys_gf_data + SYS_GF_SIZE; \
    })

#define SYS_GF_CBK_CALL(_fop, _extra...) \
    ({ \
        uintptr_t * __sys_gf_data; \
        logT("SYS-GF: cbk call '" #_fop "'"); \
        sys_gf_args_t * __sys_gf_args; \
        __sys_gf_data = sys_calls_owned_add(&sys_async_owned_calls->tail, \
                                            sys_gf_cbk_call_##_fop##_free, \
                                            sys_async_self->head.id, \
                                            _extra + SYS_GF_SIZE, \
                                            SYS_GF_CBK_CALL_SIZE(_fop), 0); \
        SYS_MARSHALL(__sys_gf_data + (_extra + 0) + SYS_GF_SIZE, \
                     sys_gf_cbk_call_##_fop, (SYS_GF_ARGS_CBK, \
                                              SYS_GF_ARGS_##_fop##_cbk)); \
        __sys_gf_args = (sys_gf_args_t *)__sys_gf_data; \
        __sys_gf_args->ops = &sys_gf_handlers_cbk_call_##_fop; \
        __sys_gf_data + SYS_GF_SIZE; \
    })

#define SYS_GF_FOP_TYPE(_fop) SYS_ARGS_TYPE(sys_gf_fop_##_fop)
#define SYS_GF_CBK_TYPE(_fop) SYS_ARGS_TYPE(sys_gf_cbk_##_fop)

#define SYS_GF_FOP_CALL_TYPE(_fop) SYS_ARGS_TYPE(sys_gf_fop_call_##_fop)
#define SYS_GF_CBK_CALL_TYPE(_fop) SYS_ARGS_TYPE(sys_gf_cbk_call_##_fop)

#define SYS_GF_FOP_TYPE_DEFINE(_fop) \
    SYS_ARGS_TYPE_DEFINE(sys_gf_fop_##_fop, (SYS_GF_ARGS_##_fop))

#define SYS_GF_CBK_TYPE_DEFINE(_fop) \
    SYS_ARGS_TYPE_DEFINE(sys_gf_cbk_##_fop, (SYS_GF_ARGS_##_fop##_cbk))

#define SYS_GF_TYPE_DEFINE(_fop) \
    SYS_GF_FOP_TYPE_DEFINE(_fop); \
    SYS_GF_CBK_TYPE_DEFINE(_fop)

#define SYS_GF_FOP_CALL_TYPE_DEFINE(_fop) \
    SYS_ARGS_TYPE_DEFINE(sys_gf_fop_call_##_fop, \
                         (SYS_GF_ARGS_FOP, SYS_GF_ARGS_##_fop))

#define SYS_GF_CBK_CALL_TYPE_DEFINE(_fop) \
    SYS_ARGS_TYPE_DEFINE(sys_gf_cbk_call_##_fop, \
                         (SYS_GF_ARGS_CBK, SYS_GF_ARGS_##_fop##_cbk))

#define SYS_GF_CALL_TYPE_DEFINE(_fop) \
    SYS_GF_FOP_CALL_TYPE_DEFINE(_fop); \
    SYS_GF_CBK_CALL_TYPE_DEFINE(_fop)

#define SYS_GF_FREE_DECLARE(_fop) \
    void sys_gf_fop_##_fop##_free(uintptr_t * data); \
    void sys_gf_cbk_##_fop##_free(uintptr_t * data); \
    void sys_gf_fop_call_##_fop##_free(uintptr_t * data); \
    void sys_gf_cbk_call_##_fop##_free(uintptr_t * data)

#define SYS_GF_DECLARE(_fop) \
    SYS_GF_TYPE_DEFINE(_fop); \
    SYS_GF_CALL_TYPE_DEFINE(_fop); \
    SYS_GF_FREE_DECLARE(_fop); \
    SYS_GF_HANDLERS_DECLARE(_fop)

#define SYS_GF_IO_WIND_ARGS \
    (call_frame_t *, frame),  \
    (void *,         cookie), \
    (xlator_t *,     this)

#define SYS_GF_IO_WIND_TAIL_ARGS \
    (call_frame_t *, frame),  \
    (xlator_t *,     this)

#define SYS_GF_IO_UNWIND_ARGS \
    (call_frame_t *, frame),  \
    (int32_t,        op_ret), \
    (int32_t,        op_errno)

#define SYS_GF_IO_UNWIND_ERROR_ARGS \
    (call_frame_t *, frame),  \
    (int32_t,        op_errno)

#define SYS_GF_IO_DECLARE(_fop) \
    SYS_IO_DECLARE(sys_gf_##_fop##_wind, __sys_gf_io, \
                   (SYS_GF_IO_WIND_ARGS, SYS_GF_ARGS_##_fop), \
                   (SYS_GF_ARGS_CBK, SYS_GF_ARGS_##_fop##_cbk)); \
    SYS_IO_DECLARE(sys_gf_##_fop##_wind_tail, __sys_gf_io, \
                   (SYS_GF_IO_WIND_TAIL_ARGS, SYS_GF_ARGS_##_fop), \
                   (SYS_GF_ARGS_FOP)); \
    SYS_IO_DECLARE(sys_gf_##_fop##_unwind, __sys_gf_io, \
                   (SYS_GF_IO_UNWIND_ARGS, SYS_GF_ARGS_##_fop##_cbk), \
                   (SYS_GF_IO_UNWIND_ARGS)); \
    SYS_IO_DECLARE(sys_gf_##_fop##_unwind_error, __sys_gf_io, \
                   (SYS_GF_IO_UNWIND_ERROR_ARGS, (dict_t *, xdata)), \
                   (SYS_GF_IO_UNWIND_ERROR_ARGS))

#define SYS_GF_FOP_LIST \
    access, create, entrylk, fentrylk, flush, fsync, fsyncdir, getxattr, \
    fgetxattr, inodelk, finodelk, link, lk, lookup, mkdir, mknod, open, \
    opendir, rchecksum, readdir, readdirp, readlink, readv, removexattr, \
    fremovexattr, rename, rmdir, setattr, fsetattr, setxattr, fsetxattr, \
    stat, fstat, statfs, symlink, truncate, ftruncate, unlink, writev, \
    xattrop, fxattrop

#define SYS_GF_CBK_LIST \
    forget, invalidate, release, releasedir

#define SYS_GF_FOP_APPLY(_sep, _macro, _args...) \
    _macro(access, ## _args) SEP_##_sep \
    _macro(create, ## _args) SEP_##_sep \
    _macro(entrylk, ## _args) SEP_##_sep \
    _macro(fentrylk, ## _args) SEP_##_sep \
    _macro(flush, ## _args) SEP_##_sep \
    _macro(fsync, ## _args) SEP_##_sep \
    _macro(fsyncdir, ## _args) SEP_##_sep \
    _macro(getxattr, ## _args) SEP_##_sep \
    _macro(fgetxattr, ## _args) SEP_##_sep \
    _macro(inodelk, ## _args) SEP_##_sep \
    _macro(finodelk, ## _args) SEP_##_sep \
    _macro(link, ## _args) SEP_##_sep \
    _macro(lk, ## _args) SEP_##_sep \
    _macro(lookup, ## _args) SEP_##_sep \
    _macro(mkdir, ## _args) SEP_##_sep \
    _macro(mknod, ## _args) SEP_##_sep \
    _macro(open, ## _args) SEP_##_sep \
    _macro(opendir, ## _args) SEP_##_sep \
    _macro(rchecksum, ## _args) SEP_##_sep \
    _macro(readdir, ## _args) SEP_##_sep \
    _macro(readdirp, ## _args) SEP_##_sep \
    _macro(readlink, ## _args) SEP_##_sep \
    _macro(readv, ## _args) SEP_##_sep \
    _macro(removexattr, ## _args) SEP_##_sep \
    _macro(fremovexattr, ## _args) SEP_##_sep \
    _macro(rename, ## _args) SEP_##_sep \
    _macro(rmdir, ## _args) SEP_##_sep \
    _macro(setattr, ## _args) SEP_##_sep \
    _macro(fsetattr, ## _args) SEP_##_sep \
    _macro(setxattr, ## _args) SEP_##_sep \
    _macro(fsetxattr, ## _args) SEP_##_sep \
    _macro(stat, ## _args) SEP_##_sep \
    _macro(fstat, ## _args) SEP_##_sep \
    _macro(statfs, ## _args) SEP_##_sep \
    _macro(symlink, ## _args) SEP_##_sep \
    _macro(truncate, ## _args) SEP_##_sep \
    _macro(ftruncate, ## _args) SEP_##_sep \
    _macro(unlink, ## _args) SEP_##_sep \
    _macro(writev, ## _args) SEP_##_sep \
    _macro(xattrop, ## _args) SEP_##_sep \
    _macro(fxattrop, ## _args)

#define SYS_GF_CBK_APPLY(_sep, _macro, _args...) \
    _macro(forget, ## _args) SEP_##_sep \
    _macro(invalidate, ## _args) SEP_##_sep \
    _macro(release, ## _args) SEP_##_sep \
    _macro(releasedir, ## _args)

#define SYS_GF_TABLE_ENTRY(_fop, _prefix) ._fop = _prefix##_##_fop

#define SYS_GF_FOP_TABLE(_prefix) \
    struct xlator_fops fops = \
    { \
        SYS_GF_FOP_APPLY(COMMA, SYS_GF_TABLE_ENTRY, _prefix) \
    }

#define SYS_GF_CBK_TABLE(_prefix) \
    struct xlator_cbks cbks = \
    { \
        SYS_GF_CBK_APPLY(COMMA, SYS_GF_TABLE_ENTRY, _prefix) \
    }

typedef struct _sys_gf_config
{
    uint32_t page_size;
    uint32_t event_pool_size;
    uint32_t frame_pool_size;
    uint32_t stack_pool_size;
    uint32_t stub_pool_size;
    uint32_t dict_pool_size;
    uint32_t dict_pair_pool_size;
    uint32_t dict_data_pool_size;
    uint32_t log_level;
    uint32_t mac_compat;
    uint32_t fuse_direct_io;
    uint32_t fuse_attribute_timeout;
    uint32_t fuse_entry_timeout;
} sys_gf_config_t;

typedef struct _sys_gf_handlers
{
    int32_t fop;
    void (* wind)(call_frame_t * frame, void * cookie, xlator_t * xl,
                  uintptr_t * cbk, uintptr_t * to, uintptr_t * data);
    void (* wind_tail)(call_frame_t * frame, xlator_t * xl, uintptr_t * cbk,
                       uintptr_t * to, uintptr_t * data);
    void (* unwind)(call_frame_t * frame, int32_t op_ret, int32_t op_errno,
                    uintptr_t * cbk, uintptr_t * to, uintptr_t * data);
    void (* unwind_error)(call_frame_t * frame, int32_t op_errno,
                          dict_t * xdata, uintptr_t * cbk, uintptr_t * to,
                          uintptr_t * data);
} sys_gf_handlers_t;

typedef struct _sys_gf_args
{
    sys_gf_handlers_t * ops;
} sys_gf_args_t;

static inline void sys_gf_wind(call_frame_t * frame, void * cookie,
                               xlator_t * xl, uintptr_t * cbk,
                               uintptr_t * to, uintptr_t * funcs,
                               uintptr_t * data)
{
    sys_gf_args_t * args;

    args = (sys_gf_args_t *)(funcs - SYS_GF_SIZE);
    SYS_ASSERT(
        args->ops->wind != NULL,
        "Call to 'wind' not allowed"
    );
    args->ops->wind(frame, cookie, xl, cbk, to, data);
}

static inline void sys_gf_wind_tail(call_frame_t * frame, xlator_t * xl,
                                    uintptr_t * cbk, uintptr_t * to,
                                    uintptr_t * funcs, uintptr_t * data)
{
    sys_gf_args_t * args;

    args = (sys_gf_args_t *)(funcs - SYS_GF_SIZE);
    SYS_ASSERT(
        args->ops->wind_tail != NULL,
        "Call to 'wind_tail' not allowed"
    );
    args->ops->wind_tail(frame, xl, cbk, to, data);
}

static inline void sys_gf_unwind(call_frame_t * frame, int32_t op_ret,
                                 int32_t op_errno, uintptr_t * cbk,
                                 uintptr_t * to, uintptr_t * funcs,
                                 uintptr_t * data)
{
    sys_gf_args_t * args;

    args = (sys_gf_args_t *)(funcs - SYS_GF_SIZE);
    SYS_ASSERT(
        args->ops->unwind != NULL,
        "Call to 'unwind' not allowed"
    );
    args->ops->unwind(frame, op_ret, op_errno, cbk, to, data);
}

static inline void sys_gf_unwind_error(call_frame_t * frame, int32_t op_errno,
                                       dict_t * xdata, uintptr_t * cbk,
                                       uintptr_t * to, uintptr_t * funcs,
                                       uintptr_t * data)
{
    sys_gf_args_t * args;

    args = (sys_gf_args_t *)(funcs - SYS_GF_SIZE);
    SYS_ASSERT(
        args->ops->unwind_error != NULL,
        "Call to 'unwind_error' not allowed"
    );
    args->ops->unwind_error(frame, op_errno, xdata, cbk, to, data);
}

SYS_GF_FOP_APPLY(SEMIC, SYS_GF_IO_DECLARE);
SYS_GF_FOP_APPLY(SEMIC, SYS_GF_DECLARE);

err_t sys_gf_initialize(sys_gf_config_t * config);
void sys_gf_terminate(void);

#endif /* __SYS_GF_H__ */
