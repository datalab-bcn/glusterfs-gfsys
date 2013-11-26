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

err_t sys_buf_read(void ** ptr, size_t * size, const char * fmt, ...)
{
    va_list args;
    void * target;
    uint32_t * length, max;
    err_t error;
    char c;

    error = 0;

    va_start(args, fmt);

    c = *fmt++;
    while (c != 0)
    {
        switch (c)
        {
            case '1':
                target = va_arg(args, uint8_t *);
                SYS_CALL(
                    sys_buf_get_uint8, (ptr, size, target),
                    T(),
                    GOTO(failed, &error)
                );
                break;
            case '2':
                target = va_arg(args, uint16_t *);
                SYS_CALL(
                    sys_buf_get_uint16, (ptr, size, target),
                    T(),
                    GOTO(failed, &error)
                );
                break;
            case '4':
                target = va_arg(args, uint32_t *);
                SYS_CALL(
                    sys_buf_get_uint32, (ptr, size, target),
                    T(),
                    GOTO(failed, &error)
                );
                break;
            case '8':
                target = va_arg(args, uint64_t *);
                SYS_CALL(
                    sys_buf_get_uint64, (ptr, size, target),
                    T(),
                    GOTO(failed, &error)
                );
                break;
            case 'u':
                target = va_arg(args, uuid_t *);
                SYS_CALL(
                    sys_buf_get_uuid, (ptr, size, target),
                    T(),
                    GOTO(failed, &error)
                );
                break;
            case 'r':
                target = va_arg(args, void *);
                max = va_arg(args, uint32_t);
                SYS_CALL(
                    sys_buf_get_raw, (ptr, size, target, max),
                    T(),
                    GOTO(failed, &error)
                );
                break;
            case 'b':
                target = va_arg(args, void *);
                max = va_arg(args, uint32_t);
                length = va_arg(args, uint32_t *);
                SYS_CALL(
                    sys_buf_get_block, (ptr, size, target, max, length),
                    T(),
                    GOTO(failed, &error)
                );
                break;
            case 's':
                target = va_arg(args, char *);
                max = va_arg(args, uint32_t);
                length = va_arg(args, uint32_t *);
                SYS_CALL(
                    sys_buf_get_str, (ptr, size, target, max, length),
                    T(),
                    GOTO(failed, &error)
                );
                break;
            default:
                sys_panic("Invalid buffer format string");
        }
        c = *fmt++;
    }

failed:
    va_end(args);

    return error;
}

err_t sys_buf_write(void ** ptr, size_t * size, const char * fmt, ...)
{
    va_list args;
    void * source;
    uint32_t max;
    err_t error;
    uint64_t u64;
    uint32_t u32;
    uint16_t u16;
    uint8_t u8;
    char c;

    error = 0;

    va_start(args, fmt);

    c = *fmt++;
    while (c != 0)
    {
        switch (c)
        {
            case '1':
                u8 = va_arg(args, uint32_t);
                SYS_CALL(
                    sys_buf_set_uint8, (ptr, size, u8),
                    T(),
                    GOTO(failed, &error)
                );
                break;
            case '2':
                u16 = va_arg(args, uint32_t);
                SYS_CALL(
                    sys_buf_set_uint16, (ptr, size, u16),
                    T(),
                    GOTO(failed, &error)
                );
                break;
            case '4':
                u32 = va_arg(args, uint32_t);
                SYS_CALL(
                    sys_buf_set_uint32, (ptr, size, u32),
                    T(),
                    GOTO(failed, &error)
                );
                break;
            case '8':
                u64 = va_arg(args, uint64_t);
                SYS_CALL(
                    sys_buf_set_uint64, (ptr, size, u64),
                    T(),
                    GOTO(failed, &error)
                );
                break;
            case 'u':
                source = va_arg(args, uuid_t *);
                SYS_CALL(
                    sys_buf_set_uuid, (ptr, size, source),
                    T(),
                    GOTO(failed, &error)
                );
                break;
            case 'r':
                source = va_arg(args, void *);
                max = va_arg(args, uint32_t);
                SYS_CALL(
                    sys_buf_set_raw, (ptr, size, source, max),
                    T(),
                    GOTO(failed, &error)
                );
                break;
            case 'b':
                source = va_arg(args, void *);
                max = va_arg(args, uint32_t);
                SYS_CALL(
                    sys_buf_set_block, (ptr, size, source, max),
                    T(),
                    GOTO(failed, &error)
                );
                break;
            case 's':
                source = va_arg(args, char *);
                SYS_CALL(
                    sys_buf_set_str, (ptr, size, source),
                    T(),
                    GOTO(failed, &error)
                );
                break;
            default:
                sys_panic("Invalid buffer format string");
        }
        c = *fmt++;
    }

failed:
    va_end(args);

    return error;
}
