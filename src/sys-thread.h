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

#ifndef __SYS_THREAD_H__
#define __SYS_THREAD_H__

#define sys_thread_for_each_online(_core) \
    for ((_core) = sys_thread_core_next_online(-1); \
         (_core) >= 0; \
         (_core) = sys_thread_core_next_online(_core))

#define SIGADD(_val, _num) ((_val) | \
                          (((_num) == 0) ? 0 : (1ULL << ((_num) - 1))))

#define SIG64(_val, _num, _list...) _val
#define SIG63(_val, _num, _list...) SIG64(SIGADD(_val, _num), ## _list, 0)
#define SIG62(_val, _num, _list...) SIG63(SIGADD(_val, _num), ## _list, 0)
#define SIG61(_val, _num, _list...) SIG62(SIGADD(_val, _num), ## _list, 0)
#define SIG60(_val, _num, _list...) SIG61(SIGADD(_val, _num), ## _list, 0)
#define SIG59(_val, _num, _list...) SIG60(SIGADD(_val, _num), ## _list, 0)
#define SIG58(_val, _num, _list...) SIG59(SIGADD(_val, _num), ## _list, 0)
#define SIG57(_val, _num, _list...) SIG58(SIGADD(_val, _num), ## _list, 0)
#define SIG56(_val, _num, _list...) SIG57(SIGADD(_val, _num), ## _list, 0)
#define SIG55(_val, _num, _list...) SIG56(SIGADD(_val, _num), ## _list, 0)
#define SIG54(_val, _num, _list...) SIG55(SIGADD(_val, _num), ## _list, 0)
#define SIG53(_val, _num, _list...) SIG54(SIGADD(_val, _num), ## _list, 0)
#define SIG52(_val, _num, _list...) SIG53(SIGADD(_val, _num), ## _list, 0)
#define SIG51(_val, _num, _list...) SIG52(SIGADD(_val, _num), ## _list, 0)
#define SIG50(_val, _num, _list...) SIG51(SIGADD(_val, _num), ## _list, 0)
#define SIG49(_val, _num, _list...) SIG50(SIGADD(_val, _num), ## _list, 0)
#define SIG48(_val, _num, _list...) SIG49(SIGADD(_val, _num), ## _list, 0)
#define SIG47(_val, _num, _list...) SIG48(SIGADD(_val, _num), ## _list, 0)
#define SIG46(_val, _num, _list...) SIG47(SIGADD(_val, _num), ## _list, 0)
#define SIG45(_val, _num, _list...) SIG46(SIGADD(_val, _num), ## _list, 0)
#define SIG44(_val, _num, _list...) SIG45(SIGADD(_val, _num), ## _list, 0)
#define SIG43(_val, _num, _list...) SIG44(SIGADD(_val, _num), ## _list, 0)
#define SIG42(_val, _num, _list...) SIG43(SIGADD(_val, _num), ## _list, 0)
#define SIG41(_val, _num, _list...) SIG42(SIGADD(_val, _num), ## _list, 0)
#define SIG40(_val, _num, _list...) SIG41(SIGADD(_val, _num), ## _list, 0)
#define SIG39(_val, _num, _list...) SIG40(SIGADD(_val, _num), ## _list, 0)
#define SIG38(_val, _num, _list...) SIG39(SIGADD(_val, _num), ## _list, 0)
#define SIG37(_val, _num, _list...) SIG38(SIGADD(_val, _num), ## _list, 0)
#define SIG36(_val, _num, _list...) SIG37(SIGADD(_val, _num), ## _list, 0)
#define SIG35(_val, _num, _list...) SIG36(SIGADD(_val, _num), ## _list, 0)
#define SIG34(_val, _num, _list...) SIG35(SIGADD(_val, _num), ## _list, 0)
#define SIG33(_val, _num, _list...) SIG34(SIGADD(_val, _num), ## _list, 0)
#define SIG32(_val, _num, _list...) SIG33(SIGADD(_val, _num), ## _list, 0)
#define SIG31(_val, _num, _list...) SIG32(SIGADD(_val, _num), ## _list, 0)
#define SIG30(_val, _num, _list...) SIG31(SIGADD(_val, _num), ## _list, 0)
#define SIG29(_val, _num, _list...) SIG30(SIGADD(_val, _num), ## _list, 0)
#define SIG28(_val, _num, _list...) SIG29(SIGADD(_val, _num), ## _list, 0)
#define SIG27(_val, _num, _list...) SIG28(SIGADD(_val, _num), ## _list, 0)
#define SIG26(_val, _num, _list...) SIG27(SIGADD(_val, _num), ## _list, 0)
#define SIG25(_val, _num, _list...) SIG26(SIGADD(_val, _num), ## _list, 0)
#define SIG24(_val, _num, _list...) SIG25(SIGADD(_val, _num), ## _list, 0)
#define SIG23(_val, _num, _list...) SIG24(SIGADD(_val, _num), ## _list, 0)
#define SIG22(_val, _num, _list...) SIG23(SIGADD(_val, _num), ## _list, 0)
#define SIG21(_val, _num, _list...) SIG22(SIGADD(_val, _num), ## _list, 0)
#define SIG20(_val, _num, _list...) SIG21(SIGADD(_val, _num), ## _list, 0)
#define SIG19(_val, _num, _list...) SIG20(SIGADD(_val, _num), ## _list, 0)
#define SIG18(_val, _num, _list...) SIG19(SIGADD(_val, _num), ## _list, 0)
#define SIG17(_val, _num, _list...) SIG18(SIGADD(_val, _num), ## _list, 0)
#define SIG16(_val, _num, _list...) SIG17(SIGADD(_val, _num), ## _list, 0)
#define SIG15(_val, _num, _list...) SIG16(SIGADD(_val, _num), ## _list, 0)
#define SIG14(_val, _num, _list...) SIG15(SIGADD(_val, _num), ## _list, 0)
#define SIG13(_val, _num, _list...) SIG14(SIGADD(_val, _num), ## _list, 0)
#define SIG12(_val, _num, _list...) SIG13(SIGADD(_val, _num), ## _list, 0)
#define SIG11(_val, _num, _list...) SIG12(SIGADD(_val, _num), ## _list, 0)
#define SIG10(_val, _num, _list...) SIG11(SIGADD(_val, _num), ## _list, 0)
#define SIG09(_val, _num, _list...) SIG10(SIGADD(_val, _num), ## _list, 0)
#define SIG08(_val, _num, _list...) SIG09(SIGADD(_val, _num), ## _list, 0)
#define SIG07(_val, _num, _list...) SIG08(SIGADD(_val, _num), ## _list, 0)
#define SIG06(_val, _num, _list...) SIG07(SIGADD(_val, _num), ## _list, 0)
#define SIG05(_val, _num, _list...) SIG06(SIGADD(_val, _num), ## _list, 0)
#define SIG04(_val, _num, _list...) SIG05(SIGADD(_val, _num), ## _list, 0)
#define SIG03(_val, _num, _list...) SIG04(SIGADD(_val, _num), ## _list, 0)
#define SIG02(_val, _num, _list...) SIG03(SIGADD(_val, _num), ## _list, 0)
#define SIG01(_val, _num, _list...) SIG02(SIGADD(_val, _num), ## _list, 0)
#define SIG00(_val, _num, _list...) SIG01(SIGADD(_val, _num), ## _list, 0)

#define SYS_SIG(_list...) SIG00(0, ## _list, 0)

extern int32_t sys_thread_core_conf;
extern int32_t sys_thread_core_onln;

int32_t sys_thread_initialize(void);
void sys_thread_terminate(void);
int32_t sys_thread_core_count(void);
int32_t sys_thread_core_next_online(int32_t current);
void sys_thread_priority(pthread_t * thread, int32_t priority);
int32_t sys_thread_migrate(pthread_t * thread, int32_t target);
int32_t sys_thread_where(void);
int32_t sys_thread_create(pthread_t * thread, int32_t priority, int32_t target,
                          uint32_t stack_size, uint64_t signals,
                          int32_t (* initializer)(void *),
                          void * (* main)(void *), void * arg);
int32_t sys_thread_wait(pthread_t thread, sys_time_t * timeout,
                        void ** result);

void sys_thread_signal_mask(uint64_t unblock, uint64_t block);
void sys_thread_signal_kill(pthread_t * thread, int32_t signum);
void sys_thread_signal_cancel(uint64_t signals);
err_t sys_thread_signal_wait(int32_t * signum, uint64_t signals,
                             sys_time_t * timeout);

extern __thread err_t sys_thread_error;

static inline void sys_thread_set_error(err_t error)
{
    sys_thread_error = error;
}

static inline err_t sys_thread_get_error(void)
{
    return sys_thread_error;
}

#endif /* __SYS_THREAD_H__ */
