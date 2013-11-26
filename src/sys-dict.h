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

#ifndef __SYS_DICT_H__
#define __SYS_DICT_H__

static inline void sys_dict_acquire(dict_t ** dst, dict_t * src)
{
    if (src != NULL)
    {
        src = dict_ref(src);
    }
    *dst = src;
}

static inline void sys_dict_release(dict_t * src)
{
    if (src != NULL)
    {
        dict_unref(src);
    }
}

err_t sys_dict_add(dict_t ** dst, char * key, data_t * value, data_t ** old);
err_t sys_dict_set(dict_t ** dst, char * key, data_t * value, data_t ** old);
err_t sys_dict_get(dict_t * src, char * key, data_t ** value);
err_t sys_dict_del(dict_t ** dst, char * key, data_t ** value);

err_t sys_dict_to_bin(data_t * data, void * value, size_t * length);
err_t sys_dict_to_block(data_t * data, void * value, size_t length);
err_t sys_dict_to_str(data_t * data, char * value, size_t * length);
err_t sys_dict_to_uuid(data_t * data, uuid_t value);
err_t sys_dict_to_int8(data_t * data, int8_t * value);
err_t sys_dict_to_int16(data_t * data, int16_t * value);
err_t sys_dict_to_int32(data_t * data, int32_t * value);
err_t sys_dict_to_int64(data_t * data, int64_t * value);
err_t sys_dict_to_uint8(data_t * data, uint8_t * value);
err_t sys_dict_to_uint16(data_t * data, uint16_t * value);
err_t sys_dict_to_uint32(data_t * data, uint32_t * value);
err_t sys_dict_to_uint64(data_t * data, uint64_t * value);

err_t sys_dict_add_bin(dict_t ** dst, char * key, void * value, size_t length,
                       data_t ** old);
err_t sys_dict_add_block(dict_t ** dst, char * key, void * value,
                         size_t length, data_t ** old);
err_t sys_dict_add_str(dict_t ** dst, char * key, char * value, data_t ** old);
err_t sys_dict_add_uuid(dict_t ** dst, char * key, uuid_t value,
                        data_t ** old);
err_t sys_dict_add_int8(dict_t ** dst, char * key, int8_t value,
                        data_t ** old);
err_t sys_dict_add_int16(dict_t ** dst, char * key, int16_t value,
                         data_t ** old);
err_t sys_dict_add_int32(dict_t ** dst, char * key, int32_t value,
                         data_t ** old);
err_t sys_dict_add_int64(dict_t ** dst, char * key, int64_t value,
                         data_t ** old);
err_t sys_dict_add_uint8(dict_t ** dst, char * key, uint8_t value,
                         data_t ** old);
err_t sys_dict_add_uint16(dict_t ** dst, char * key, uint16_t value,
                          data_t ** old);
err_t sys_dict_add_uint32(dict_t ** dst, char * key, uint32_t value,
                          data_t ** old);
err_t sys_dict_add_uint64(dict_t ** dst, char * key, uint64_t value,
                          data_t ** old);

err_t sys_dict_set_bin(dict_t ** dst, char * key, void * value, size_t length,
                       data_t ** old);
err_t sys_dict_set_block(dict_t ** dst, char * key, void * value,
                         size_t length, data_t ** old);
err_t sys_dict_set_str(dict_t ** dst, char * key, char * value, data_t ** old);
err_t sys_dict_set_uuid(dict_t ** dst, char * key, uuid_t value,
                        data_t ** old);
err_t sys_dict_set_int8(dict_t ** dst, char * key, int8_t value,
                        data_t ** old);
err_t sys_dict_set_int16(dict_t ** dst, char * key, int16_t value,
                         data_t ** old);
err_t sys_dict_set_int32(dict_t ** dst, char * key, int32_t value,
                         data_t ** old);
err_t sys_dict_set_int64(dict_t ** dst, char * key, int64_t value,
                         data_t ** old);
err_t sys_dict_set_uint8(dict_t ** dst, char * key, uint8_t value,
                         data_t ** old);
err_t sys_dict_set_uint16(dict_t ** dst, char * key, uint16_t value,
                          data_t ** old);
err_t sys_dict_set_uint32(dict_t ** dst, char * key, uint32_t value,
                          data_t ** old);
err_t sys_dict_set_uint64(dict_t ** dst, char * key, uint64_t value,
                          data_t ** old);

err_t sys_dict_get_bin(dict_t * dst, char * key, void * value,
                       size_t * length);
err_t sys_dict_get_block(dict_t * dst, char * key, void * value,
                         size_t length);
err_t sys_dict_get_str(dict_t * dst, char * key, char * value,
                       size_t * length);
err_t sys_dict_get_uuid(dict_t * dst, char * key, uuid_t value);
err_t sys_dict_get_int8(dict_t * dst, char * key, int8_t * value);
err_t sys_dict_get_int16(dict_t * dst, char * key, int16_t * value);
err_t sys_dict_get_int32(dict_t * dst, char * key, int32_t * value);
err_t sys_dict_get_int64(dict_t * dst, char * key, int64_t * value);
err_t sys_dict_get_uint8(dict_t * dst, char * key, uint8_t * value);
err_t sys_dict_get_uint16(dict_t * dst, char * key, uint16_t * value);
err_t sys_dict_get_uint32(dict_t * dst, char * key, uint32_t * value);
err_t sys_dict_get_uint64(dict_t * dst, char * key, uint64_t * value);

err_t sys_dict_del_bin(dict_t ** dst, char * key, void * value,
                       size_t * length);
err_t sys_dict_del_block(dict_t ** dst, char * key, void * value,
                         size_t length);
err_t sys_dict_del_str(dict_t ** dst, char * key, char * value,
                       size_t * length);
err_t sys_dict_del_uuid(dict_t ** dst, char * key, uuid_t value);
err_t sys_dict_del_int8(dict_t ** dst, char * key, int8_t * value);
err_t sys_dict_del_int16(dict_t ** dst, char * key, int16_t * value);
err_t sys_dict_del_int32(dict_t ** dst, char * key, int32_t * value);
err_t sys_dict_del_int64(dict_t ** dst, char * key, int64_t * value);
err_t sys_dict_del_uint8(dict_t ** dst, char * key, uint8_t * value);
err_t sys_dict_del_uint16(dict_t ** dst, char * key, uint16_t * value);
err_t sys_dict_del_uint32(dict_t ** dst, char * key, uint32_t * value);
err_t sys_dict_del_uint64(dict_t ** dst, char * key, uint64_t * value);

#endif /* __SYS_DICT_H__ */
