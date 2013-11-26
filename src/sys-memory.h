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

#ifndef __SYS_MEMORY_H__
#define __SYS_MEMORY_H__

#include <mem-pool.h>

#include "sys-base.h"
#include "sys-log.h"
#include "sys-error.h"

#define SYS_MEM_LABEL __label_memory_retry

#define __SYS_MEM_NO_FAIL(_err) \
    do \
    { \
        sys_memory_emergency_use(); \
        _err = 0; \
        goto SYS_MEM_LABEL; \
    } while (0)

#define __SYS_ACTION_NO_FAIL() __SYS_MEM_NO_FAIL

#define __SYS_ALLOC(_err, _sz, _res, _pptr, _size, _type, _log, _acts...) \
    ({ \
        __label__ SYS_MEM_LABEL; \
        size_t _sz = (_size); \
        typeof(_pptr) _res = (_pptr); \
        err_t _err; \
        goto SYS_MEM_LABEL; /* Avoid "label not used" warning */ \
SYS_MEM_LABEL: \
        _err = SYS_PTR(_res, GF_MALLOC, (_sz, _type), ENOMEM, \
                       D(all, "sys-memory"), \
                       LOG(Dm(all, "sys-memory"), _sz), \
                       LOG(_log, "Unable to allocate %lu bytes of memory " \
                                 "for '" #_type "'", _sz), \
                       ## _acts); \
        if (_err == 0) \
        { \
            logT("Allocated %lu bytes of memory for '" #_type "' at %p", \
                    _sz, *_res); \
        } \
        _err; \
    })

#define __SYS_ALLOC0(_err, _sz, _res, _pptr, _size, _type, _log, _acts...) \
    ({ \
        size_t _sz = (_size); \
        typeof(_pptr) _res = (_pptr); \
        err_t _err = SYS_ALLOC(_res, _sz, _type, _log, ## _acts); \
        if (_err == 0) \
        { \
            memset(*_res, 0, _sz); \
        } \
        _err; \
    })

#define __SYS_RESIZE(_err, _sz, _res, _old, _pptr, _size, _type, _log, \
                     _acts...) \
    ({ \
        __label__ SYS_MEM_LABEL; \
        size_t _sz = (_size); \
        typeof(_pptr) _res = (_pptr); \
        typeof(*(_pptr)) _old = *_res; \
        err_t _err; \
        goto SYS_MEM_LABEL; /* Avoid "label not used" warning */ \
SYS_MEM_LABEL: \
        _err = SYS_PTR(_res, GF_REALLOC, (_old, _sz), ENOMEM, \
                       D(all, "sys-memory"), \
                       LOG(Dm(all, "sys-memory"), _sz), \
                       LOG(_log, "Unable to resize memory block %p to %lu " \
                                 "bytes for '" #_type "'", _old, _sz), \
                       ## _acts); \
        if (_err == 0) \
        { \
            logT("Memory block at %p resized to %p with %lu bytes", \
                 _old, *_res, _sz); \
        } \
        _err; \
    })

#define __SYS_ALLOC_ALIGNED(_err, _sz, _res, _pptr, _size, _algn, _type, \
                            _log, _acts...) \
    ({ \
        __label__ SYS_MEM_LABEL; \
        size_t _sz = (_size); \
        typeof(_pptr) _res = (_pptr); \
        err_t _err; \
        goto SYS_MEM_LABEL; /* Avoid "label not used" warning */ \
SYS_MEM_LABEL: \
        _err = SYS_CALL(posix_memalign, ((void **)_res, (_algn), _sz), \
                        D(all, "sys-memory"), \
                        LOG(Dm(all, "sys-memory"), _sz), \
                        LOG(_log, "Unable to allocate %lu bytes of aligned " \
                                  "memory for '" #_type "'", _sz), \
                        ## _acts); \
        if (_err == 0) \
        { \
            logT("Allocated %lu bytes of aligned memory for '" #_type "' " \
                 "at %p", _sz, *_res); \
        } \
        _err; \
    })

#define __SYS_ALLOC0_ALIGNED(_err, _sz, _res, _pptr, _size, _algn, _type, \
                             _log, _acts...) \
    ({ \
        size_t _sz = (_size); \
        typeof(_pptr) _res = (_pptr); \
        err_t _err = SYS_ALLOC_ALIGNED(_res, _sz, _algn, _type, _log, \
                                       ## _acts); \
        if (_err == 0) \
        { \
            memset(*_res, 0, _sz); \
        } \
        _err; \
    })

#define __SYS_FREE(_tmp, _ptr) \
    do \
    { \
        typeof(_ptr) _tmp = (_ptr); \
        logT("Releasing memory block at %p", _tmp); \
        GF_FREE(_tmp); \
    } while (0)

#define __SYS_FREE_ALIGNED(_tmp, _ptr) \
    do \
    { \
        typeof(_ptr) _tmp = (_ptr); \
        logT("Releasing aligned memory block at %p", _tmp); \
        free(_tmp); \
    } while (0)

#define __SYS_POOL_CREATE(_err, _sz, _res, _ppool, _type, _count, _log, \
                          _acts...) \
    ({ \
        __label__ SYS_MEM_LABEL; \
        size_t _sz = (_count); \
        struct mem_pool ** _res = (_ppool); \
        err_t _err; \
        goto SYS_MEM_LABEL; /* Avoid "label not used" warning */ \
SYS_MEM_LABEL: \
        _err = SYS_PTR(_res, mem_pool_new, (_type, _sz), ENOMEM, \
                       D(all, "sys-memory"), \
                       LOG(_log, "Unable to create a memory pool for '" \
                                 #_type "' with %lu elements", _sz), \
                       ## _acts); \
        if (_err == 0) \
        { \
            logT("Created memory pool for " #_type " with %lu elements at " \
                 "%p", _sz, *_res); \
        } \
        _err; \
    })

#define __SYS_POOL_DESTROY(_tmp, _ppool) \
    do \
    { \
        struct mem_pool ** _tmp = (_ppool); \
        logT("Destroying memory pool at %p", *_tmp); \
        mem_pool_destroy(*_tmp); \
        *_tmp = NULL; \
    } while (0)

#define __SYS_POOL_ALLOC(_err, _tmp, _res, _pptr, _pool, _type, _log, \
                         _acts...) \
    ({ \
        __label__ SYS_MEM_LABEL; \
        struct mem_pool * _tmp = (_pool); \
        typeof(_pptr) _res = (_pptr); \
        err_t _err; \
        goto SYS_MEM_LABEL; /* Avoid "label not used" warning */ \
SYS_MEM_LABEL: \
        _err = SYS_PTR(_res, mem_get, (_tmp), ENOMEM, \
                       D(all, "sys-memory"), \
                       LOG(_log, "Unable to allocate an element '" #_type \
                                 "' from pool %p", _tmp), \
                       ## _acts); \
        if (_err == 0) \
        { \
            logT("Returned element %p from pool %p (" #_type ")", \
                 *_res, _tmp); \
        } \
        _err; \
    })

#define __SYS_POOL_ALLOC0(_err, _tmp, _res, _pptr, _pool, _type, _log, \
                          _acts...) \
    ({ \
        __label__ SYS_MEM_LABEL; \
        struct mem_pool * _tmp = (_pool); \
        typeof(_pptr) _res = (_pptr); \
        err_t _err; \
        goto SYS_MEM_LABEL; /* Avoid "label not used" warning */ \
SYS_MEM_LABEL: \
        _err = SYS_PTR(_res, mem_get0, (_tmp), ENOMEM, \
                       D(all, "sys-memory"), \
                       LOG(_log, "Unable to allocate an element '" #_type \
                                 "' from pool %p", _tmp), \
                       ## _acts); \
        if (_err == 0) \
        { \
            logT("Returned element %p from pool %p (" #_type ")", \
                 *_res, _tmp); \
        } \
        _err; \
    })

#define __SYS_POOL_FREE(_tmp, _ptr) \
    do \
    { \
        typeof(_ptr) _tmp = (_ptr); \
        logT("Releasing element %p from pool", _tmp); \
        mem_put(_tmp); \
    } while (0)

#define _SYS_ALLOC(_args...) __SYS_ALLOC(_args)
#define SYS_ALLOC(_pptr, _size, _type, _log, _acts...) \
    _SYS_ALLOC(SYS_TMP(3), _pptr, _size, _type, _log, ## _acts)

#define _SYS_ALLOC0(_args...) __SYS_ALLOC0(_args)
#define SYS_ALLOC0(_pptr, _size, _type, _log, _acts...) \
    _SYS_ALLOC0(SYS_TMP(3), _pptr, _size, _type, _log, ## _acts)

#define _SYS_RESIZE(_args...) __SYS_RESIZE(_args)
#define SYS_RESIZE(_pptr, _size, _type, _log, _acts...) \
    _SYS_RESIZE(SYS_TMP(4), _pptr, _size, _type, _log, ## _acts)

#define _SYS_ALLOC_ALIGNED(_args...) __SYS_ALLOC_ALIGNED(_args)
#define SYS_ALLOC_ALIGNED(_pptr, _size, _algn, _type, _log, _acts...) \
    _SYS_ALLOC_ALIGNED(SYS_TMP(3), _pptr, _size, _algn, _type, _log, ## _acts)

#define _SYS_ALLOC0_ALIGNED(_args...) __SYS_ALLOC0_ALIGNED(_args)
#define SYS_ALLOC0_ALIGNED(_pptr, _size, _algn, _type, _log, _acts...) \
    _SYS_ALLOC0_ALIGNED(SYS_TMP(3), _pptr, _size, _algn, _type, _log, \
                        ## _acts)

#define _SYS_FREE(_args...) __SYS_FREE(_args)
#define SYS_FREE(_ptr) \
    _SYS_FREE(SYS_TMP(1), _ptr)

#define _SYS_FREE_ALIGNED(_args...) __SYS_FREE_ALIGNED(_args)
#define SYS_FREE_ALIGNED(_ptr) \
    _SYS_FREE_ALIGNED(SYS_TMP(1), _ptr)

#define SYS_MALLOC(_pptr, _type, _log, _acts...) \
    SYS_ALLOC(_pptr, sizeof(**(_pptr)), _type, _log, ## _acts)

#define SYS_MALLOC0(_pptr, _type, _log, _acts...) \
    SYS_ALLOC0(_pptr, sizeof(**(_pptr)), _type, _log, ## _acts)

#define SYS_MALLOC_ALIGNED(_pptr, _algn, _type, _log, _acts...) \
    SYS_ALLOC_ALIGNED(_pptr, sizeof(**(_pptr)), _algn, _type, _log, ## _acts)

#define SYS_MALLOC0_ALIGNED(_pptr, _algn, _type, _log, _acts...) \
    SYS_ALLOC0_ALIGNED(_pptr, sizeof(**(_pptr)), _algn, _type, _log, ## _acts)

#define SYS_CALLOC(_pptr, _count, _type, _log, _acts...) \
    SYS_ALLOC(_pptr, sizeof(**(_pptr)) * (_count), _type, _log, ## _acts)

#define SYS_CALLOC0(_pptr, _count, _type, _log, _acts...) \
    SYS_ALLOC0(_pptr, sizeof(**(_pptr)) * (_count), _type, _log, ## _acts)

#define SYS_CALLOC_ALIGNED(_pptr, _count, _algn, _type, _log, _acts...) \
    SYS_ALLOC_ALIGNED(_pptr, sizeof(**(_pptr)) * (_count), _algn, _type, \
                      _log, ## _acts)

#define SYS_CALLOC0_ALIGNED(_pptr, _count, _algn, _type, _log, _acts...) \
    SYS_ALLOC0_ALIGNED(_pptr, sizeof(**(_pptr)) * (_count), _algn, _type, \
                       _log, ## _acts)

#define SYS_CRESIZE(_pptr, _count, _type, _log, _acts...) \
    SYS_RESIZE(_pptr, sizeof(**(_pptr)) * (_count), _type, _log, ## _acts)

#define _SYS_POOL_CREATE(_args...) __SYS_POOL_CREATE(_args)
#define SYS_POOL_CREATE(_ppool, _type, _count, _log, _acts...) \
    _SYS_POOL_CREATE(SYS_TMP(3), _ppool, _type, _count, _log, ## _acts)

#define _SYS_POOL_DESTROY(_args...) __SYS_POOL_DESTROY(_args)
#define SYS_POOL_DESTROY(_ppool) \
    _SYS_POOL_DESTROY(SYS_TMP(1), _ppool)

#define _SYS_POOL_ALLOC(_args...) __SYS_POOL_ALLOC(_args)
#define SYS_POOL_ALLOC(_pptr, _pool, _type, _log, _acts...) \
    _SYS_POOL_ALLOC(SYS_TMP(3), _pptr, _pool, _type, _log, ## _acts)

#define _SYS_POOL_ALLOC0(_args...) __SYS_POOL_ALLOC0(_args)
#define SYS_POOL_ALLOC0(_pptr, _pool, _type, _log, _acts...) \
    _SYS_POOL_ALLOC0(SYS_TMP(3), _pptr, _pool, _type, _log, ## _acts)

#define _SYS_POOL_FREE(_args...) __SYS_POOL_FREE(_args)
#define SYS_POOL_FREE(_ptr) \
    _SYS_POOL_FREE(SYS_TMP(1), _ptr)

enum sys_mem_types
{
    sys_mt_emergency = gf_common_mt_end + 1,
    sys_mt_uint8_t,
    sys_mt_int64_t,
    sys_mt_list_head,
    sys_mt_pthread_t,
    sys_mt_call_pool_t,
    sys_mt_sys_calls_t,
    sys_mt_sys_async_queue_t,
    sys_mt_sys_heap_t,
    sys_mt_sys_heap_item_t,
    sys_mt_end
};

void sys_memory_emergency_use(void);

#endif /* __SYS_MEMORY_H__ */
