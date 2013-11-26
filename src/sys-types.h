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

#ifndef __SYS_TYPES_H__
#define __SYS_TYPES_H__

#define SYS_TYPE_FIELD(_size, _type, _name) \
    _type _name[(_size) / sizeof(_type)];

#define SYS_TYPE_INT(_size, _bits, _signed, _unsigned) \
    SYS_TYPE_FIELD(_size, _signed, s##_bits) \
    SYS_TYPE_FIELD(_size, _unsigned, u##_bits)

#ifdef __SIZEOF_INT128__

#define SYS_TYPE_INT128(_size) \
    SYS_TYPE_INT(_size, 128, __int128, unsigned __int128)

#else

#define SYS_TYPE_INT128(_size)

#endif

#define SYS_TYPE_INT256

#define SYS_TYPE_INT64(_size) SYS_TYPE_INT(_size, 64, int64_t, uint64_t)
#define SYS_TYPE_INT32(_size) SYS_TYPE_INT(_size, 32, int32_t, uint32_t)
#define SYS_TYPE_INT16(_size) SYS_TYPE_INT(_size, 16, int16_t, uint16_t)
#define SYS_TYPE_INT8(_size)  SYS_TYPE_INT(_size,  8, int8_t,  uint8_t)

#define SYS_TYPE_INTPTR(_size) \
    SYS_TYPE_FIELD(_size, intptr_t, sptr) \
    SYS_TYPE_FIELD(_size, uintptr_t, uptr)

#define __SYS_TYPE_ANY_DEFINE(_name, _size)           \
    typedef union _##_name                            \
    {                                                 \
        SYS_TYPE_INT128(_size)                        \
        SYS_TYPE_INT64(_size)                         \
        SYS_TYPE_INT32(_size)                         \
        SYS_TYPE_INT16(_size)                         \
        SYS_TYPE_INT8(_size)                          \
        SYS_TYPE_INTPTR(_size)                        \
        SYS_TYPE_FIELD(_size, double,           d)    \
        SYS_TYPE_FIELD(_size, float,            f)    \
        SYS_TYPE_FIELD(_size, void *,           ptr)  \
        SYS_TYPE_FIELD(_size, struct list_head, list) \
    } __align(_size) _name##_t

#define SYS_TYPE_ANY_DEFINE(_bits) \
    __SYS_TYPE_ANY_DEFINE(any##_bits, (_bits) / 8)

#pragma pack(push, 1)

SYS_TYPE_ANY_DEFINE(8);
SYS_TYPE_ANY_DEFINE(16);
SYS_TYPE_ANY_DEFINE(32);
SYS_TYPE_ANY_DEFINE(64);
SYS_TYPE_ANY_DEFINE(128);
SYS_TYPE_ANY_DEFINE(256);
SYS_TYPE_ANY_DEFINE(512);
SYS_TYPE_ANY_DEFINE(1024);
SYS_TYPE_ANY_DEFINE(2048);
SYS_TYPE_ANY_DEFINE(4096);

__SYS_TYPE_ANY_DEFINE(anyptr, sizeof(intptr_t));
__SYS_TYPE_ANY_DEFINE(anycl, CACHE_LINE_SIZE);

#pragma pack(pop)

#endif /* __SYS_TYPES_H__ */
