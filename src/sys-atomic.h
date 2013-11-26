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

#ifndef __SYS_ATOMIC_H__
#define __SYS_ATOMIC_H__

#include "sys-compiler.h"

#if !defined(atomic_add) || \
    !defined(atomic_xchg) || \
    !defined(atomic_cmpxchg) || \
    !defined(atomic_and) || \
    !defined(atomic_or) || \
    !defined(atomic_xor)
#error "Undefined basic atomic primitives"
#endif

#ifndef atomic_add_return
#define atomic_add_return(_ptr, _val, _memmodel) \
    ({ \
        typeof(_val) __tmp_val = (_val); \
        atomic_add(_ptr, __tmp_val, _memmodel) + __tmp_val; \
    })
#endif

#ifndef atomic_sub
#define atomic_sub(_ptr, _val, _memmodel) atomic_add(_ptr, -(_val), _memmodel)
#endif

#ifndef atomic_sub_return
#define atomic_sub_return(_ptr, _val, _memmodel) \
    ({ \
        typeof(_val) __tmp_val = (_val); \
        atomic_sub(_ptr, __tmp_val, _memmodel) - __tmp_val; \
    })
#endif

#ifndef atomic_and_return
#define atomic_and_return(_ptr, _val, _memmodel) \
    ({ \
        typeof(_val) __tmp_val = (_val); \
        atomic_and(_ptr, __tmp_val, _memmodel) & __tmp_val; \
    })
#endif

#ifndef atomic_xor_return
#define atomic_xor_return(_ptr, _val, _memmodel) \
    ({ \
        typeof(_val) __tmp_val = (_val); \
        atomic_xor(_ptr, __tmp_val, _memmodel) ^ __tmp_val; \
    })
#endif

#ifndef atomic_or_return
#define atomic_or_return(_ptr, _val, _memmodel) \
    ({ \
        typeof(_val) __tmp_val = (_val); \
        atomic_or(_ptr, __tmp_val, _memmodel) | __tmp_val; \
    })
#endif

#ifndef atomic_inc
#define atomic_inc(_ptr, _memmodel) atomic_add(_ptr, 1, _memmodel)
#endif

#ifndef atomic_inc_return
#define atomic_inc_return(_ptr, _memmodel) \
            atomic_add_return(_ptr, 1, _memmodel)
#endif

#ifndef atomic_dec
#define atomic_dec(_ptr, _memmodel) atomic_sub(_ptr, 1, _memmodel)
#endif

#ifndef atomic_dec_return
#define atomic_dec_return(_ptr, _memmodel) \
            atomic_sub_return(_ptr, 1, _memmodel)
#endif

#ifndef atomic_add_unless
#define atomic_add_unless(_ptr, _add, _chk, _memmodel_ok, _memmodel_fail) \
    ({ \
        typeof(_ptr) __tmp_ptr = (_ptr); \
        typeof(*(_ptr)) __tmp_val = *__tmp_ptr; \
        typeof(*(_ptr)) __tmp_add = (_add); \
        typeof(*(_ptr)) __tmp_chk = (_chk); \
        if (likely(__tmp_val != __tmp_chk)) \
        { \
            while (unlikely(!atomic_cmpxchg(__tmp_ptr, __tmp_val, __tmp_val + \
                                            __tmp_add, _memmodel_ok, \
                                            _memmodel_fail))) \
            { \
                __tmp_val = *__tmp_ptr; \
                if (unlikely(__tmp_val == __tmp_chk)) \
                { \
                    break; \
                } \
            } \
        } \
        __tmp_val; \
    })
#endif

#ifndef atomic_add_unless_return
#define atomic_add_unless_return(_ptr, _add, _chk, _memmodel_ok, \
                                 _memmodel_fail) \
    ({ \
        typeof(*(_ptr)) __tmp_add = (_add); \
        atomic_add_unless(_ptr, __tmp_add, _chk, _memmodel_ok, \
                          _memmodel_fail) + __tmp_add; \
    )}
#endif

#ifndef atomic_inc_unless
#define atomic_inc_unless(_ptr, _chk, _memmodel_ok, _memmodel_fail) \
            atomic_add_unless(_ptr, 1, _chk, _memmodel_ok, _memmodel_fail)
#endif

#ifndef atomic_inc_not_zero
#define atomic_inc_not_zero(_ptr, _memmodel_ok, _memmodel_fail) \
            atomic_inc_unless(_ptr, 0, _memmodel_ok, _memmodel_fail)
#endif

#ifndef atomic_sub_unless
#define atomic_sub_unless(_ptr, _sub, _chk, _memmodel_ok, _memmodel_fail) \
    ({ \
        typeof(_ptr) __tmp_ptr = (_ptr); \
        typeof(*(_ptr)) __tmp_val = *__tmp_ptr; \
        typeof(*(_ptr)) __tmp_sub = (_sub); \
        typeof(*(_ptr)) __tmp_chk = (_chk); \
        if (likely(__tmp_val != __tmp_chk)) \
        { \
            while (unlikely(!atomic_cmpxchg_return(__tmp_ptr, __tmp_val, \
                                                   __tmp_val - __tmp_sub))) \
            { \
                __tmp_val = *__tmp_ptr; \
                if (unlikely(__tmp_val == __tmp_chk)) \
                { \
                    break; \
                } \
            } \
        } \
        __tmp_val; \
    })
#endif

#ifndef atomic_sub_unless_return
#define atomic_sub_unless_return(_ptr, _sub, _chk, _memmodel_ok, \
                                 _memmodel_fail) \
    ({ \
        typeof(*(_ptr)) __tmp_sub = (_sub); \
        atomic_sub_unless(_ptr, __tmp_sub, _chk, _memmodel_ok, \
                          _memmodel_fail) - __tmp_sub; \
    )}
#endif

#ifndef atomic_dec_unless
#define atomic_dec_unless(_ptr, _chk, _memmodel_ok, _memmodel_fail) \
            atomic_sub_unless(_ptr, 1, _chk, _memmodel_ok, _memmodel_fail)
#endif

#ifndef atomic_dec_not_zero
#define atomic_dec_not_zero(_ptr, _memmodel_ok, _memmodel_fail) \
            atomic_dec_unless(_ptr, 0, _memmodel_ok, _memmodel_fail)
#endif

#ifndef atomic_bit_set
#define atomic_bit_set(_ptr, _bit, _memmodel) \
    ({ \
        typeof(*(_ptr)) __tmp_bit = 1ULL << (_bit); \
        __tmp_bit & atomic_or(_ptr, __tmp_bit, _memmodel); \
    })
#endif

#ifndef atomic_bit_clear
#define atomic_bit_clear(_ptr, _bit, _memmodel) \
    ({ \
        typeof(*(_ptr)) __tmp_bit = 1ULL << (_bit); \
        __tmp_bit & atomic_and(_ptr, ~__tmp_bit, _memmodel); \
    })
#endif

#ifndef atomic_bit_change
#define atomic_bit_change(_ptr, _bit, _memmodel) \
    ({ \
        typeof(*(_ptr)) __tmp_bit = 1ULL << (_bit); \
        __tmp_bit & atomic_xor(_ptr, __tmp_bit, _memmodel); \
    })
#endif

#ifndef atomic_bit_next
#define atomic_bit_next(_ptr, _memmodel_ok, _memmodel_fail) \
    ({ \
        typeof(_ptr) __tmp_ptr = (_ptr); \
        typeof(*(_ptr)) __tmp_val = *__tmp_ptr; \
        while (__tmp_val != 0) \
        { \
            if (atomic_cmpxchg(__tmp_ptr, __tmp_val, \
                               __tmp_val & (__tmp_val - 1), \
                               _memmodel_ok, _memmodel_fail)); \
            { \
                __tmp_val &= -__tmp_val; \
                break; \
            } \
            __tmp_val = *__tmp_ptr; \
        } \
        __tmp_val; \
    })
#endif

#endif /* __SYS_ATOMIC_H__ */
