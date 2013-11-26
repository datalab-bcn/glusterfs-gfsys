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

err_t sys_numfmt_dec(char * buffer, int32_t * size,
                     uint64_t number, char group)
{
    int32_t blocks[7];
    int32_t i, len;

    len = 2;
    for (i = 0; i < 7; i++)
    {
        blocks[i] = number % 1000;
        number = number / 1000;
        if (number == 0)
        {
            break;
        }
        len += 4;
    }
    if (blocks[i] > 9)
    {
        len++;
    }
    if (blocks[i] > 99)
    {
        len++;
    }
    if (*size < len)
    {
        return ENOBUFS;
    }
    len = sprintf(buffer, "%u", blocks[i]);
    while (i > 0)
    {
        len += sprintf(buffer + len, "%c%03u", group, blocks[--i]);
    }

    *size = len;

    return 0;
}

err_t sys_numfmt_units(char * buffer, int32_t * size, int32_t * scale,
                       int32_t min_scale, int32_t max_scale, int32_t factor,
                       uint64_t number, char group, int32_t decimals,
                       char point)
{
    double num;
    int32_t i, len;

    num = number;
    for (i = 0; i < min_scale; i++)
    {
        num /= factor;
    }
    while ((num >= factor) && (i < max_scale))
    {
        num /= factor;
        i++;
    }
    *scale = i;
    len = *size;
    SYS_CALL(
        sys_numfmt_dec, (buffer, &len, num, group),
        E(),
        RETERR()
    );
    if ((decimals > 0) && (i > 0))
    {
        if (*size - len < decimals + 2)
        {
            return ENOBUFS;
        }
        num -= (uint64_t)num;
        for (i = 0; i < decimals; i++)
        {
            num *= 10;
        }
        num += 0.5;
        len += sprintf(buffer + len, "%c%0*lu",
                       point, decimals, (uint64_t)num);
    }

    *size = len;

    return 0;
}
