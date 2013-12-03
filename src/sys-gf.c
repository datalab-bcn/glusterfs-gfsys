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

#include <event.h>
#include <call-stub.h>

#define SYS_GF_HANDLER_DEFINE(_fop) \
    void sys_gf_handler_##_fop##_wind(call_frame_t * frame, void * cookie, \
                                      xlator_t * xl, uintptr_t * cbk, \
                                      uintptr_t * to, uintptr_t * data) \
    { \
        SYS_GF_FOP_TYPE(_fop) * args; \
        logT("SYS-GF: fop wind handler '" #_fop "'"); \
        args = (SYS_GF_FOP_TYPE(_fop) *)data; \
        SYS_IO(sys_gf_##_fop##_wind, (frame, cookie, xl, \
                                      SYS_ARGS_LOAD(args, \
                                                    (SYS_GF_ARGS_##_fop))), \
               cbk, to); \
    } \
    void sys_gf_handler_##_fop##_wind_tail(call_frame_t * frame, \
                                           xlator_t * xl, uintptr_t * cbk, \
                                           uintptr_t * to, uintptr_t * data) \
    { \
        SYS_GF_FOP_TYPE(_fop) * args; \
        logT("SYS-GF: fop wind_tail handler '" #_fop "'"); \
        args = (SYS_GF_FOP_TYPE(_fop) *)data; \
        SYS_IO(sys_gf_##_fop##_wind_tail, (frame, xl, \
                                           SYS_ARGS_LOAD(args, \
                                                      (SYS_GF_ARGS_##_fop))), \
               cbk, to); \
    } \
    void sys_gf_handler_call_##_fop##_wind(call_frame_t * frame, \
                                           void * cookie, xlator_t * xl, \
                                           uintptr_t * cbk, uintptr_t * to, \
                                           uintptr_t * data) \
    { \
        SYS_GF_FOP_CALL_TYPE(_fop) * args; \
        logT("SYS-GF: fop call wind handler '" #_fop "'"); \
        args = (SYS_GF_FOP_CALL_TYPE(_fop) *)data; \
        SYS_IO(sys_gf_##_fop##_wind, (args->frame, cookie, xl, \
                                      SYS_ARGS_LOAD(args, \
                                                    (SYS_GF_ARGS_##_fop))), \
               cbk, to); \
    } \
    void sys_gf_handler_call_##_fop##_wind_tail(call_frame_t * frame, \
                                                xlator_t * xl, \
                                                uintptr_t * cbk, \
                                                uintptr_t * to, \
                                                uintptr_t * data) \
    { \
        SYS_GF_FOP_CALL_TYPE(_fop) * args; \
        logT("SYS-GF: fop call wind_tail handler '" #_fop "'"); \
        args = (SYS_GF_FOP_CALL_TYPE(_fop) *)data; \
        SYS_IO(sys_gf_##_fop##_wind_tail, (args->frame, xl, \
                                           SYS_ARGS_LOAD(args, \
                                                      (SYS_GF_ARGS_##_fop))), \
               cbk, to); \
    } \
    void sys_gf_handler_##_fop##_unwind(call_frame_t * frame, int32_t op_ret, \
                                        int32_t op_errno, uintptr_t * cbk, \
                                        uintptr_t * to, uintptr_t * data) \
    { \
        SYS_GF_CBK_TYPE(_fop) * args; \
        logT("SYS-GF: cbk unwind handler '" #_fop "'"); \
        args = (SYS_GF_CBK_TYPE(_fop) *)data; \
        SYS_IO(sys_gf_##_fop##_unwind, (frame, op_ret, op_errno, \
                                        SYS_ARGS_LOAD(args, \
                                                (SYS_GF_ARGS_##_fop##_cbk))), \
               cbk, to); \
    } \
    void sys_gf_handler_##_fop##_unwind_error(call_frame_t * frame, \
                                              int32_t op_errno, \
                                              dict_t * xdata, \
                                              uintptr_t * cbk, \
                                              uintptr_t * to, \
                                              uintptr_t * data) \
    { \
        logT("SYS-GF: cbk unwind_error handler '" #_fop "'"); \
        SYS_IO(sys_gf_##_fop##_unwind_error, (frame, op_errno, xdata), \
               cbk, to); \
    } \
    void sys_gf_handler_call_##_fop##_unwind(call_frame_t * frame, \
                                             int32_t op_ret, \
                                             int32_t op_errno, \
                                             uintptr_t * cbk, \
                                             uintptr_t * to, \
                                             uintptr_t * data) \
    { \
        SYS_GF_CBK_CALL_TYPE(_fop) * args; \
        logT("SYS-GF: cbk call unwind handler '" #_fop "'"); \
        args = (SYS_GF_CBK_CALL_TYPE(_fop) *)data; \
        SYS_IO(sys_gf_##_fop##_unwind, (args->frame, args->op_ret, \
                                        args->op_errno, \
                                        SYS_ARGS_LOAD(args, \
                                                (SYS_GF_ARGS_##_fop##_cbk))), \
               cbk, to); \
    } \
    void sys_gf_handler_call_##_fop##_unwind_error(call_frame_t * frame, \
                                                   int32_t op_errno, \
                                                   dict_t * xdata, \
                                                   uintptr_t * cbk, \
                                                   uintptr_t * to, \
                                                   uintptr_t * data) \
    { \
        SYS_GF_CBK_CALL_TYPE(_fop) * args; \
        logT("SYS-GF: cbk call unwind_error handler '" #_fop "'"); \
        args = (SYS_GF_CBK_CALL_TYPE(_fop) *)data; \
        SYS_IO(sys_gf_##_fop##_unwind_error, (args->frame, op_errno, xdata), \
               cbk, to); \
    } \
    sys_gf_handlers_t sys_gf_handlers_fop_##_fop = \
    { \
        .wind         = sys_gf_handler_##_fop##_wind, \
        .wind_tail    = sys_gf_handler_##_fop##_wind_tail, \
        .unwind       = sys_gf_handler_call_##_fop##_unwind, \
        .unwind_error = sys_gf_handler_##_fop##_unwind_error \
    }; \
    sys_gf_handlers_t sys_gf_handlers_fop_call_##_fop = \
    { \
        .wind         = sys_gf_handler_call_##_fop##_wind, \
        .wind_tail    = sys_gf_handler_call_##_fop##_wind_tail, \
        .unwind       = sys_gf_handler_call_##_fop##_unwind, \
        .unwind_error = sys_gf_handler_call_##_fop##_unwind_error \
    }; \
    sys_gf_handlers_t sys_gf_handlers_cbk_##_fop = \
    { \
        .wind         = NULL, \
        .wind_tail    = NULL, \
        .unwind       = sys_gf_handler_##_fop##_unwind, \
        .unwind_error = sys_gf_handler_##_fop##_unwind_error \
    }; \
    sys_gf_handlers_t sys_gf_handlers_cbk_call_##_fop = \
    { \
        .wind         = NULL, \
        .wind_tail    = NULL, \
        .unwind       = sys_gf_handler_call_##_fop##_unwind, \
        .unwind_error = sys_gf_handler_call_##_fop##_unwind_error \
    }


#define SYS_GF_DEFINE(_fop) \
    SYS_GF_HANDLER_DEFINE(_fop); \
    void sys_gf_fop_##_fop##_free(uintptr_t * data) \
    { \
        logT("SYS-GF: fop free '" #_fop "'"); \
        SYS_ARGS_FREE((SYS_GF_FOP_TYPE(_fop) *)data, (SYS_GF_ARGS_##_fop)); \
        __sys_gf_args_release(data); \
    } \
    void sys_gf_cbk_##_fop##_free(uintptr_t * data) \
    { \
        logT("SYS-GF: cbk free '" #_fop "'"); \
        SYS_ARGS_FREE((SYS_GF_CBK_TYPE(_fop) *)data, \
                      (SYS_GF_ARGS_##_fop##_cbk)); \
        __sys_gf_args_release(data); \
    } \
    void sys_gf_fop_call_##_fop##_free(uintptr_t * data) \
    { \
        logT("SYS-GF: fop call free '" #_fop "'"); \
        SYS_ARGS_FREE((SYS_GF_FOP_CALL_TYPE(_fop) *)data, \
                      (SYS_GF_ARGS_FOP, SYS_GF_ARGS_##_fop)); \
        __sys_gf_args_release(data); \
    } \
    void sys_gf_cbk_call_##_fop##_free(uintptr_t * data) \
    { \
        logT("SYS-GF: cbk call free '" #_fop "'"); \
        SYS_ARGS_FREE((SYS_GF_CBK_CALL_TYPE(_fop) *)data, \
                      (SYS_GF_ARGS_CBK, SYS_GF_ARGS_##_fop##_cbk)); \
        __sys_gf_args_release(data); \
    }

SYS_ASYNC_TO_CREATE(__sys_gf_args_release, ((uintptr_t *, data)))
{
    uintptr_t owner;

    owner = sys_calls_owned_owner(data);
    if (sys_async_self->head.id == owner)
    {
        sys_calls_owned_release(&sys_async_owned_calls->head, data);
    }
    else
    {
        SYS_ASYNC_TO(sys_async_queue_get(owner), __sys_gf_args_release,
                     (data));
    }
}

SYS_GF_FOP_APPLY(, SYS_GF_DEFINE)

/*
 * sys_gf_access(frame, callback, cookie, this, ...)
 * sys_gf_args_access(frame, callback, cookie, this, args)
 * sys_gf_forward_access(frame, this, ...)
 * sys_gf_args_forward_access(frame, this, args)
 * sys_gf_return_access(frame, ret, errno, ...)
 * sys_gf_args_return_access(frame, ret, errno, args)
 */
#define SYS_GF_IO_DEFINE(_fop) \
    static SYS_IO_CBK_DEFINE(sys_gf_##_fop##_wind, int32_t, \
                             (SYS_GF_ARGS_CBK, SYS_GF_ARGS_##_fop##_cbk)) \
    { \
        logT("SYS-GF: callback '" #_fop "'"); \
        uintptr_t * data = (uintptr_t *)cookie; \
        cookie = *(void **)sys_io_data(data); \
        SYS_IO_RESUME(sys_gf_##_fop##_wind, data, \
                      (op_ret < 0) ? op_errno : 0, \
                      SYS_ARGS_NAMES((SYS_GF_ARGS_CBK, \
                                      SYS_GF_ARGS_##_fop##_cbk))); \
        return 0; \
    } \
    void sys_gf_##_fop##_wind(call_frame_t * frame, fop_##_fop##_cbk_t rfn, \
                              void * cookie, xlator_t * xl, \
                              SYS_ARGS_DECL((SYS_GF_ARGS_##_fop))) \
    { \
        logT("SYS-GF: wind '" #_fop "'"); \
        STACK_WIND_COOKIE(frame, rfn, cookie, xl, xl->fops->_fop, \
                          SYS_ARGS_NAMES((SYS_GF_ARGS_##_fop))); \
    } \
    void sys_gf_##_fop##_wind_tail(call_frame_t * frame, xlator_t * xl, \
                                   SYS_ARGS_DECL((SYS_GF_ARGS_##_fop))) \
    { \
        logT("SYS-GF: wind_tail '" #_fop "'"); \
        STACK_WIND_TAIL(frame, xl, xl->fops->_fop, \
                        SYS_ARGS_NAMES((SYS_GF_ARGS_##_fop))); \
    } \
    void sys_gf_##_fop##_unwind(call_frame_t * frame, int32_t op_ret, \
                                int32_t op_errno, \
                                SYS_ARGS_DECL((SYS_GF_ARGS_##_fop##_cbk))) \
    { \
        logT("SYS-GF: unwind '" #_fop "'"); \
        STACK_UNWIND_STRICT(_fop, frame, op_ret, op_errno, \
                            SYS_ARGS_NAMES((SYS_GF_ARGS_##_fop##_cbk))); \
    } \
    void sys_gf_##_fop##_unwind_error(call_frame_t * frame, int32_t op_errno, \
                                      dict_t * xdata) \
    { \
        logT("SYS-GF: unwind_error '" #_fop "'"); \
        STACK_UNWIND_STRICT(_fop, frame, -1, op_errno, \
                            SYS_ARGS_APPLY(SYS_GF_NULL, \
                                           (SYS_GF_ARGS_##_fop##_cbk), \
                                           _fop)); \
    } \
    SYS_IO_DEFINE(sys_gf_##_fop##_wind, __sys_gf_io, \
                  (SYS_GF_IO_WIND_ARGS, SYS_GF_ARGS_##_fop), \
                  (SYS_GF_ARGS_CBK, SYS_GF_ARGS_##_fop##_cbk)) \
    { \
        logT("SYS-GF: submitting fop wind '" #_fop "'"); \
        *sys_io_data(__sys_gf_io) = (uintptr_t)cookie; \
        sys_gf_##_fop##_wind(frame, SYS_IO_CBK(sys_gf_##_fop##_wind), \
                             __sys_gf_io, this, \
                             SYS_ARGS_NAMES((SYS_GF_ARGS_##_fop))); \
    } \
    SYS_IO_DEFINE(sys_gf_##_fop##_wind_tail, __sys_gf_io, \
                  (SYS_GF_IO_WIND_TAIL_ARGS, SYS_GF_ARGS_##_fop), \
                  (SYS_GF_ARGS_FOP)) \
    { \
        logT("SYS-GF: submitting fop wind_tail '" #_fop "'"); \
        sys_gf_##_fop##_wind_tail(frame, this, \
                        SYS_ARGS_NAMES((SYS_GF_ARGS_##_fop))); \
        SYS_IO_RESUME(sys_gf_##_fop##_wind_tail, __sys_gf_io, 0, frame, \
                      this); \
    } \
    SYS_IO_DEFINE(sys_gf_##_fop##_unwind, __sys_gf_io, \
                  (SYS_GF_IO_UNWIND_ARGS, SYS_GF_ARGS_##_fop##_cbk), \
                  (SYS_GF_IO_UNWIND_ARGS)) \
    { \
        logT("SYS-GF: submitting fop unwind '" #_fop "'"); \
        sys_gf_##_fop##_unwind(frame, op_ret, op_errno, \
                               SYS_ARGS_NAMES((SYS_GF_ARGS_##_fop##_cbk))); \
        SYS_IO_RESUME(sys_gf_##_fop##_unwind, __sys_gf_io, 0, frame, \
                      op_ret, op_errno); \
    } \
    SYS_IO_DEFINE(sys_gf_##_fop##_unwind_error, __sys_gf_io, \
                  (SYS_GF_IO_UNWIND_ERROR_ARGS, (dict_t *, xdata)), \
                  (SYS_GF_IO_UNWIND_ERROR_ARGS)) \
    { \
        logT("SYS-GF: submitting fop unwind_error '" #_fop "'"); \
        sys_gf_##_fop##_unwind_error(frame, op_errno, xdata); \
        SYS_IO_RESUME(sys_gf_##_fop##_unwind_error, __sys_gf_io, 0, frame, \
                      op_errno); \
    }

#define SYS_GF_NULL(_arg, _fop, _more...) \
    ({ \
        typeof(ARG_TYPE(_arg)) __data = 0; \
        if ((void *)&((SYS_GF_CBK_TYPE(_fop) *)0)->SYS_ARG_NAME(_arg) == \
            (void *)&((SYS_GF_CBK_TYPE(_fop) *)0)->xdata) \
        { \
            __data = (ARG_TYPE(_arg))(uintptr_t)xdata; \
        } \
        __data; \
    })

static sys_gf_config_t sys_gf_config =
{
    .page_size              = 128,
    .event_pool_size        = 16384,
    .frame_pool_size        = 4096,
    .stack_pool_size        = 1024,
    .stub_pool_size         = 1024,
    .dict_pool_size         = 4096,
    .dict_pair_pool_size    = 16384,
    .dict_data_pool_size    = 16384,
    .log_level              = LOG_I,
    .mac_compat             = GF_OPTION_DISABLE,
#ifdef GF_DARWIN_HOST_OS
    .fuse_direct_io         = GF_OPTION_DISABLE,
#else
    .fuse_direct_io         = GF_OPTION_DEFERRED,
#endif
    .fuse_attribute_timeout = -1,
    .fuse_entry_timeout     = -1
};

err_t sys_gf_initialize_global_pools(glusterfs_ctx_t * ctx)
{
    err_t error;

    SYS_PTR(
        &ctx->iobuf_pool, iobuf_pool_new, (),
        ENOMEM,
        E(),
        RETERR()
    );
    SYS_PTR(
        &ctx->event_pool, event_pool_new, (sys_gf_config.event_pool_size),
        ENOMEM,
        E(),
        GOTO(failed, &error)
    );
    SYS_POOL_CREATE(
        &ctx->stub_mem_pool, call_stub_t, sys_gf_config.stub_pool_size,
        E(),
        GOTO(failed_event, &error)
    );
    SYS_POOL_CREATE(
        &ctx->dict_pool, dict_t, sys_gf_config.dict_pool_size,
        E(),
        GOTO(failed_stub, &error)
    );
    SYS_POOL_CREATE(
        &ctx->dict_pair_pool, data_pair_t, sys_gf_config.dict_pair_pool_size,
        E(),
        GOTO(failed_dict, &error)
    );
    SYS_POOL_CREATE(
        &ctx->dict_data_pool, data_t, sys_gf_config.dict_data_pool_size,
        E(),
        GOTO(failed_pair, &error)
    );

    return 0;

failed_pair:
    SYS_POOL_DESTROY(&ctx->dict_pair_pool);
failed_dict:
    SYS_POOL_DESTROY(&ctx->dict_pool);
failed_stub:
    SYS_POOL_DESTROY(&ctx->stub_mem_pool);
failed_event:
    // TODO: There is no defined way to destroy the event_pool
//    event_pool_destroy(ctx->event_pool);
    ctx->event_pool = NULL;
failed:
    iobuf_pool_destroy(ctx->iobuf_pool);
    ctx->iobuf_pool = NULL;

    return error;
}

void sys_gf_terminate_global_pools(glusterfs_ctx_t * ctx)
{
    SYS_POOL_DESTROY(&ctx->dict_data_pool);
    SYS_POOL_DESTROY(&ctx->dict_pair_pool);
    SYS_POOL_DESTROY(&ctx->dict_pool);
    SYS_POOL_DESTROY(&ctx->stub_mem_pool);

    // TODO: There is no defined way to destroy the event_pool
//    event_pool_destroy(ctx->event_pool);
    ctx->event_pool = NULL;

    iobuf_pool_destroy(ctx->iobuf_pool);
    ctx->iobuf_pool = NULL;
}

err_t sys_gf_initialize_call_pool(glusterfs_ctx_t * ctx)
{
    err_t error;

    call_pool_t * pool;

    SYS_MALLOC(
        &pool, sys_mt_call_pool_t,
        E(),
        RETERR()
    );

    SYS_POOL_CREATE(
        &pool->frame_mem_pool, call_frame_t, sys_gf_config.frame_pool_size,
        E(),
        GOTO(failed, &error)
    );
    SYS_POOL_CREATE(
        &pool->stack_mem_pool, call_stack_t, sys_gf_config.stack_pool_size,
        E(),
        GOTO(failed_frame, &error)
    );

    INIT_LIST_HEAD(&pool->all_frames);
    LOCK_INIT(&pool->lock);

    ctx->pool = pool;

    return 0;

failed_frame:
    SYS_POOL_DESTROY(&pool->frame_mem_pool);
failed:
    SYS_FREE(pool);

    return error;
}

void sys_gf_terminate_call_pool(glusterfs_ctx_t * ctx)
{
    call_pool_t * pool;

    pool = ctx->pool;

    SYS_POOL_DESTROY(&pool->stack_mem_pool);
    SYS_POOL_DESTROY(&pool->frame_mem_pool);

    SYS_FREE(ctx->pool);
}

err_t sys_gf_initialize_pools(glusterfs_ctx_t * ctx)
{
    err_t error;

    SYS_CALL(
        sys_gf_initialize_global_pools, (ctx),
        E(),
        RETERR()
    );

    SYS_CALL(
        sys_gf_initialize_call_pool, (ctx),
        E(),
        GOTO(failed, &error)
    );

    return 0;

failed:
    sys_gf_terminate_global_pools(ctx);

    return error;
}

void sys_gf_terminate_pools(glusterfs_ctx_t * ctx)
{
    sys_gf_terminate_call_pool(ctx);
    sys_gf_terminate_global_pools(ctx);
}

err_t sys_gf_initialize_defaults(glusterfs_ctx_t * ctx)
{
    err_t error;

    xlator_mem_acct_init(THIS, sys_mt_end);

    SYS_PTR(
        &ctx->process_uuid, generate_glusterfs_ctx_id, (),
        ENOMEM,
        E(),
        RETERR()
    );

    ctx->page_size = sys_gf_config.page_size * GF_UNIT_KB;

    SYS_CALL(
        sys_gf_initialize_pools, (ctx),
        E(),
        GOTO(failed, &error)
    );

    ctx->cmd_args.log_level = sys_gf_config.log_level;
    ctx->cmd_args.mac_compat = sys_gf_config.mac_compat;
    ctx->cmd_args.fuse_direct_io_mode = sys_gf_config.fuse_direct_io;
    ctx->cmd_args.fuse_attribute_timeout =
        sys_gf_config.fuse_attribute_timeout;
    ctx->cmd_args.fuse_entry_timeout = sys_gf_config.fuse_entry_timeout;

    INIT_LIST_HEAD(&ctx->cmd_args.xlator_options);

    return 0;

failed:
    SYS_FREE(ctx->process_uuid);

    return error;
}

void sys_gf_terminate_defaults(glusterfs_ctx_t * ctx)
{
    sys_gf_terminate_pools(ctx);
    SYS_FREE(ctx->process_uuid);
}

err_t sys_gf_initialize_log(glusterfs_ctx_t * ctx)
{
    if (ctx->cmd_args.log_file == NULL)
    {
        ctx->cmd_args.log_file = "-";
    }

    SYS_CODE(
        gf_log_init, (ctx, ctx->cmd_args.log_file),
        EINVAL,
        E(),
        RETERR()
    );

    gf_log_set_loglevel(ctx->cmd_args.log_level);

    return 0;
}

void sys_gf_terminate_log(glusterfs_ctx_t * ctx)
{
    // TODO: Do proper cleanup
}

err_t sys_gf_initialize(sys_gf_config_t * config)
{
    glusterfs_ctx_t * ctx;
    err_t error;

    if (config != NULL)
    {
        sys_gf_config = * config;
    }
    if (sys_gf_config.page_size < 4)
    {
        sys_gf_config.page_size = 4;
    }
    if (sys_gf_config.event_pool_size < 1024)
    {
        sys_gf_config.event_pool_size = 1024;
    }
    if (sys_gf_config.frame_pool_size < 1024)
    {
        sys_gf_config.frame_pool_size = 1024;
    }
    if (sys_gf_config.stack_pool_size < 1024)
    {
        sys_gf_config.stack_pool_size = 1024;
    }
    if (sys_gf_config.stub_pool_size < 1024)
    {
        sys_gf_config.stub_pool_size = 1024;
    }
    if (sys_gf_config.dict_pool_size < 1024)
    {
        sys_gf_config.dict_pool_size = 1024;
    }
    if (sys_gf_config.dict_pair_pool_size < 1024)
    {
        sys_gf_config.dict_pair_pool_size = 1024;
    }
    if (sys_gf_config.dict_data_pool_size < 1024)
    {
        sys_gf_config.dict_data_pool_size = 1024;
    }
    if (sys_gf_config.log_level < LOG_A)
    {
        sys_gf_config.log_level = LOG_A;
    }
    if (sys_gf_config.log_level > LOG_T)
    {
        sys_gf_config.log_level = LOG_T;
    }

    SYS_PTR(
        &ctx, glusterfs_ctx_new, (),
        ENOMEM,
        E(),
        RETERR()
    );
    SYS_CODE(
        glusterfs_globals_init, (ctx),
        ENOMEM,
        E(),
        GOTO(failed, &error)
    );

    THIS->ctx = ctx;

    SYS_CALL(
        sys_gf_initialize_defaults, (ctx),
        E(),
        GOTO(failed, &error)
    );

    SYS_CALL(
        sys_gf_initialize_log, (ctx),
        E(),
        GOTO(failed_defaults, &error)
    );

    logN("glusterfs library initialized");

    return 0;

failed_defaults:
    sys_gf_terminate_defaults(ctx);
failed:
    free(ctx);

    return error;
}

void sys_gf_terminate(void)
{
    glusterfs_ctx_t * ctx;

    logN("Shutting down glusterfs library");

    ctx = THIS->ctx;

    sys_gf_terminate_defaults(ctx);
    sys_gf_terminate_log(ctx);

    THIS->ctx = NULL;

    free(ctx);
}

SYS_GF_FOP_APPLY(, SYS_GF_IO_DEFINE)
