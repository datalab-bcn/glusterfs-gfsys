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

#ifndef __SYS_BUFFER_H__
#define __SYS_BUFFER_H__

static inline int8_t __sys_buf_get_int8(void ** ptr)
{
    int8_t value = *(int8_t *)*ptr;
    (*ptr) += sizeof(value);
    return value;
}

static inline uint8_t __sys_buf_get_uint8(void ** ptr)
{
    uint8_t value = *(uint8_t *)*ptr;
    (*ptr) += sizeof(value);
    return value;
}

static inline int16_t __sys_buf_get_int16(void ** ptr)
{
    int16_t value = *(int16_t *)*ptr;
    (*ptr) += sizeof(value);
    return ntoh16(value);
}

static inline uint16_t __sys_buf_get_uint16(void ** ptr)
{
    uint16_t value = *(uint16_t *)*ptr;
    (*ptr) += sizeof(value);
    return ntoh16(value);
}

static inline int32_t __sys_buf_get_int32(void ** ptr)
{
    int32_t value = *(int32_t *)*ptr;
    (*ptr) += sizeof(value);
    return ntoh32(value);
}

static inline uint32_t __sys_buf_get_uint32(void ** ptr)
{
    uint32_t value = *(uint32_t *)*ptr;
    (*ptr) += sizeof(value);
    return ntoh32(value);
}

static inline int64_t __sys_buf_get_int64(void ** ptr)
{
    int64_t value = *(int64_t *)*ptr;
    (*ptr) += sizeof(value);
    return ntoh64(value);
}

static inline uint64_t __sys_buf_get_uint64(void ** ptr)
{
    uint64_t value = *(uint64_t *)*ptr;
    (*ptr) += sizeof(value);
    return ntoh64(value);
}

static inline void * __sys_buf_ptr_raw(void ** ptr, size_t length)
{
    void * tmp = *ptr;
    (*ptr) += length;
    return tmp;
}

static inline void __sys_buf_get_raw(void ** ptr, void * raw, size_t length)
{
    memcpy(raw, *ptr, length);
    (*ptr) += length;
}

static inline uuid_t * __sys_buf_ptr_uuid(void ** ptr)
{
    return __sys_buf_ptr_raw(ptr, sizeof(uuid_t));
}

static inline void __sys_buf_get_uuid(void ** ptr, uuid_t uuid)
{
    __sys_buf_get_raw(ptr, uuid, sizeof(uuid));
}

static inline void * __sys_buf_ptr_block(void ** ptr, uint32_t * length)
{
    *length = __sys_buf_get_uint32(ptr);
    return __sys_buf_ptr_raw(ptr, *length);
}

static inline uint32_t __sys_buf_get_block(void ** ptr, void * block)
{
    uint32_t length = __sys_buf_get_uint32(ptr);
    __sys_buf_get_raw(ptr, block,length);
    return length;
}

static inline char * __sys_buf_ptr_str(void ** ptr, uint32_t * length)
{
    *length = __sys_buf_get_uint32(ptr) - 1;
    return __sys_buf_ptr_raw(ptr, *length + 1);
}

static inline uint32_t __sys_buf_get_str(void ** ptr, char * str)
{
    return __sys_buf_get_block(ptr, str) - 1;
}

static inline void __sys_buf_set_int8(void ** ptr, int8_t data)
{
    *(int8_t *)*ptr = data;
    (*ptr) += sizeof(data);
}

static inline void __sys_buf_set_uint8(void ** ptr, uint8_t data)
{
    *(uint8_t *)*ptr = data;
    (*ptr) += sizeof(data);
}

static inline void __sys_buf_set_int16(void ** ptr, int16_t data)
{
    *(int16_t *)*ptr = hton16(data);
    (*ptr) += sizeof(data);
}

static inline void __sys_buf_set_uint16(void ** ptr, uint16_t data)
{
    *(uint16_t *)*ptr = hton16(data);
    (*ptr) += sizeof(data);
}

static inline void __sys_buf_set_int32(void ** ptr, int32_t data)
{
    *(int32_t *)*ptr = hton32(data);
    (*ptr) += sizeof(data);
}

static inline void __sys_buf_set_uint32(void ** ptr, uint32_t data)
{
    *(uint32_t *)*ptr = hton32(data);
    (*ptr) += sizeof(data);
}

static inline void __sys_buf_set_int64(void ** ptr, int64_t data)
{
    *(int64_t *)*ptr = hton64(data);
    (*ptr) += sizeof(data);
}

static inline void __sys_buf_set_uint64(void ** ptr, uint64_t data)
{
    *(uint64_t *)*ptr = hton64(data);
    (*ptr) += sizeof(data);
}

static inline void __sys_buf_set_raw(void ** ptr, void * raw, size_t length)
{
    *ptr = mempcpy(*ptr, raw, length);
}

static inline void __sys_buf_set_uuid(void ** ptr, uuid_t uuid)
{
    __sys_buf_set_raw(ptr, uuid, sizeof(uuid));
}

static inline void __sys_buf_set_block(void ** ptr, void * block,
                                       uint32_t length)
{
    __sys_buf_set_uint32(ptr, length);
    __sys_buf_set_raw(ptr, block, length);
}

static inline void __sys_buf_set_str(void ** ptr, char * str)
{
    __sys_buf_set_block(ptr, str, strlen(str) + 1);
}

static inline err_t sys_buf_get_int8(void ** ptr, size_t * size,
                                     int8_t * data)
{
    SYS_TEST(*size >= sizeof(int8_t), ENODATA, T(), RETERR());
    (*size) -= sizeof(int8_t);
    *data = __sys_buf_get_int8(ptr);
    return 0;
}

static inline err_t sys_buf_get_uint8(void ** ptr, size_t * size,
                                      uint8_t * data)
{
    SYS_TEST(*size >= sizeof(uint8_t), ENODATA, T(), RETERR());
    (*size) -= sizeof(uint8_t);
    *data = __sys_buf_get_uint8(ptr);
    return 0;
}

static inline err_t sys_buf_get_int16(void ** ptr, size_t * size,
                                      int16_t * data)
{
    SYS_TEST(*size >= sizeof(int16_t), ENODATA, T(), RETERR());
    (*size) -= sizeof(int16_t);
    *data = __sys_buf_get_int16(ptr);
    return 0;
}

static inline err_t sys_buf_get_uint16(void ** ptr, size_t * size,
                                       uint16_t * data)
{
    SYS_TEST(*size >= sizeof(uint16_t), ENODATA, T(), RETERR());
    (*size) -= sizeof(uint16_t);
    *data = __sys_buf_get_uint16(ptr);
    return 0;
}

static inline err_t sys_buf_get_int32(void ** ptr, size_t * size,
                                      int32_t * data)
{
    SYS_TEST(*size >= sizeof(int32_t), ENODATA, T(), RETERR());
    (*size) -= sizeof(int32_t);
    *data = __sys_buf_get_int32(ptr);
    return 0;
}

static inline err_t sys_buf_get_uint32(void ** ptr, size_t * size,
                                       uint32_t * data)
{
    SYS_TEST(*size >= sizeof(uint32_t), ENODATA, T(), RETERR());
    (*size) -= sizeof(uint32_t);
    *data = __sys_buf_get_uint32(ptr);
    return 0;
}

static inline err_t sys_buf_get_int64(void ** ptr, size_t * size,
                                      int64_t * data)
{
    SYS_TEST(*size >= sizeof(int64_t), ENODATA, T(), RETERR());
    (*size) -= sizeof(int64_t);
    *data = __sys_buf_get_int64(ptr);
    return 0;
}

static inline err_t sys_buf_get_uint64(void ** ptr, size_t * size,
                                       uint64_t * data)
{
    SYS_TEST(*size >= sizeof(uint64_t), ENODATA, T(), RETERR());
    (*size) -= sizeof(uint64_t);
    *data = __sys_buf_get_uint64(ptr);
    return 0;
}

static inline err_t sys_buf_ptr_raw(void ** ptr, size_t * size,
                                    void ** raw, size_t length)
{
    SYS_TEST(*size >= length, ENODATA, T(), RETERR());
    (*size) -= length;
    *raw = __sys_buf_ptr_raw(ptr, length);
    return 0;
}

static inline err_t sys_buf_get_raw(void ** ptr, size_t * size,
                                    void * raw, size_t length)
{
    SYS_TEST(*size >= length, ENODATA, T(), RETERR());
    (*size) -= length;
    __sys_buf_get_raw(ptr, raw, length);
    return 0;
}

static inline err_t sys_buf_ptr_uuid(void ** ptr, size_t * size,
                                     uuid_t ** uuid)
{
    return sys_buf_ptr_raw(ptr, size, (void **)uuid, sizeof(uuid));
}

static inline err_t sys_buf_get_uuid(void ** ptr, size_t * size,
                                     uuid_t uuid)
{
    return sys_buf_get_raw(ptr, size, uuid, sizeof(uuid));
}

static inline err_t sys_buf_ptr_block(void ** ptr, size_t * size,
                                      void ** block, uint32_t * length)
{
    SYS_CALL(sys_buf_get_uint32, (ptr, size, length), T(), RETERR());
    return sys_buf_ptr_raw(ptr, size, block, *length);
}

static inline err_t sys_buf_get_block(void ** ptr, size_t * size,
                                      void * block, uint32_t max,
                                      uint32_t * length)
{
    SYS_CALL(sys_buf_get_uint32, (ptr, size, length), T(), RETERR());
    SYS_TEST(max >= *length, ENOBUFS, T(), RETERR());
    return sys_buf_get_raw(ptr, size, block, *length);
}

static inline err_t sys_buf_ptr_str(void ** ptr, size_t * size,
                                    char ** str, uint32_t * length)
{
    return sys_buf_ptr_block(ptr, size, (void **)str, length);
}

static inline err_t sys_buf_get_str(void ** ptr, size_t * size,
                                    char * str, uint32_t max,
                                    uint32_t * length)
{
    SYS_CALL(sys_buf_get_block, (ptr, size, str, max, length), T(), RETERR());
    SYS_TEST(*length > 0, EINVAL, T(), RETERR());
    (*length)--;
    return 0;
}

static inline err_t sys_buf_set_int8(void ** ptr, size_t * size,
                                     int8_t data)
{
    SYS_TEST(*size >= sizeof(int8_t), ENOBUFS, T(), RETERR());
    (*size) -= sizeof(int8_t);
    __sys_buf_set_int8(ptr, data);
    return 0;
}

static inline err_t sys_buf_set_uint8(void ** ptr, size_t * size,
                                      uint8_t data)
{
    SYS_TEST(*size >= sizeof(uint8_t), ENOBUFS, T(), RETERR());
    (*size) -= sizeof(uint8_t);
    __sys_buf_set_uint8(ptr, data);
    return 0;
}

static inline err_t sys_buf_set_int16(void ** ptr, size_t * size,
                                      int16_t data)
{
    SYS_TEST(*size >= sizeof(int16_t), ENOBUFS, T(), RETERR());
    (*size) -= sizeof(int16_t);
    __sys_buf_set_int16(ptr, data);
    return 0;
}

static inline err_t sys_buf_set_uint16(void ** ptr, size_t * size,
                                       uint16_t data)
{
    SYS_TEST(*size >= sizeof(uint16_t), ENOBUFS, T(), RETERR());
    (*size) -= sizeof(uint16_t);
    __sys_buf_set_uint16(ptr, data);
    return 0;
}

static inline err_t sys_buf_set_int32(void ** ptr, size_t * size,
                                      int32_t data)
{
    SYS_TEST(*size >= sizeof(int32_t), ENOBUFS, T(), RETERR());
    (*size) -= sizeof(int32_t);
    __sys_buf_set_int32(ptr, data);
    return 0;
}

static inline err_t sys_buf_set_uint32(void ** ptr, size_t * size,
                                       uint32_t data)
{
    SYS_TEST(*size >= sizeof(uint32_t), ENOBUFS, T(), RETERR());
    (*size) -= sizeof(uint32_t);
    __sys_buf_set_uint32(ptr, data);
    return 0;
}

static inline err_t sys_buf_set_int64(void ** ptr, size_t * size,
                                      int64_t data)
{
    SYS_TEST(*size >= sizeof(int64_t), ENOBUFS, T(), RETERR());
    (*size) -= sizeof(int64_t);
    __sys_buf_set_int64(ptr, data);
    return 0;
}

static inline err_t sys_buf_set_uint64(void ** ptr, size_t * size,
                                       uint64_t data)
{
    SYS_TEST(*size >= sizeof(uint64_t), ENOBUFS, T(), RETERR());
    (*size) -= sizeof(uint64_t);
    __sys_buf_set_uint64(ptr, data);
    return 0;
}

static inline err_t sys_buf_set_raw(void ** ptr, size_t * size,
                                    void * raw, size_t length)
{
    SYS_TEST(*size >= length, ENOBUFS, T(), RETERR());
    (*size) -= length;
    __sys_buf_set_raw(ptr, raw, length);
    return 0;
}

static inline err_t sys_buf_set_uuid(void ** ptr, size_t * size,
                                     uuid_t uuid)
{
    return sys_buf_set_raw(ptr, size, uuid, sizeof(uuid));
}

static inline err_t sys_buf_set_block(void ** ptr, size_t * size,
                                      void * block, uint32_t length)
{
    SYS_TEST(*size >= length + sizeof(uint32_t), ENODATA, T(), RETERR());
    (*size) -= length + sizeof(uint32_t);
    __sys_buf_set_block(ptr, block, length);
    return 0;
}

static inline err_t sys_buf_set_str(void ** ptr, size_t * size,
                                    char * str)
{
    return sys_buf_set_block(ptr, size, str, strlen(str) + 1);
}

static inline err_t sys_buf_check(size_t * size, size_t length)
{
    SYS_TEST(*size >= length, ENOBUFS, T(), RETERR());
    (*size) -= length;
    return 0;
}

err_t sys_buf_read(void ** ptr, size_t * size, const char * fmt, ...);
err_t sys_buf_write(void ** ptr, size_t * size, const char * fmt, ...);

#endif /* __SYS_BUFFER_H__ */
