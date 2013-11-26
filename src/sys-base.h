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

#ifndef __SYS_BASE_H__
#define __SYS_BASE_H__

#include <inttypes.h>
#include <stdbool.h>

#include <globals.h>

#include "sys-compiler.h"

#define offset_of(_type, _field) ((uintptr_t)(&((_type *)0)->_field))
#define container_of(_ptr, _type, _field) ((_type *)(((uint8_t *)(_ptr)) - offset_of(_type, _field)))
#define count_of(_array) (sizeof(_array) / sizeof((_array)[0]))

#define __SYS_GLUE(_a, _b) _a##_b
#define _SYS_GLUE(_a, _b) __SYS_GLUE(_a, _b)
#define SYS_GLUE_1(_value, _args...) _SYS_GLUE(_value, _args)
#define SYS_GLUE_2(_value, _args...) _SYS_GLUE(_value, SYS_GLUE_1(_args))
#define SYS_GLUE_3(_value, _args...) _SYS_GLUE(_value, SYS_GLUE_2(_args))
#define SYS_GLUE_4(_value, _args...) _SYS_GLUE(_value, SYS_GLUE_3(_args))
#define SYS_GLUE_5(_value, _args...) _SYS_GLUE(_value, SYS_GLUE_4(_args))
#define SYS_GLUE_6(_value, _args...) _SYS_GLUE(_value, SYS_GLUE_5(_args))
#define SYS_GLUE_7(_value, _args...) _SYS_GLUE(_value, SYS_GLUE_6(_args))
#define SYS_GLUE_8(_value, _args...) _SYS_GLUE(_value, SYS_GLUE_7(_args))
#define SYS_GLUE(_args...) SYS_GLUE_8(_args)

#define __SYS_TMP_1() SYS_GLUE(__sys_tmp_, __COUNTER__)
#define __SYS_TMP_2() __SYS_TMP_1(), __SYS_TMP_1()
#define __SYS_TMP_3() __SYS_TMP_2(), __SYS_TMP_1()
#define __SYS_TMP_4() __SYS_TMP_2(), __SYS_TMP_2()
#define __SYS_TMP_5() __SYS_TMP_3(), __SYS_TMP_2()
#define __SYS_TMP_6() __SYS_TMP_3(), __SYS_TMP_3()
#define __SYS_TMP_7() __SYS_TMP_4(), __SYS_TMP_3()
#define __SYS_TMP_8() __SYS_TMP_4(), __SYS_TMP_4()

#define SYS_TMP(_count) __SYS_TMP_##_count()

#define _SYS_MARG(_arg...) , ## _arg
#define SYS_MARG(_arg...) _SYS_MARG(_arg)

#define _SYS_FIRST(_value, _args...) _value
#define SYS_FIRST(_args...) _SYS_FIRST(_args)

#define SYS_COUNT_0(_value, _args...) _value
#define SYS_COUNT_1(_value, _args...) SYS_COUNT_0(_args, 0)
#define SYS_COUNT_2(_value, _args...) SYS_COUNT_1(_args, 1)
#define SYS_COUNT_3(_value, _args...) SYS_COUNT_2(_args, 2)
#define SYS_COUNT_4(_value, _args...) SYS_COUNT_3(_args, 3)
#define SYS_COUNT_5(_value, _args...) SYS_COUNT_4(_args, 4)
#define SYS_COUNT_6(_value, _args...) SYS_COUNT_5(_args, 5)
#define SYS_COUNT_7(_value, _args...) SYS_COUNT_6(_args, 6)
#define SYS_COUNT_8(_value, _args...) SYS_COUNT_7(_args, 7)
#define SYS_COUNT_9(_value, _args...) SYS_COUNT_8(_args, 8)
#define SYS_COUNT_10(_value, _args...) SYS_COUNT_9(_args, 9)
#define SYS_COUNT_11(_value, _args...) SYS_COUNT_10(_args, 10)
#define SYS_COUNT_12(_value, _args...) SYS_COUNT_11(_args, 11)
#define SYS_COUNT_13(_value, _args...) SYS_COUNT_12(_args, 12)
#define SYS_COUNT_14(_value, _args...) SYS_COUNT_13(_args, 13)
#define SYS_COUNT_15(_value, _args...) SYS_COUNT_14(_args, 14)
#define SYS_COUNT_16(_value, _args...) SYS_COUNT_15(_args, 15)

#define SYS_COUNT(_args...) SYS_COUNT_16(_args, 16)

#define SYS_KEEP_0(_args...)
#define SYS_KEEP_1(_value, _args...) _value
#define SYS_KEEP_2(_value, _args...) _value SYS_MARG(SYS_KEEP_1(_args))
#define SYS_KEEP_3(_value, _args...) _value SYS_MARG(SYS_KEEP_2(_args))
#define SYS_KEEP_4(_value, _args...) _value SYS_MARG(SYS_KEEP_3(_args))
#define SYS_KEEP_5(_value, _args...) _value SYS_MARG(SYS_KEEP_4(_args))
#define SYS_KEEP_6(_value, _args...) _value SYS_MARG(SYS_KEEP_5(_args))
#define SYS_KEEP_7(_value, _args...) _value SYS_MARG(SYS_KEEP_6(_args))
#define SYS_KEEP_8(_value, _args...) _value SYS_MARG(SYS_KEEP_7(_args))
#define SYS_KEEP_9(_value, _args...) _value SYS_MARG(SYS_KEEP_8(_args))
#define SYS_KEEP_10(_value, _args...) _value SYS_MARG(SYS_KEEP_9(_args))
#define SYS_KEEP_11(_value, _args...) _value SYS_MARG(SYS_KEEP_10(_args))
#define SYS_KEEP_12(_value, _args...) _value SYS_MARG(SYS_KEEP_11(_args))
#define SYS_KEEP_13(_value, _args...) _value SYS_MARG(SYS_KEEP_12(_args))
#define SYS_KEEP_14(_value, _args...) _value SYS_MARG(SYS_KEEP_13(_args))
#define SYS_KEEP_15(_value, _args...) _value SYS_MARG(SYS_KEEP_14(_args))
#define SYS_KEEP_16(_value, _args...) _value SYS_MARG(SYS_KEEP_15(_args))

#define SYS_KEEP(_count, _args...) SYS_KEEP_##_count(_args)

#define SYS_SKIP_0(_args...) _args
#define SYS_SKIP_1(_value, _args...) _args
#define SYS_SKIP_2(_value, _args...) SYS_SKIP_1(_args)
#define SYS_SKIP_3(_value, _args...) SYS_SKIP_2(_args)
#define SYS_SKIP_4(_value, _args...) SYS_SKIP_3(_args)
#define SYS_SKIP_5(_value, _args...) SYS_SKIP_4(_args)
#define SYS_SKIP_6(_value, _args...) SYS_SKIP_5(_args)
#define SYS_SKIP_7(_value, _args...) SYS_SKIP_6(_args)
#define SYS_SKIP_8(_value, _args...) SYS_SKIP_7(_args)
#define SYS_SKIP_9(_value, _args...) SYS_SKIP_8(_args)
#define SYS_SKIP_10(_value, _args...) SYS_SKIP_9(_args)
#define SYS_SKIP_11(_value, _args...) SYS_SKIP_10(_args)
#define SYS_SKIP_12(_value, _args...) SYS_SKIP_11(_args)
#define SYS_SKIP_13(_value, _args...) SYS_SKIP_12(_args)
#define SYS_SKIP_14(_value, _args...) SYS_SKIP_13(_args)
#define SYS_SKIP_15(_value, _args...) SYS_SKIP_14(_args)
#define SYS_SKIP_16(_value, _args...) SYS_SKIP_15(_args)

#define SYS_SKIP(_count, _args...) SYS_SKIP_##_count(_args)

#define SYS_AT_LEAST_0(_args...) _args
#define SYS_AT_LEAST_1(_value, _args...) _value SYS_MARG(_args)
#define SYS_AT_LEAST_2(_value, _args...) _value, SYS_AT_LEAST_1(_args)
#define SYS_AT_LEAST_3(_value, _args...) _value, SYS_AT_LEAST_2(_args)
#define SYS_AT_LEAST_4(_value, _args...) _value, SYS_AT_LEAST_3(_args)
#define SYS_AT_LEAST_5(_value, _args...) _value, SYS_AT_LEAST_4(_args)
#define SYS_AT_LEAST_6(_value, _args...) _value, SYS_AT_LEAST_5(_args)
#define SYS_AT_LEAST_7(_value, _args...) _value, SYS_AT_LEAST_6(_args)
#define SYS_AT_LEAST_8(_value, _args...) _value, SYS_AT_LEAST_7(_args)
#define SYS_AT_LEAST_9(_value, _args...) _value, SYS_AT_LEAST_8(_args)
#define SYS_AT_LEAST_10(_value, _args...) _value, SYS_AT_LEAST_9(_args)
#define SYS_AT_LEAST_11(_value, _args...) _value, SYS_AT_LEAST_10(_args)
#define SYS_AT_LEAST_12(_value, _args...) _value, SYS_AT_LEAST_11(_args)
#define SYS_AT_LEAST_13(_value, _args...) _value, SYS_AT_LEAST_12(_args)
#define SYS_AT_LEAST_14(_value, _args...) _value, SYS_AT_LEAST_13(_args)
#define SYS_AT_LEAST_15(_value, _args...) _value, SYS_AT_LEAST_14(_args)
#define SYS_AT_LEAST_16(_value, _args...) _value, SYS_AT_LEAST_15(_args)

#define SYS_AT_LEAST(_count, _args...) SYS_AT_LEAST_##_count(_args)

#define SYS_GET_0(_value, _args...) _value
#define SYS_GET_1(_value, _args...) SYS_GET_0(_args)
#define SYS_GET_2(_value, _args...) SYS_GET_1(_args)
#define SYS_GET_3(_value, _args...) SYS_GET_2(_args)
#define SYS_GET_4(_value, _args...) SYS_GET_3(_args)
#define SYS_GET_5(_value, _args...) SYS_GET_4(_args)
#define SYS_GET_6(_value, _args...) SYS_GET_5(_args)
#define SYS_GET_7(_value, _args...) SYS_GET_6(_args)
#define SYS_GET_8(_value, _args...) SYS_GET_7(_args)
#define SYS_GET_9(_value, _args...) SYS_GET_8(_args)
#define SYS_GET_10(_value, _args...) SYS_GET_9(_args)
#define SYS_GET_11(_value, _args...) SYS_GET_10(_args)
#define SYS_GET_12(_value, _args...) SYS_GET_11(_args)
#define SYS_GET_13(_value, _args...) SYS_GET_12(_args)
#define SYS_GET_14(_value, _args...) SYS_GET_13(_args)
#define SYS_GET_15(_value, _args...) SYS_GET_14(_args)

#define SYS_GET(_idx, _args...) SYS_GET_##_idx(_args)

#define __SYS_SELECT(_dummy, _check, _value, _more...) _value
#define _SYS_SELECT(_args...) __SYS_SELECT(_args)
#define SYS_SELECT(_opt1, _opt2, _check...) \
    _SYS_SELECT(DUMMY SYS_MARG(SYS_FIRST(_check)), _opt1, _opt2)

#define SEP_
#define SEP_COMMA ,
#define SEP_PLUS +
#define SEP_SEMIC ;

#define SYS_SEP(_sep, _args...) SYS_SELECT(SEP_##_sep, , _args) _args

#define SYS_JOIN_1(_sep, _value, _args...) \
    _value SYS_SEP(_sep, _args)
#define SYS_JOIN_2(_sep, _value, _args...) \
    _value SYS_SEP(_sep, SYS_JOIN_1(_sep, _args))
#define SYS_JOIN_3(_sep, _value, _args...) \
    _value SYS_SEP(_sep, SYS_JOIN_2(_sep, _args))
#define SYS_JOIN_4(_sep, _value, _args...) \
    _value SYS_SEP(_sep, SYS_JOIN_3(_sep, _args))
#define SYS_JOIN_5(_sep, _value, _args...) \
    _value SYS_SEP(_sep, SYS_JOIN_4(_sep, _args))
#define SYS_JOIN_6(_sep, _value, _args...) \
    _value SYS_SEP(_sep, SYS_JOIN_5(_sep, _args))
#define SYS_JOIN_7(_sep, _value, _args...) \
    _value SYS_SEP(_sep, SYS_JOIN_6(_sep, _args))
#define SYS_JOIN_8(_sep, _value, _args...) \
    _value SYS_SEP(_sep, SYS_JOIN_7(_sep, _args))
#define SYS_JOIN_9(_sep, _value, _args...) \
    _value SYS_SEP(_sep, SYS_JOIN_8(_sep, _args))
#define SYS_JOIN_10(_sep, _value, _args...) \
    _value SYS_SEP(_sep, SYS_JOIN_9(_sep, _args))
#define SYS_JOIN_11(_sep, _value, _args...) \
    _value SYS_SEP(_sep, SYS_JOIN_10(_sep, _args))
#define SYS_JOIN_12(_sep, _value, _args...) \
    _value SYS_SEP(_sep, SYS_JOIN_11(_sep, _args))
#define SYS_JOIN_13(_sep, _value, _args...) \
    _value SYS_SEP(_sep, SYS_JOIN_12(_sep, _args))
#define SYS_JOIN_14(_sep, _value, _args...) \
    _value SYS_SEP(_sep, SYS_JOIN_13(_sep, _args))
#define SYS_JOIN_15(_sep, _value, _args...) \
    _value SYS_SEP(_sep, SYS_JOIN_14(_sep, _args))
#define SYS_JOIN_16(_sep, _value, _args...) \
    _value SYS_SEP(_sep, SYS_JOIN_15(_sep, _args))

#define SYS_JOIN(_sep, _args...) SYS_JOIN_16(_sep, _args)

#define __SYS_EXPAND(_args...) _args
#define _SYS_EXPAND(_args) __SYS_EXPAND _args
#define SYS_EXPAND(_args) _SYS_EXPAND(SYS_SELECT(_args, (), _args))

#define ACCESS_ONCE(_x) (*(volatile typeof(_x) *)&(_x))

#define __SYS_MIN(_t1, _t2, _a, _b) \
    ({ \
        typeof(_a) _t1 = (_a); \
        typeof(_b) _t2 = (_b); \
        (_t1 <= _t2) ? _t1 : _t2; \
    })
#define _SYS_MIN(_args...) __SYS_MIN(_args)
#define SYS_MIN(_a, _b) _SYS_MIN(SYS_TMP(2), _a, _b)

#define __SYS_MAX(_t1, _t2, _a, _b) \
    ({ \
        typeof(_a) _t1 = (_a); \
        typeof(_b) _t2 = (_b); \
        (_t1 >= _t2) ? _t1 : _t2; \
    })
#define _SYS_MAX(_args...) __SYS_MAX(_args)
#define SYS_MAX(_a, _b) _SYS_MAX(SYS_TMP(2), _a, _b)

#endif /* __SYS_BASE_H__ */
