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

err_t sys_dict_cow(dict_t ** dst)
{
    dict_t * tmp = *dst;

    if (tmp == NULL)
    {
        SYS_PTR(
            &tmp, dict_new, (),
            ENOMEM,
            E(),
            RETERR()
        );
        *dst = tmp;
    }
    else if (tmp->refcount > 1)
    {
        SYS_PTR(
            &tmp, dict_copy_with_ref, (tmp, NULL),
            ENOMEM,
            E(),
            RETERR()
        );
        dict_unref(*dst);
        *dst = tmp;
    }

    return 0;
}

err_t __sys_dict_get(dict_t * src, char * key, data_t ** value)
{
    data_t * tmp;
    if (src != NULL)
    {
        tmp = dict_get(src, key);
        if (tmp != NULL)
        {
            *value = tmp;

            return 0;
        }
    }

    return ENOENT;
}

err_t sys_dict_get(dict_t * src, char * key, data_t ** value)
{
    SYS_CALL(
        __sys_dict_get, (src, key, value),
        T(),
        RETERR()
    );

    data_ref(*value);

    return 0;
}

err_t __sys_dict_put(dict_t ** dst, char * key, data_t * value,
                     data_t ** old, bool replace)
{
    data_t * tmp = NULL;

    if (__sys_dict_get(*dst, key, &tmp) == 0)
    {
        if (old != NULL)
        {
            data_ref(tmp);
            *old = tmp;

            if (!replace)
            {
                return EEXIST;
            }
        }
    }
    else if (old != NULL)
    {
        *old = NULL;
    }

    if ((value != NULL) || (tmp != NULL))
    {
        SYS_CALL(
            sys_dict_cow, (dst),
            E(),
            RETERR()
        );

        if (value != NULL)
        {
            SYS_CODE(
                dict_set, (*dst, key, value),
                ENOMEM,
                E(),
                RETERR()
            );
        }
        else
        {
            dict_del(*dst, key);
        }
    }

    return 0;
}

err_t sys_dict_add(dict_t ** dst, char * key, data_t * value,
                   data_t ** old)
{
    return __sys_dict_put(dst, key, value, old, false);
}

err_t sys_dict_set(dict_t ** dst, char * key, data_t * value,
                   data_t ** old)
{
    return __sys_dict_put(dst, key, value, old, true);
}

err_t sys_dict_del(dict_t ** dst, char * key, data_t ** value)
{
    return sys_dict_set(dst, key, NULL, value);
}

void sys_dict_data_release(data_t * data)
{
    data_unref(data);
}

err_t sys_dict_to_bin(data_t * data, void * value, size_t * length)
{
    if (*length < data->len)
    {
        return ENOBUFS;
    }

    *length = data->len;
    memcpy(value, data->data, data->len);

    return 0;
}

err_t sys_dict_to_block(data_t * data, void * value, size_t length)
{
    if (length != data->len)
    {
        return EINVAL;
    }

    memcpy(value, data->data, length);

    return 0;
}

err_t sys_dict_to_str(data_t * data, char * value, size_t * length)
{
    size_t len;

    len = strnlen(data->data, data->len) + 1;
    if (len > data->len)
    {
        return EINVAL;
    }
    if (*length < len)
    {
        return ENOBUFS;
    }
    *length = len;
    memcpy(value, data->data, len);

    return 0;
}

err_t sys_dict_to_uuid(data_t * data, uuid_t value)
{
    return sys_dict_to_block(data, value, sizeof(uuid_t));
}

err_t sys_dict_to_int8(data_t * data, int8_t * value)
{
    if (data->len != sizeof(int8_t))
    {
        return EINVAL;
    }

    *value = *(int8_t *)data->data;

    return 0;
}

err_t sys_dict_to_int16(data_t * data, int16_t * value)
{
    if (data->len != sizeof(int16_t))
    {
        return EINVAL;
    }

    *value = ntoh16(*(int16_t *)data->data);

    return 0;
}

err_t sys_dict_to_int32(data_t * data, int32_t * value)
{
    if (data->len != sizeof(int32_t))
    {
        return EINVAL;
    }

    *value = ntoh32(*(int32_t *)data->data);

    return 0;
}

err_t sys_dict_to_int64(data_t * data, int64_t * value)
{
    if (data->len != sizeof(int64_t))
    {
        return EINVAL;
    }

    *value = ntoh64(*(int64_t *)data->data);

    return 0;
}

err_t sys_dict_to_uint8(data_t * data, uint8_t * value)
{
    if (data->len != sizeof(uint8_t))
    {
        return EINVAL;
    }

    *value = *(uint8_t *)data->data;

    return 0;
}

err_t sys_dict_to_uint16(data_t * data, uint16_t * value)
{
    if (data->len != sizeof(uint16_t))
    {
        return EINVAL;
    }

    *value = ntoh16(*(uint16_t *)data->data);

    return 0;
}

err_t sys_dict_to_uint32(data_t * data, uint32_t * value)
{
    if (data->len != sizeof(uint32_t))
    {
        return EINVAL;
    }

    *value = ntoh32(*(uint32_t *)data->data);

    return 0;
}

err_t sys_dict_to_uint64(data_t * data, uint64_t * value)
{
    if (data->len != sizeof(uint64_t))
    {
        return EINVAL;
    }

    *value = ntoh64(*(uint64_t *)data->data);

    return 0;
}

err_t __sys_dict_put_bin(dict_t ** dst, char * key, void * value,
                         size_t length, data_t ** old, bool replace)
{
    data_t * tmp, * prev;
    err_t error;

    SYS_PTR(
        &tmp, get_new_data, (),
        ENOMEM,
        E(),
        RETERR()
    );

    SYS_CALL(
        __sys_dict_put, (dst, key, tmp, &prev, replace),
        D(),
        GOTO(failed, &error)
    );

    SYS_ALLOC(
        &tmp->data, length, sys_mt_uint8_t,
        E(),
        GOTO(failed_restore, &error)
    );

    memcpy(tmp->data, value, length);
    tmp->len = length;

    if (old != NULL)
    {
        *old = prev;
    }
    else if (prev != NULL)
    {
        data_unref(prev);
    }

    return 0;

failed_restore:
    SYS_CALL(
        sys_dict_set, (dst, key, prev, NULL),
        E(),
        LOG(E(), "Unable to restore original state")
    );
failed:
    data_destroy(tmp);

    return error;
}

err_t sys_dict_add_bin(dict_t ** dst, char * key, void * value,
                       size_t length, data_t ** old)
{
    return __sys_dict_put_bin(dst, key, value, length, old, false);
}

err_t sys_dict_add_block(dict_t ** dst, char * key, void * value,
                         size_t length, data_t ** old)
{
    return __sys_dict_put_bin(dst, key, value, length, old, false);
}

err_t sys_dict_add_str(dict_t ** dst, char * key, char * value,
                       data_t ** old)
{
    return __sys_dict_put_bin(dst, key, value, strlen(value) + 1, old, false);
}

err_t sys_dict_add_uuid(dict_t ** dst, char * key, uuid_t value,
                        data_t ** old)
{
    return __sys_dict_put_bin(dst, key, value, sizeof(uuid_t), old, false);
}

err_t sys_dict_add_int8(dict_t ** dst, char * key, int8_t value,
                        data_t ** old)
{
    return __sys_dict_put_bin(dst, key, &value, sizeof(value), old, false);
}

err_t sys_dict_add_int16(dict_t ** dst, char * key, int16_t value,
                         data_t ** old)
{
    value = hton16(value);
    return __sys_dict_put_bin(dst, key, &value, sizeof(value), old, false);
}

err_t sys_dict_add_int32(dict_t ** dst, char * key, int32_t value,
                         data_t ** old)
{
    value = hton32(value);
    return __sys_dict_put_bin(dst, key, &value, sizeof(value), old, false);
}

err_t sys_dict_add_int64(dict_t ** dst, char * key, int64_t value,
                         data_t ** old)
{
    value = hton64(value);
    return __sys_dict_put_bin(dst, key, &value, sizeof(value), old, false);
}

err_t sys_dict_add_uint8(dict_t ** dst, char * key, uint8_t value,
                         data_t ** old)
{
    return __sys_dict_put_bin(dst, key, &value, sizeof(value), old, false);
}

err_t sys_dict_add_uint16(dict_t ** dst, char * key, uint16_t value,
                          data_t ** old)
{
    value = ntoh16(value);
    return __sys_dict_put_bin(dst, key, &value, sizeof(value), old, false);
}

err_t sys_dict_add_uint32(dict_t ** dst, char * key, uint32_t value,
                          data_t ** old)
{
    value = ntoh32(value);
    return __sys_dict_put_bin(dst, key, &value, sizeof(value), old, false);
}

err_t sys_dict_add_uint64(dict_t ** dst, char * key, uint64_t value,
                          data_t ** old)
{
    value = ntoh64(value);
    return __sys_dict_put_bin(dst, key, &value, sizeof(value), old, false);
}

err_t sys_dict_set_bin(dict_t ** dst, char * key, void * value,
                       size_t length, data_t ** old)
{
    return __sys_dict_put_bin(dst, key, value, length, old, true);
}

err_t sys_dict_set_block(dict_t ** dst, char * key, void * value,
                         size_t length, data_t ** old)
{
    return __sys_dict_put_bin(dst, key, value, length, old, true);
}

err_t sys_dict_set_str(dict_t ** dst, char * key, char * value,
                       data_t ** old)
{
    return __sys_dict_put_bin(dst, key, value, strlen(value) + 1, old, true);
}

err_t sys_dict_set_uuid(dict_t ** dst, char * key, uuid_t value,
                        data_t ** old)
{
    return __sys_dict_put_bin(dst, key, value, sizeof(uuid_t), old, true);
}

err_t sys_dict_set_int8(dict_t ** dst, char * key, int8_t value,
                        data_t ** old)
{
    return __sys_dict_put_bin(dst, key, &value, sizeof(value), old, true);
}

err_t sys_dict_set_int16(dict_t ** dst, char * key, int16_t value,
                         data_t ** old)
{
    value = hton16(value);
    return __sys_dict_put_bin(dst, key, &value, sizeof(value), old, true);
}

err_t sys_dict_set_int32(dict_t ** dst, char * key, int32_t value,
                         data_t ** old)
{
    value = hton32(value);
    return __sys_dict_put_bin(dst, key, &value, sizeof(value), old, true);
}

err_t sys_dict_set_int64(dict_t ** dst, char * key, int64_t value,
                         data_t ** old)
{
    value = hton64(value);
    return __sys_dict_put_bin(dst, key, &value, sizeof(value), old, true);
}

err_t sys_dict_set_uint8(dict_t ** dst, char * key, uint8_t value,
                         data_t ** old)
{
    return __sys_dict_put_bin(dst, key, &value, sizeof(value), old, true);
}

err_t sys_dict_set_uint16(dict_t ** dst, char * key, uint16_t value,
                          data_t ** old)
{
    value = hton16(value);
    return __sys_dict_put_bin(dst, key, &value, sizeof(value), old, true);
}

err_t sys_dict_set_uint32(dict_t ** dst, char * key, uint32_t value,
                          data_t ** old)
{
    value = hton32(value);
    return __sys_dict_put_bin(dst, key, &value, sizeof(value), old, true);
}

err_t sys_dict_set_uint64(dict_t ** dst, char * key, uint64_t value,
                          data_t ** old)
{
    value = hton64(value);
    return __sys_dict_put_bin(dst, key, &value, sizeof(value), old, true);
}

err_t sys_dict_get_bin(dict_t * dst, char * key, void * value,
                       size_t * length)
{
    data_t * tmp;

    SYS_CALL(
        __sys_dict_get, (dst, key, &tmp),
        T(),
        RETERR()
    );

    return sys_dict_to_bin(tmp, value, length);
}

err_t sys_dict_get_block(dict_t * dst, char * key, void * value,
                         size_t length)
{
    data_t * tmp;

    SYS_CALL(
        __sys_dict_get, (dst, key, &tmp),
        T(),
        RETERR()
    );

    return sys_dict_to_block(tmp, value, length);
}

err_t sys_dict_get_str(dict_t * dst, char * key, char * value,
                       size_t * length)
{
    data_t * tmp;

    SYS_CALL(
        __sys_dict_get, (dst, key, &tmp),
        T(),
        RETERR()
    );

    return sys_dict_to_str(tmp, value, length);
}

err_t sys_dict_get_uuid(dict_t * dst, char * key, uuid_t value)
{
    return sys_dict_get_block(dst, key, value, sizeof(uuid_t));
}

err_t sys_dict_get_int8(dict_t * dst, char * key, int8_t * value)
{
    data_t * tmp;

    SYS_CALL(
        __sys_dict_get, (dst, key, &tmp),
        T(),
        RETERR()
    );

    return sys_dict_to_int8(tmp, value);
}

err_t sys_dict_get_int16(dict_t * dst, char * key, int16_t * value)
{
    data_t * tmp;

    SYS_CALL(
        __sys_dict_get, (dst, key, &tmp),
        T(),
        RETERR()
    );

    return sys_dict_to_int16(tmp, value);
}

err_t sys_dict_get_int32(dict_t * dst, char * key, int32_t * value)
{
    data_t * tmp;

    SYS_CALL(
        __sys_dict_get, (dst, key, &tmp),
        T(),
        RETERR()
    );

    return sys_dict_to_int32(tmp, value);
}

err_t sys_dict_get_int64(dict_t * dst, char * key, int64_t * value)
{
    data_t * tmp;

    SYS_CALL(
        __sys_dict_get, (dst, key, &tmp),
        T(),
        RETERR()
    );

    return sys_dict_to_int64(tmp, value);
}

err_t sys_dict_get_uint8(dict_t * dst, char * key, uint8_t * value)
{
    data_t * tmp;

    SYS_CALL(
        __sys_dict_get, (dst, key, &tmp),
        T(),
        RETERR()
    );

    return sys_dict_to_uint8(tmp, value);
}

err_t sys_dict_get_uint16(dict_t * dst, char * key, uint16_t * value)
{
    data_t * tmp;

    SYS_CALL(
        __sys_dict_get, (dst, key, &tmp),
        T(),
        RETERR()
    );

    return sys_dict_to_uint16(tmp, value);
}

err_t sys_dict_get_uint32(dict_t * dst, char * key, uint32_t * value)
{
    data_t * tmp;

    SYS_CALL(
        __sys_dict_get, (dst, key, &tmp),
        T(),
        RETERR()
    );

    return sys_dict_to_uint32(tmp, value);
}

err_t sys_dict_get_uint64(dict_t * dst, char * key, uint64_t * value)
{
    data_t * tmp;

    SYS_CALL(
        __sys_dict_get, (dst, key, &tmp),
        T(),
        RETERR()
    );

    return sys_dict_to_uint64(tmp, value);
}


err_t sys_dict_del_bin(dict_t ** dst, char * key, void * value,
                       size_t * length)
{
    if (value != NULL)
    {
        SYS_CALL(
            sys_dict_get_bin, (*dst, key, value, length),
            T(),
            RETERR()
        );
    }

    return sys_dict_del(dst, key, NULL);
}

err_t sys_dict_del_block(dict_t ** dst, char * key, void * value,
                         size_t length)
{
    if (value != NULL)
    {
        SYS_CALL(
            sys_dict_get_block, (*dst, key, value, length),
            T(),
            RETERR()
        );
    }

    return sys_dict_del(dst, key, NULL);
}

err_t sys_dict_del_str(dict_t ** dst, char * key, char * value,
                       size_t * length)
{
    if (value != NULL)
    {
        SYS_CALL(
            sys_dict_get_str, (*dst, key, value, length),
            T(),
            RETERR()
        );
    }

    return sys_dict_del(dst, key, NULL);
}

err_t sys_dict_del_uuid(dict_t ** dst, char * key, uuid_t value)
{
    return sys_dict_del_block(dst, key, value, sizeof(uuid_t));
}

err_t sys_dict_del_int8(dict_t ** dst, char * key, int8_t * value)
{
    if (value != NULL)
    {
        SYS_CALL(
            sys_dict_get_int8, (*dst, key, value),
            T(),
            RETERR()
        );
    }

    return sys_dict_del(dst, key, NULL);
}

err_t sys_dict_del_int16(dict_t ** dst, char * key, int16_t * value)
{
    if (value != NULL)
    {
        SYS_CALL(
            sys_dict_get_int16, (*dst, key, value),
            T(),
            RETERR()
        );
    }

    return sys_dict_del(dst, key, NULL);
}

err_t sys_dict_del_int32(dict_t ** dst, char * key, int32_t * value)
{
    if (value != NULL)
    {
        SYS_CALL(
            sys_dict_get_int32, (*dst, key, value),
            T(),
            RETERR()
        );
    }

    return sys_dict_del(dst, key, NULL);
}

err_t sys_dict_del_int64(dict_t ** dst, char * key, int64_t * value)
{
    if (value != NULL)
    {
        SYS_CALL(
            sys_dict_get_int64, (*dst, key, value),
            T(),
            RETERR()
        );
    }

    return sys_dict_del(dst, key, NULL);
}

err_t sys_dict_del_uint8(dict_t ** dst, char * key, uint8_t * value)
{
    if (value != NULL)
    {
        SYS_CALL(
            sys_dict_get_uint8, (*dst, key, value),
            T(),
            RETERR()
        );
    }

    return sys_dict_del(dst, key, NULL);
}

err_t sys_dict_del_uint16(dict_t ** dst, char * key, uint16_t * value)
{
    if (value != NULL)
    {
        SYS_CALL(
            sys_dict_get_uint16, (*dst, key, value),
            T(),
            RETERR()
        );
    }

    return sys_dict_del(dst, key, NULL);
}

err_t sys_dict_del_uint32(dict_t ** dst, char * key, uint32_t * value)
{
    if (value != NULL)
    {
        SYS_CALL(
            sys_dict_get_uint32, (*dst, key, value),
            T(),
            RETERR()
        );
    }

    return sys_dict_del(dst, key, NULL);
}

err_t sys_dict_del_uint64(dict_t ** dst, char * key, uint64_t * value)
{
    if (value != NULL)
    {
        SYS_CALL(
            sys_dict_get_uint64, (*dst, key, value),
            T(),
            RETERR()
        );
    }

    return sys_dict_del(dst, key, NULL);
}
