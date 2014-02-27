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

#ifndef __SYS_COMPILER_GCC_H__
#define __SYS_COMPILER_GCC_H__

#if defined(__i386) || defined(__i386__)

#define SYS_ARCH_X86

#elif defined(__amd64) || defined(__amd64__) || \
      defined(__x86_64) || defined(__x86_64__)

#define SYS_ARCH_X86_64

#else

#error "Unsupported architecture"

#endif

#include <x86intrin.h>
#include <cpuid.h>

#define CACHE_LINE_SIZE 64

static __inline void cpu_sync(void)
{
    int32_t a, b, c, d;

    __cpuid(0, a, b, c, d);
}

static __inline int64_t cpu_rdtsc(void)
{
    return __rdtsc();
}

#define cpu_pause() __pause()
#define cpu_barrier() \
    do \
    { \
        __asm__ __volatile__ ("" : : : "memory"); \
    } while (0)

#define smp_mb() \
    do \
    { \
        __asm__ __volatile__ ("mfence" : : : "memory"); \
    } while (0)

#define smp_rmb() \
    do \
    { \
        __asm__ __volatile__ ("lfence" : : : "memory"); \
    } while (0)

#define smp_wmb() \
    do \
    { \
        __asm__ __volatile__ ("sfence" : : : "memory"); \
    } while (0)

#define __must_check __attribute__((warn_unused_result))
#define __align(_n) __attribute__((aligned(_n)))
#define __inline __attribute__((always_inline))
#define __noinline __attribute__((noinline))
#define __deprecated __attribute__((deprecated))
#define __noreturn __attribute__((noreturn))
#define __section(_name) __attribute__((section(_name)))
#define __used __attribute__((used))
#define __unused __attribute__((unused))
#define __warn(_text) __attribute__((warning(_text)))
#define __error(_text) __attribute__((error(_text)))

#define likely(_x) __builtin_expect (!!(_x), 1)
#define unlikely(_x) __builtin_expect (!!(_x), 0)

#define __atomic_bit_set(_old, _addr, _bit, _memmodel) \
    ({ \
        int32_t _old; \
        __asm__ __volatile__ ("\tlock; bts %2, %1\n" \
                              "\tsbb %0, %0\n" \
                              : "=r" (_old), \
                                "+m" (*(volatile typeof(*(_addr)) *)(_addr)) \
                              : "Ir" (_bit) \
                              : "memory"); \
        _old; \
    })
#define _atomic_bit_set(_args...) __atomic_bit_set(_args)
#define atomic_bit_set(_addr, _bit, _memmodel) \
    _atomic_bit_set(SYS_TMP(1), _addr, _bit, _memmodel)

#define __atomic_bit_clear(_old, _addr, _bit, _memmodel) \
    ({ \
        int32_t _old; \
        __asm__ __volatile__ ("\tlock; btr %2, %1\n" \
                              "\tsbb %0, %0\n" \
                              : "=r" (_old), \
                                "+m" (*(volatile typeof(*(_addr)) *)(_addr)) \
                              : "Ir" (_bit) \
                              : "memory"); \
        _old; \
    })
#define _atomic_bit_clear(_args...) __atomic_bit_clear(_args)
#define atomic_bit_clear(_addr, _bit, _memmodel) \
    _atomic_bit_clear(SYS_TMP(1), _addr, _bit, _memmodel)

#define __atomic_bit_change(_old, _addr, _bit, _memmodel) \
    ({ \
        int32_t _old; \
        __asm__ __volatile__ ("\tlock; btc %2, %1\n" \
                              "\tsbb %0, %0\n" \
                              : "=r" (_old), \
                                "+m" (*(volatile typeof(*(_addr)) *)(_addr)) \
                              : "Ir" (_bit) \
                              : "memory"); \
        _old; \
    })
#define _atomic_bit_change(_args...) __atomic_bit_change(_args)
#define atomic_bit_change(_addr, _bit, _memmodel) \
    _atomic_bit_change(SYS_TMP(1), _addr, _bit, _memmodel)

#if defined(__ATOMIC_RELAXED)

#define memory_order_relaxed __ATOMIC_RELAXED
#define memory_order_consume __ATOMIC_CONSUME
#define memory_order_acquire __ATOMIC_ACQUIRE
#define memory_order_release __ATOMIC_RELEASE
#define memory_order_acq_rel __ATOMIC_ACQ_REL
#define memory_order_seq_cst __ATOMIC_SEQ_CST

#define atomic_load(_ptr, _memmodel) \
    __atomic_load_n(_ptr, _memmodel)

#define atomic_store(_ptr, _val, _memmodel) \
    __atomic_store_n(_ptr, _val, _memmodel)

#define atomic_xchg(_ptr, _val, _memmodel) \
    __atomic_exchange_n(_ptr, _val, _memmodel)

#define atomic_cmpxchg(_ptr, _cmp, _val, _memmodel_ok, _memmodel_fail) \
    ({ \
        typeof(_cmp) __tmp_cmp = (_cmp); \
        __atomic_compare_exchange_n(_ptr, &__tmp_cmp, _val, true, \
                                    _memmodel_ok, _memmodel_fail); \
    })

#define atomic_add(_ptr, _val, _memmodel) \
    __atomic_fetch_add(_ptr, _val, _memmodel)
#define atomic_add_return(_ptr, _val, _memmodel) \
    __atomic_add_fetch(_ptr, _val, _memmodel)

#define atomic_sub(_ptr, _val, _memmodel) \
    __atomic_fetch_sub(_ptr, _val, _memmodel)
#define atomic_sub_return(_ptr, _val, _memmodel) \
    __atomic_sub_fetch(_ptr, _val, _memmodel)

#define atomic_and(_ptr, _val, _memmodel) \
    __atomic_fetch_and(_ptr, _val, _memmodel)
#define atomic_and_return(_ptr, _val, _memmodel) \
    __atomic_and_fetch(_ptr, _val, _memmodel)

#define atomic_xor(_ptr, _val, _memmodel) \
    __atomic_fetch_xor(_ptr, _val, _memmodel)
#define atomic_xor_return(_ptr, _val, _memmodel) \
    __atomic_xor_fetch(_ptr, _val, _memmodel)

#define atomic_or(_ptr, _val, _memmodel) \
    __atomic_fetch_or(_ptr, _val, _memmodel)
#define atomic_or_return(_ptr, _val, _memmodel) \
    __atomic_or_fetch(_ptr, _val, _memmodel)

#else /* !defined(__ATOMIC_RELAXED) */

#define memory_order_relaxed 0
#define memory_order_consume 1
#define memory_order_acquire 2
#define memory_order_release 3
#define memory_order_acq_rel 4
#define memory_order_seq_cst 5

#define atomic_load(_ptr, _memmodel) \
    ({ \
        typeof(*(_ptr)) __tmp_data = *(_ptr); \
        if ((_memmodel) >= memory_order_acquire) \
        { \
            cpu_barrier(); \
        } \
        __tmp_data; \
    })

#define atomic_store(_ptr, _value, _memmodel) \
    do \
    { \
        if ((_memmodel) >= memory_order_release) \
        { \
            cpu_barrier(); \
        } \
        *(_ptr) = (_value); \
    } while (0)

#define atomic_xchg(_ptr, _val, _memmodel) \
    __sync_lock_test_and_set(_ptr, _val)

#define atomic_cmpxchg(_ptr, _cmp, _val, _memmodel_ok, _memmodel_fail) \
    __sync_bool_compare_and_swap(_ptr, _cmp, _val)

#define atomic_add(_ptr, _val, _memmodel) \
    __sync_fetch_and_add(_ptr, _val)
#define atomic_add_return(_ptr, _val, _memmodel) \
    __sync_add_and_fetch(_ptr, _val)

#define atomic_sub(_ptr, _val, _memmodel) \
    __sync_fetch_and_sub(_ptr, _val)
#define atomic_sub_return(_ptr, _val, _memmodel) \
    __sync_sub_and_fetch(_ptr, _val)

#define atomic_and(_ptr, _val, _memmodel) \
    __sync_fetch_and_and(_ptr, _val)
#define atomic_and_return(_ptr, _val, _memmodel) \
    __sync_and_and_fetch(_ptr, _val)

#define atomic_xor(_ptr, _val, _memmodel) \
    __sync_fetch_and_xor(_ptr, _val)
#define atomic_xor_return(_ptr, _val, _memmodel) \
    __sync_xor_and_fetch(_ptr, _val)

#define atomic_or(_ptr, _val, _memmodel) \
    __sync_fetch_and_or(_ptr, _val)
#define atomic_or_return(_ptr, _val, _memmodel) \
    __sync_or_and_fetch(_ptr, _val)

#endif /* !defined(__ATOMIC_RELAXED) */

#define sys_bits_count8(_n) __builtin_popcount((_n) & 0xFF)
#define sys_bits_count16(_n) __builtin_popcount((_n) & 0xFFFF)
#define sys_bits_count32(_n) __builtin_popcount((_n) & 0xFFFFFFFF)
#define sys_bits_count64(_n) __builtin_popcountll((_n) & 0xFFFFFFFFFFFFFFFF)

#define sys_bits_first_one_index8(_n) \
    ({ \
        typeof(_n) __tmp_num = (_n) & 0xFF; \
        (__tmp_num == 0) ? -1 : __builtin_ctz(__tmp_num); \
    })
#define sys_bits_first_one_index16(_n) \
    ({ \
        typeof(_n) __tmp_num = (_n) & 0xFFFF; \
        (__tmp_num == 0) ? -1 : __builtin_ctz(__tmp_num); \
    })
#define sys_bits_first_one_index32(_n) \
    ({ \
        typeof(_n) __tmp_num = (_n) & 0xFFFFFFFF; \
        (__tmp_num == 0) ? -1 : __builtin_ctz(__tmp_num); \
    })
#define sys_bits_first_one_index64(_n) \
    ({ \
        typeof(_n) __tmp_num = (_n) & 0xFFFFFFFFFFFFFFFF; \
        (__tmp_num == 0) ? -1 : __builtin_ctzll(__tmp_num); \
    })

#define sys_bits_last_one_index8(_n) \
    ({ \
        typeof(_n) __tmp_num = (_n) & 0xFF; \
        (__tmp_num == 0) ? -1 : 7 - __builtin_clz(__tmp_num); \
    })
#define sys_bits_last_one_index16(_n) \
    ({ \
        typeof(_n) __tmp_num = (_n) & 0xFFFF; \
        (__tmp_num == 0) ? -1 : 15 - __builtin_clz(__tmp_num); \
    })
#define sys_bits_last_one_index32(_n) \
    ({ \
        typeof(_n) __tmp_num = (_n) & 0xFFFFFFFF; \
        (__tmp_num == 0) ? -1 : 31 - __builtin_clz(__tmp_num); \
    })
#define sys_bits_last_one_index64(_n) \
    ({ \
        typeof(_n) __tmp_num = (_n) & 0xFFFFFFFFFFFFFFFF; \
        (__tmp_num == 0) ? -1 : 63 - __builtin_clzll(__tmp_num); \
    })

#endif /* __SYS_COMPILER_GCC_H__ */
