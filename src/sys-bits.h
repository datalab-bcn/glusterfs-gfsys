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

#ifndef __SYS_BITS_H__
#define __SYS_BITS_H__

#include "sys-compiler.h"

#ifndef sys_bits_count8
#define sys_bits_count8(_n) \
    ({ \
        typeof(_n) __tmp_num = (_n); \
        __tmp_num -= (__tmp_num >> 1) & 0x55; \
        __tmp_num = (__tmp_num & 0x33) + ((__tmp_num >> 2) & 0x33); \
        __tmp_num = (__tmp_num + (__tmp_num >> 4)) & 0x0F; \
        __tmp_num; \
    })
#endif

#ifndef sys_bits_count16
#define sys_bits_count16(_n) \
    ({ \
        typeof(_n) __tmp_num = (_n); \
        __tmp_num -= (__tmp_num >> 1) & 0x5555; \
        __tmp_num = (__tmp_num & 0x3333) + ((__tmp_num >> 2) & 0x3333); \
        __tmp_num = (__tmp_num + (__tmp_num >> 4)) & 0x0F0F; \
        __tmp_num += __tmp_num >> 8; \
        __tmp_num &= 0xFF; \
        __tmp_num; \
    })
#endif

#ifndef sys_bits_count32
#define sys_bits_count32(_n) \
    ({ \
        typeof(_n) __tmp_num = (_n); \
        __tmp_num -= (__tmp_num >> 1) & 0x55555555; \
        __tmp_num = (__tmp_num & 0x33333333) + \
                    ((__tmp_num >> 2) & 0x33333333); \
        __tmp_num = (__tmp_num + (__tmp_num >> 4)) & 0x0F0F0F0F; \
        __tmp_num += __tmp_num >> 8; \
        __tmp_num += __tmp_num >> 16; \
        __tmp_num &= 0xFF; \
        __tmp_num; \
    })
#endif

#ifndef sys_bits_count64
#define sys_bits_count64(_n) \
    ({ \
        typeof(_n) __tmp_num = (_n); \
        __tmp_num -= (__tmp_num >> 1) & 0x5555555555555555; \
        __tmp_num = (__tmp_num & 0x3333333333333333) + \
                    ((__tmp_num >> 2) & 0x3333333333333333); \
        __tmp_num = (__tmp_num + (__tmp_num >> 4)) & 0x0F0F0F0F0F0F0F0F; \
        __tmp_num += __tmp_num >> 8; \
        __tmp_num += __tmp_num >> 16; \
        __tmp_num += __tmp_num >> 32; \
        __tmp_num &= 0xFF; \
        __tmp_num; \
    })
#endif

#ifndef sys_bits_reverse8
#define sys_bits_reverse8(_n) \
    ({ \
        typeof(_n) __tmp_num = (_n); \
        __tmp_num = ((__tmp_num >> 4) & 0x0F) | ((__tmp_num & 0x0F) << 4); \
        __tmp_num = ((__tmp_num >> 2) & 0x33) | ((__tmp_num & 0x33) << 2); \
        __tmp_num = ((__tmp_num >> 1) & 0x55) | ((__tmp_num & 0x55) << 1); \
        __tmp_num; \
    })
#endif

#ifndef sys_bits_reverse16
#define sys_bits_reverse16(_n) \
    ({ \
        typeof(_n) __tmp_num = (_n); \
        __tmp_num = ((__tmp_num >> 8) & 0x00FF) | \
                    ((__tmp_num & 0x00FF) << 8); \
        __tmp_num = ((__tmp_num >> 4) & 0x0F0F) | \
                    ((__tmp_num & 0x0F0F) << 4); \
        __tmp_num = ((__tmp_num >> 2) & 0x3333) | \
                    ((__tmp_num & 0x3333) << 2); \
        __tmp_num = ((__tmp_num >> 1) & 0x5555) | \
                    ((__tmp_num & 0x5555) << 1); \
        __tmp_num; \
    })
#endif

#ifndef sys_bits_reverse32
#define sys_bits_reverse32(_n) \
    ({ \
        typeof(_n) __tmp_num = (_n); \
        __tmp_num = ((__tmp_num >> 16) & 0x0000FFFF) | \
                    ((__tmp_num & 0x0000FFFF) << 16); \
        __tmp_num = ((__tmp_num >>  8) & 0x00FF00FF) | \
                    ((__tmp_num & 0x00FF00FF) <<  8); \
        __tmp_num = ((__tmp_num >>  4) & 0x0F0F0F0F) | \
                    ((__tmp_num & 0x0F0F0F0F) <<  4); \
        __tmp_num = ((__tmp_num >>  2) & 0x33333333) | \
                    ((__tmp_num & 0x33333333) <<  2); \
        __tmp_num = ((__tmp_num >>  1) & 0x55555555) | \
                    ((__tmp_num & 0x55555555) <<  1); \
        __tmp_num; \
    })
#endif

#ifndef sys_bits_reverse64
#define sys_bits_reverse64(_n) \
    ({ \
        typeof(_n) __tmp_num = (_n); \
        __tmp_num = ((__tmp_num >> 32) & 0x00000000FFFFFFFF) | \
                    ((__tmp_num & 0x00000000FFFFFFFF) << 32); \
        __tmp_num = ((__tmp_num >> 16) & 0x0000FFFF0000FFFF) | \
                    ((__tmp_num & 0x0000FFFF0000FFFF) << 16); \
        __tmp_num = ((__tmp_num >>  8) & 0x00FF00FF00FF00FF) | \
                    ((__tmp_num & 0x00FF00FF00FF00FF) <<  8); \
        __tmp_num = ((__tmp_num >>  4) & 0x0F0F0F0F0F0F0F0F) | \
                    ((__tmp_num & 0x0F0F0F0F0F0F0F0F) <<  4); \
        __tmp_num = ((__tmp_num >>  2) & 0x3333333333333333) | \
                    ((__tmp_num & 0x3333333333333333) <<  2); \
        __tmp_num = ((__tmp_num >>  1) & 0x5555555555555555) | \
                    ((__tmp_num & 0x5555555555555555) <<  1); \
        __tmp_num; \
    })
#endif

#ifndef sys_bits_first_one_index8
#define sys_bits_first_one_index8(_n) (ffs((_n) & 0xFF) - 1)
#endif

#ifndef sys_bits_first_one_index16
#define sys_bits_first_one_index16(_n) (ffs((_n) & 0xFFFF) - 1)
#endif

#ifndef sys_bits_first_one_index32
#define sys_bits_first_one_index32(_n) (ffs((_n) & 0xFFFFFFFF) - 1)
#endif

#ifndef sys_bits_first_one_index64
#define sys_bits_first_one_index64(_n) (ffsll((_n) & 0xFFFFFFFFFFFFFFFF) - 1)
#endif

#ifndef sys_bits_first_zero_index8
#define sys_bits_first_zero_index8(_n) sys_bits_first_one_index8(~(_n))
#endif

#ifndef sys_bits_first_zero_index16
#define sys_bits_first_zero_index16(_n) sys_bits_first_one_index16(~(_n))
#endif

#ifndef sys_bits_first_zero_index32
#define sys_bits_first_zero_index32(_n) sys_bits_first_one_index32(~(_n))
#endif

#ifndef sys_bits_first_zero_index64
#define sys_bits_first_zero_index64(_n) sys_bits_first_one_index64(~(_n))
#endif

#ifndef sys_bits_first_one_mask
#define sys_bits_first_one_mask(_n) \
    ({ \
        typeof(_n) __tmp_num = (_n); \
        __tmp_num & (-__tmp_num); \
    })
#endif

#ifndef sys_bits_first_zero_mask
#define sys_bits_first_zero_mask(_n) sys_bits_first_one_mask(~(_n))
#endif

#ifndef sys_bits_first_one_mask8
#define sys_bits_first_one_mask8(_n) (sys_bits_first_one_mask(_n) & 0xFF)
#endif

#ifndef sys_bits_first_one_mask16
#define sys_bits_first_one_mask16(_n) (sys_bits_first_one_mask(_n) & 0xFFFF)
#endif

#ifndef sys_bits_first_one_mask32
#define sys_bits_first_one_mask32(_n) \
            (sys_bits_first_one_mask(_n) & 0xFFFFFFFF)
#endif

#ifndef sys_bits_first_one_mask64
#define sys_bits_first_one_mask64(_n) \
            (sys_bits_first_one_mask(_n) & 0xFFFFFFFFFFFFFFFF)
#endif

#ifndef sys_bits_first_zero_mask8
#define sys_bits_first_zero_mask8(_n) (sys_bits_first_zero_mask(_n) & 0xFF)
#endif

#ifndef sys_bits_first_zero_mask16
#define sys_bits_first_zero_mask16(_n) (sys_bits_first_zero_mask(_n) & 0xFFFF)
#endif

#ifndef sys_bits_first_zero_mask32
#define sys_bits_first_zero_mask32(_n) \
            (sys_bits_first_zero_mask(_n) & 0xFFFFFFFF)
#endif

#ifndef sys_bits_first_zero_mask64
#define sys_bits_first_zero_mask64(_n) \
            (sys_bits_first_zero_mask(_n) & 0xFFFFFFFFFFFFFFFF)
#endif

#ifndef sys_bits_last_one_index8
#define sys_bits_last_one_index8(_n) \
    ({ \
        typeof(_n) __tmp_num = (_n) & 0xFF; \
        int32_t __tmp_result = (__tmp_num > 0xF) << 2; \
        __tmp_num >>= __tmp_result; \
        int32_t __tmp_shift = (__tmp_num > 3) << 1; \
        __tmp_num >>= __tmp_shift; __tmp_result |= __tmp_shift; \
        __tmp_result |= (__tmp_num > 1); \
        __tmp_result - 1; \
    })
#endif

#ifndef sys_bits_last_one_index16
#define sys_bits_last_one_index16(_n) \
    ({ \
        typeof(_n) __tmp_num = (_n) & 0xFFFF; \
        int32_t __tmp_result = (__tmp_num > 0xFF) << 3; \
        __tmp_num >>= __tmp_result; \
        int32_t __tmp_shift = (__tmp_num > 0xF) << 2; \
        __tmp_num >>= __tmp_shift; __tmp_result |= __tmp_shift; \
        __tmp_shift = (__tmp_num > 3) << 1; \
        __tmp_num >>= __tmp_shift; __tmp_result |= __tmp_shift; \
        __tmp_result |= (__tmp_num > 1); \
        __tmp_result - 1; \
    })
#endif

#ifndef sys_bits_last_one_index32
#define sys_bits_last_one_index32(_n) \
    ({ \
        typeof(_n) __tmp_num = (_n) & 0xFFFFFFFF; \
        int32_t __tmp_result = (__tmp_num > 0xFFFF) << 4; \
        __tmp_num >>= __tmp_result; \
        int32_t __tmp_shift = (__tmp_num > 0xFF) << 3; \
        __tmp_num >>= __tmp_shift; __tmp_result |= __tmp_shift; \
        __tmp_shift = (__tmp_num > 0xF) << 2; \
        __tmp_num >>= __tmp_shift; __tmp_result |= __tmp_shift; \
        __tmp_shift = (__tmp_num > 3) << 1; \
        __tmp_num >>= __tmp_shift; __tmp_result |= __tmp_shift; \
        __tmp_result |= (__tmp_num > 1); \
        __tmp_result - 1; \
    })
#endif

#ifndef sys_bits_last_one_index64
#define sys_bits_last_one_index64(_n) \
    ({ \
        typeof(_n) __tmp_num = (_n) & 0xFFFFFFFFFFFFFFFF; \
        int32_t __tmp_result = (__tmp_num > 0xFFFFFFFF) << 5; \
        __tmp_num >>= __tmp_result; \
        int32_t __tmp_shift = (__tmp_num > 0xFFFF) << 4; \
        __tmp_num >>= __tmp_shift; __tmp_result |= __tmp_shift; \
        __tmp_shift = (__tmp_num > 0xFF) << 3; \
        __tmp_num >>= __tmp_shift; __tmp_result |= __tmp_shift; \
        __tmp_shift = (__tmp_num > 0xF) << 2; \
        __tmp_num >>= __tmp_shift; __tmp_result |= __tmp_shift; \
        __tmp_shift = (__tmp_num > 3) << 1; \
        __tmp_num >>= __tmp_shift; __tmp_result |= __tmp_shift; \
        __tmp_result |= (__tmp_num > 1); \
        __tmp_result - 1; \
    })
#endif

#ifndef sys_bits_last_zero_index8
#define sys_bits_last_zero_index8(_n) sys_bits_last_one_index8(~(_n))
#endif

#ifndef sys_bits_last_zero_index16
#define sys_bits_last_zero_index16(_n) sys_bits_last_one_index16(~(_n))
#endif

#ifndef sys_bits_last_zero_index32
#define sys_bits_last_zero_index32(_n) sys_bits_last_one_index32(~(_n))
#endif

#ifndef sys_bits_last_zero_index64
#define sys_bits_last_zero_index64(_n) sys_bits_last_one_index64(~(_n))
#endif

#ifndef sys_bits_last_one_mask8
#define sys_bits_last_one_mask8(_n) \
    ({ \
        typeof(_n) __tmp_num = (_n) & 0xFF; \
        (__tmp_num != 0) << (sys_bits_last_one_index8(__tmp_num) & 7); \
    })
#endif

#ifndef sys_bits_last_one_mask16
#define sys_bits_last_one_mask16(_n) \
    ({ \
        typeof(_n) __tmp_num = (_n) & 0xFFFF; \
        (__tmp_num != 0) << (sys_bits_last_one_index16(__tmp_num) & 15); \
    })
#endif

#ifndef sys_bits_last_one_mask32
#define sys_bits_last_one_mask32(_n) \
    ({ \
        typeof(_n) __tmp_num = (_n) & 0xFFFFFFFF; \
        (__tmp_num != 0) << (sys_bits_last_one_index32(__tmp_num) & 31); \
    })
#endif

#ifndef sys_bits_last_one_mask64
#define sys_bits_last_one_mask64(_n) \
    ({ \
        typeof(_n) __tmp_num = (_n) & 0xFFFFFFFFFFFFFFFF; \
        (__tmp_num != 0) << (sys_bits_last_one_index64(__tmp_num) & 63); \
    })
#endif

#ifndef sys_bits_last_zero_mask8
#define sys_bits_last_zero_mask8(_n) \
    ({ \
        typeof(_n) __tmp_num = (_n) & 0xFF; \
        (__tmp_num != 0) << (sys_bits_last_zero_index8(__tmp_num) & 7); \
    })
#endif

#ifndef sys_bits_last_zero_mask16
#define sys_bits_last_zero_mask16(_n) \
    ({ \
        typeof(_n) __tmp_num = (_n) & 0xFFFF; \
        (__tmp_num != 0) << (sys_bits_last_zero_index16(__tmp_num) & 15); \
    })
#endif

#ifndef sys_bits_last_zero_mask32
#define sys_bits_last_zero_mask32(_n) \
    ({ \
        typeof(_n) __tmp_num = (_n) & 0xFFFFFFFF; \
        (__tmp_num != 0) << (sys_bits_last_zero_index32(__tmp_num) & 31); \
    })
#endif

#ifndef sys_bits_last_zero_mask64
#define sys_bits_last_zero_mask64(_n) \
    ({ \
        typeof(_n) __tmp_num = (_n) & 0xFFFFFFFFFFFFFFFF; \
        (__tmp_num != 0) << (sys_bits_last_zero_index64(__tmp_num) & 63); \
    })
#endif

#ifndef sys_bits_is_two_power
#define sys_bits_is_two_power(_n) \
    ({ \
        typeof(_n) __tmp_num = (_n); \
        (__tmp_num & (-__tmp_num)) == __tmp_num; \
    })
#endif

#ifndef sys_bits_next_two_power8
#define sys_bits_next_two_power8(_n) \
    ((1 << (sys_bits_last_one_index8(_n) + 1)) & 0xFF)
#endif

#ifndef sys_bits_next_two_power16
#define sys_bits_next_two_power16(_n) \
    ((1 << (sys_bits_last_one_index16(_n) + 1)) & 0xFFFF)
#endif

#ifndef sys_bits_next_two_power32
#define sys_bits_next_two_power32(_n) \
    ((1 << (sys_bits_last_one_index32(_n) + 1)) & 0xFFFFFFFF)
#endif

#ifndef sys_bits_next_two_power64
#define sys_bits_next_two_power64(_n) \
    ((1ULL << (sys_bits_last_one_index64(_n) + 1)) & 0xFFFFFFFFFFFFFFFF)
#endif

#endif /* __SYS_BITS_H__ */
