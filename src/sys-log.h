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

#ifndef __SYS_LOG_H__
#define __SYS_LOG_H__

#include <logging.h>

/* NOTE: These macros seem ugly, but they are very useful to write less, more
 *       compact, more clean and easy to follow code.
 */

/**
 * Logging.
 *
 * This module exports the following macros:
 *
 *     sys_log(_mode, _fmt...)
 *
 *         Macro to log a message with optional arguments (like printf()). The
 *         mode is defined as:
 *
 *             <level>[<type>]([<repetitions>[, <arguments>][, <name>]])
 *
 *         Where <level> is one of these (required):
 *
 *             U: Unconditional (always logged)
 *             M: Emergency
 *             A: Alert
 *             C: Critical
 *             E: Error
 *             W: Warning
 *             N: Notice
 *             I: Information
 *             D: Debug
 *             T: Trace
 *
 *         <type> is one of these (optional):
 *
 *             g: generic (default value if not specified)
 *             s: stack trace (show calling stack)
 *             m: memory (report a low memory condition)
 *
 *         <repetitions> is one of these (optional):
 *
 *             all: always log the message (default if not specified)
 *             once: only log the message once
 *             rl: rate limit the logging of the message
 *
 *         <arguments> depends on the <repetitions> value, and it is currently
 *                     only defined for the 'rl' case:
 *
 *             rl: it is a comma separated list of the period to consider and
 *                 the maximum allowed messages in that period.
 *
 *         <name> is an optional text argument that will be used to calls to
 *                gf_log(). If not specified, THIS->name will be used. Note
 *                that <repetitions> must be specified if you want to define
 *                this argument.
 *
 *         Examples:
 *
 *             sys_log(E(), "Generic error message logged always");
 *             sys_log(Ws(once), "Warning message including calling stack "
 *                               "logged once");
 *             sys_log(Ig(rl, 10, 1), "Generic info message logged at most "
 *                                    "one time every 10 steps");
 *             sys_log(Em(), 256); // Generic error message indicating that it
 *                                 // is not possible to allocate 256 bytes of
 *                                 // memory.
 *             sys_log(W(all, "log"), "Warning message using 'log' as the "
 *                                    "source");
 *
 *     log<level>[<type>]([<arguments>], _fmt)
 *
 *         This is a shorthand of:
 *
 *             sys_log(<level><type>(<arguments>), _fmt)
 *
 *         Note that it is not possible to specify <name> using this format.
 *         Use nlog...() to include a <name> argument.
 *
 *         Examples:
 *
 *             // Equivalent to sys_log(E(), "Error")
 *             logE("Error");
 *
 *             // Equivalent to sys_log(Ws(once), "Warn")
 *             logWs_once("Warn");
 *
 *             // Equivalent to sys_log(Ig(rl, 10, 1), "Info")
 *             logIg_rl(10, 1, "Info");
 *
 *     nlog<level>[<type>]([<arguments>], <name>, _fmt)
 *
 *         This is a shorthand of:
 *
 *             sys_log(<level><type>(<arguments>, <name>), _fmt)
 *
 *         If <arguments> is empty, 'all' will be used.
 *
 *         Examples:
 *
 *             // Equivalent to sys_log(E(all, "test"), "Error")
 *             nlogE("test", "Error");
 *
 *             // Equivalent to sys_log(Ws(once, "test"), "Warn")
 *             nlogWs_once("test", "Warn");
 *
 *             // Equivalent to sys_log(Ig(rl, 10, 1, "test"), "Info")
 *             nlogIg_rl(10, 1, "test", "Info");
 *
 *     LOG() action
 *
 *         Action to be used in some special constructions. It must be called
 *         as:
 *
 *             LOG(_mode, _fmt...)
 *
 *         Examples:
 *
 *             SYS_CALL(function, (args), D(), LOG(W(once), "Warning"))
 */

/// Log levels mapped to GlusterFS log levels
#define LOG_U GF_LOG_NONE
#define LOG_M GF_LOG_EMERG
#define LOG_A GF_LOG_ALERT
#define LOG_C GF_LOG_CRITICAL
#define LOG_E GF_LOG_ERROR
#define LOG_W GF_LOG_WARNING
#define LOG_N GF_LOG_NOTICE
#define LOG_I GF_LOG_INFO
#define LOG_D GF_LOG_DEBUG
#define LOG_T GF_LOG_TRACE

/**
 * Compile time log level.
 *
 * Anything that uses a log level greater than this one will not be compiled.
 */
#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_D
#endif

/**
 * Compile time filtering based on static LOG_LEVEL.
 *
 * Used to conditionally include things at compile time. Aimed basically to
 * structure fields or variable declarations.
 *
 * If a ';' or any other special character is needed, it must be included
 * inside the argument. For example:
 *
 *     struct _test_struct
 *     {
 *               int32_t regular_field;
 *         ON_D( int32_t debug_field;   )
 *     };
 */

#define ON_U(_x) _x

#if LOG_LEVEL >= LOG_M
#define ON_M(_x) _x
#else
#define ON_M(_x)
#endif

#if LOG_LEVEL >= LOG_A
#define ON_A(_x) _x
#else
#define ON_A(_x)
#endif

#if LOG_LEVEL >= LOG_C
#define ON_C(_x) _x
#else
#define ON_C(_x)
#endif

#if LOG_LEVEL >= LOG_E
#define ON_E(_x) _x
#else
#define ON_E(_x)
#endif

#if LOG_LEVEL >= LOG_W
#define ON_W(_x) _x
#else
#define ON_W(_x)
#endif

#if LOG_LEVEL >= LOG_N
#define ON_N(_x) _x
#else
#define ON_N(_x)
#endif

#if LOG_LEVEL >= LOG_I
#define ON_I(_x) _x
#else
#define ON_I(_x)
#endif

#if LOG_LEVEL >= LOG_D
#define ON_D(_x) _x
#else
#define ON_D(_x)
#endif

#if LOG_LEVEL >= LOG_T
#define ON_T(_x) _x
#else
#define ON_T(_x)
#endif

/**
 * Compile time conditional block based on static LOG_LEVEL.
 *
 * Used to conditionally include things at compile time. Aimed to entire code
 * sections where the use of ON_x() is not practical or an 'else' clause is
 * needed.
 *
 * The expression is constant so the compiler can optimize it out.
 */

#define IF_U() if (1)
#define IF_M() if (LOG_LEVEL >= LOG_M)
#define IF_A() if (LOG_LEVEL >= LOG_A)
#define IF_C() if (LOG_LEVEL >= LOG_C)
#define IF_E() if (LOG_LEVEL >= LOG_E)
#define IF_W() if (LOG_LEVEL >= LOG_W)
#define IF_N() if (LOG_LEVEL >= LOG_N)
#define IF_I() if (LOG_LEVEL >= LOG_I)
#define IF_D() if (LOG_LEVEL >= LOG_D)
#define IF_T() if (LOG_LEVEL >= LOG_T)

/**
 * Basic logging types.
 *
 * These macros conditionally make a log entry based on a static log level.
 * There are 3 types of logs:
 *
 *     g: generic log entry (default)
 *     s: stack trace (includes the call stack)
 *     m: memory log entry (used for low memory reports)
 */
#define __SYS_LOG_TYPE_g(_name, _lvl, _fmt...) \
    do \
    { \
        IF_##_lvl() \
        { \
            gf_log(_name, LOG_##_lvl, _fmt); \
        } \
    } while (0)

#define __SYS_LOG_TYPE_(_name, _lvl, _fmt...) \
    __SYS_LOG_TYPE_g(_name, _lvl, ## _fmt)

#define __SYS_LOG_TYPE_s(_name, _lvl, _fmt...) \
    do \
    { \
        IF_##_lvl() \
        { \
            gf_log_callingfn(_name, LOG_##_lvl, _fmt); \
        } \
    } while (0)

#define __SYS_LOG_TYPE_m(_name, _lvl, _size) \
    do \
    { \
        IF_##_lvl() \
        { \
            gf_log_nomem(_name, LOG_##_lvl, _size); \
        } \
    } while (0)

#define __SYS_LOG_TYPE(_type, _name, _lvl, _fmt...) \
    __SYS_LOG_TYPE_##_type(_name, _lvl, ## _fmt)

/**
 * Log limiting macros.
 *
 * Determine how many times or how often a log entry can be written. There are
 * 3 possibilities:
 *
 *     all: all messages are logged (default)
 *     rl: rate limit (limits the frequency of the logged message)
 *     once: the message will be logged only once
 *     none: message is never logged
 */
#define __SYS_LOG_TIMES_all(_type, _lvl, _name, _fmt...) \
    __SYS_LOG_TYPE(_type, _name, _lvl, ## _fmt)

#define __SYS_LOG_TIMES_(_type, _lvl, _name, _fmt...) \
    __SYS_LOG_TIMES_all(_type, _name, _lvl, ## _fmt)

#define __SYS_LOG_TIMES_rl_2(_cnt, _type, _lvl, _steps, _max, _name, _fmt...) \
    do \
    { \
        if (LOG_##_lvl <= THIS->ctx->log.loglevel) \
        { \
            int32_t _cnt; \
            SYS_RL( \
                _steps, _max, &_cnt, \
                BREAK() \
            ); \
            if (_cnt > 0) \
            { \
                __SYS_LOG_TYPE_g(_name, W, \
                    "%d messages suppressed due to rate limiting", _cnt); \
            } \
            __SYS_LOG_TYPE(_type, _name, _lvl, ## _fmt); \
        } \
    } while (0)
#define __SYS_LOG_TIMES_rl_1(_args...) __SYS_LOG_TIMES_rl_2(_args)
#define __SYS_LOG_TIMES_rl(_type, _lvl, _steps, _max, _name, _fmt...) \
    __SYS_LOG_TIMES_rl_1(SYS_TMP(1), _type, _lvl, _steps, _max, _name, ## _fmt)

#define __SYS_LOG_TIMES_none(_args...) do { } while (0)

#define __SYS_LOG_TIMES_once(_type, _lvl, _name, _fmt...) \
    do \
    { \
        if (LOG_##_lvl <= THIS->ctx->log.loglevel) \
        { \
            SYS_ONCE(BREAK()); \
            __SYS_LOG_TYPE(_type, _name, _lvl, ## _fmt); \
        } \
    } while (0)

#define __SYS_LOG_TIMES_1(_rep, _type, _lvl, _args...) \
    __SYS_LOG_TIMES_##_rep(_type, _lvl, ## _args)

#define __SYS_LOG_TIMES(_args...) \
    __SYS_LOG_TIMES_1(_args)

#define __SYS_LOG_NAME_4(_a, _b, _c, _d, _e...) _a, _b, _c, _d
#define __SYS_LOG_NAME_6(_a, _b, _c, _d, _e, _f, _g...) _a, _b, _c, _d, _e, _f

#define __SYS_LOG_NAME (THIS)->name

#define __SYS_LOG_NAME_all(_type, _lvl, _name...) \
    __SYS_LOG_NAME_4(all, _type, _lvl, ## _name, __SYS_LOG_NAME)
#define __SYS_LOG_NAME_(_type, _lvl, _name...) \
    __SYS_LOG_NAME_4(all, _type, _lvl, ## _name, __SYS_LOG_NAME)
#define __SYS_LOG_NAME_rl(_type, _lvl, _steps, _max, _name...) \
    __SYS_LOG_NAME_6(rl, _type, _lvl, _steps, _max, ## _name, __SYS_LOG_NAME)
#define __SYS_LOG_NAME_once(_type, _lvl, _name...) \
    __SYS_LOG_NAME_4(once, _type, _lvl, ##_name, __SYS_LOG_NAME)
#define __SYS_LOG_NAME_none(_type, _lvl, _name...) \
    __SYS_LOG_NAME_4(none, _type, _lvl, ##_name, __SYS_LOG_NAME)

/**
 * Logging mode decoding.
 *
 * Macros to decode an encoded logging mode.
 */
#define __SYS_LOG_MODE_U(_rep, _args...) __SYS_LOG_NAME_##_rep(g, U, ## _args)
#define __SYS_LOG_MODE_M(_rep, _args...) __SYS_LOG_NAME_##_rep(g, M, ## _args)
#define __SYS_LOG_MODE_A(_rep, _args...) __SYS_LOG_NAME_##_rep(g, A, ## _args)
#define __SYS_LOG_MODE_C(_rep, _args...) __SYS_LOG_NAME_##_rep(g, C, ## _args)
#define __SYS_LOG_MODE_E(_rep, _args...) __SYS_LOG_NAME_##_rep(g, E, ## _args)
#define __SYS_LOG_MODE_W(_rep, _args...) __SYS_LOG_NAME_##_rep(g, W, ## _args)
#define __SYS_LOG_MODE_N(_rep, _args...) __SYS_LOG_NAME_##_rep(g, N, ## _args)
#define __SYS_LOG_MODE_I(_rep, _args...) __SYS_LOG_NAME_##_rep(g, I, ## _args)
#define __SYS_LOG_MODE_D(_rep, _args...) __SYS_LOG_NAME_##_rep(g, D, ## _args)
#define __SYS_LOG_MODE_T(_rep, _args...) __SYS_LOG_NAME_##_rep(g, T, ## _args)

#define __SYS_LOG_MODE_Ug(_rep, _args...) __SYS_LOG_NAME_##_rep(g, U, ## _args)
#define __SYS_LOG_MODE_Mg(_rep, _args...) __SYS_LOG_NAME_##_rep(g, M, ## _args)
#define __SYS_LOG_MODE_Ag(_rep, _args...) __SYS_LOG_NAME_##_rep(g, A, ## _args)
#define __SYS_LOG_MODE_Cg(_rep, _args...) __SYS_LOG_NAME_##_rep(g, C, ## _args)
#define __SYS_LOG_MODE_Eg(_rep, _args...) __SYS_LOG_NAME_##_rep(g, E, ## _args)
#define __SYS_LOG_MODE_Wg(_rep, _args...) __SYS_LOG_NAME_##_rep(g, W, ## _args)
#define __SYS_LOG_MODE_Ng(_rep, _args...) __SYS_LOG_NAME_##_rep(g, N, ## _args)
#define __SYS_LOG_MODE_Ig(_rep, _args...) __SYS_LOG_NAME_##_rep(g, I, ## _args)
#define __SYS_LOG_MODE_Dg(_rep, _args...) __SYS_LOG_NAME_##_rep(g, D, ## _args)
#define __SYS_LOG_MODE_Tg(_rep, _args...) __SYS_LOG_NAME_##_rep(g, T, ## _args)

#define __SYS_LOG_MODE_Us(_rep, _args...) __SYS_LOG_NAME_##_rep(s, U, ## _args)
#define __SYS_LOG_MODE_Ms(_rep, _args...) __SYS_LOG_NAME_##_rep(s, M, ## _args)
#define __SYS_LOG_MODE_As(_rep, _args...) __SYS_LOG_NAME_##_rep(s, A, ## _args)
#define __SYS_LOG_MODE_Cs(_rep, _args...) __SYS_LOG_NAME_##_rep(s, C, ## _args)
#define __SYS_LOG_MODE_Es(_rep, _args...) __SYS_LOG_NAME_##_rep(s, E, ## _args)
#define __SYS_LOG_MODE_Ws(_rep, _args...) __SYS_LOG_NAME_##_rep(s, W, ## _args)
#define __SYS_LOG_MODE_Ns(_rep, _args...) __SYS_LOG_NAME_##_rep(s, N, ## _args)
#define __SYS_LOG_MODE_Is(_rep, _args...) __SYS_LOG_NAME_##_rep(s, I, ## _args)
#define __SYS_LOG_MODE_Ds(_rep, _args...) __SYS_LOG_NAME_##_rep(s, D, ## _args)
#define __SYS_LOG_MODE_Ts(_rep, _args...) __SYS_LOG_NAME_##_rep(s, T, ## _args)

#define __SYS_LOG_MODE_Um(_rep, _args...) __SYS_LOG_NAME_##_rep(m, U, ## _args)
#define __SYS_LOG_MODE_Mm(_rep, _args...) __SYS_LOG_NAME_##_rep(m, M, ## _args)
#define __SYS_LOG_MODE_Am(_rep, _args...) __SYS_LOG_NAME_##_rep(m, A, ## _args)
#define __SYS_LOG_MODE_Cm(_rep, _args...) __SYS_LOG_NAME_##_rep(m, C, ## _args)
#define __SYS_LOG_MODE_Em(_rep, _args...) __SYS_LOG_NAME_##_rep(m, E, ## _args)
#define __SYS_LOG_MODE_Wm(_rep, _args...) __SYS_LOG_NAME_##_rep(m, W, ## _args)
#define __SYS_LOG_MODE_Nm(_rep, _args...) __SYS_LOG_NAME_##_rep(m, N, ## _args)
#define __SYS_LOG_MODE_Im(_rep, _args...) __SYS_LOG_NAME_##_rep(m, I, ## _args)
#define __SYS_LOG_MODE_Dm(_rep, _args...) __SYS_LOG_NAME_##_rep(m, D, ## _args)
#define __SYS_LOG_MODE_Tm(_rep, _args...) __SYS_LOG_NAME_##_rep(m, T, ## _args)

/**
 * Generic log macro.
 *
 * It logs a message using the specified mode.
 */
#define sys_log(_mode, _fmt...) \
    __SYS_LOG_TIMES(__SYS_LOG_MODE_##_mode, ## _fmt)

/**
 * Easy to use logging macros.
 *
 * Convenient macros to do logging.
 */
#define logU(_fmt...) sys_log(U(all), ## _fmt)
#define logM(_fmt...) sys_log(M(all), ## _fmt)
#define logA(_fmt...) sys_log(A(all), ## _fmt)
#define logC(_fmt...) sys_log(C(all), ## _fmt)
#define logE(_fmt...) sys_log(E(all), ## _fmt)
#define logW(_fmt...) sys_log(W(all), ## _fmt)
#define logN(_fmt...) sys_log(N(all), ## _fmt)
#define logI(_fmt...) sys_log(I(all), ## _fmt)
#define logD(_fmt...) sys_log(D(all), ## _fmt)
#define logT(_fmt...) sys_log(T(all), ## _fmt)

#define logUg(_fmt...) sys_log(Ug(all), ## _fmt)
#define logMg(_fmt...) sys_log(Mg(all), ## _fmt)
#define logAg(_fmt...) sys_log(Ag(all), ## _fmt)
#define logCg(_fmt...) sys_log(Cg(all), ## _fmt)
#define logEg(_fmt...) sys_log(Eg(all), ## _fmt)
#define logWg(_fmt...) sys_log(Wg(all), ## _fmt)
#define logNg(_fmt...) sys_log(Ng(all), ## _fmt)
#define logIg(_fmt...) sys_log(Ig(all), ## _fmt)
#define logDg(_fmt...) sys_log(Dg(all), ## _fmt)
#define logTg(_fmt...) sys_log(Tg(all), ## _fmt)

#define logUs(_fmt...) sys_log(Us(all), ## _fmt)
#define logMs(_fmt...) sys_log(Ms(all), ## _fmt)
#define logAs(_fmt...) sys_log(As(all), ## _fmt)
#define logCs(_fmt...) sys_log(Cs(all), ## _fmt)
#define logEs(_fmt...) sys_log(Es(all), ## _fmt)
#define logWs(_fmt...) sys_log(Ws(all), ## _fmt)
#define logNs(_fmt...) sys_log(Ns(all), ## _fmt)
#define logIs(_fmt...) sys_log(Is(all), ## _fmt)
#define logDs(_fmt...) sys_log(Ds(all), ## _fmt)
#define logTs(_fmt...) sys_log(Ts(all), ## _fmt)

#define logUm(_size) sys_log(Um(all), _size)
#define logMm(_size) sys_log(Mm(all), _size)
#define logAm(_size) sys_log(Am(all), _size)
#define logCm(_size) sys_log(Cm(all), _size)
#define logEm(_size) sys_log(Em(all), _size)
#define logWm(_size) sys_log(Wm(all), _size)
#define logNm(_size) sys_log(Nm(all), _size)
#define logIm(_size) sys_log(Im(all), _size)
#define logDm(_size) sys_log(Dm(all), _size)
#define logTm(_size) sys_log(Tm(all), _size)

#define logU_once(_fmt...) sys_log(U(once), ## _fmt)
#define logM_once(_fmt...) sys_log(M(once), ## _fmt)
#define logA_once(_fmt...) sys_log(A(once), ## _fmt)
#define logC_once(_fmt...) sys_log(C(once), ## _fmt)
#define logE_once(_fmt...) sys_log(E(once), ## _fmt)
#define logW_once(_fmt...) sys_log(W(once), ## _fmt)
#define logN_once(_fmt...) sys_log(N(once), ## _fmt)
#define logI_once(_fmt...) sys_log(I(once), ## _fmt)
#define logD_once(_fmt...) sys_log(D(once), ## _fmt)
#define logT_once(_fmt...) sys_log(T(once), ## _fmt)

#define logUg_once(_fmt...) sys_log(Ug(once), ## _fmt)
#define logMg_once(_fmt...) sys_log(Mg(once), ## _fmt)
#define logAg_once(_fmt...) sys_log(Ag(once), ## _fmt)
#define logCg_once(_fmt...) sys_log(Cg(once), ## _fmt)
#define logEg_once(_fmt...) sys_log(Eg(once), ## _fmt)
#define logWg_once(_fmt...) sys_log(Wg(once), ## _fmt)
#define logNg_once(_fmt...) sys_log(Ng(once), ## _fmt)
#define logIg_once(_fmt...) sys_log(Ig(once), ## _fmt)
#define logDg_once(_fmt...) sys_log(Dg(once), ## _fmt)
#define logTg_once(_fmt...) sys_log(Tg(once), ## _fmt)

#define logUs_once(_fmt...) sys_log(Us(once), ## _fmt)
#define logMs_once(_fmt...) sys_log(Ms(once), ## _fmt)
#define logAs_once(_fmt...) sys_log(As(once), ## _fmt)
#define logCs_once(_fmt...) sys_log(Cs(once), ## _fmt)
#define logEs_once(_fmt...) sys_log(Es(once), ## _fmt)
#define logWs_once(_fmt...) sys_log(Ws(once), ## _fmt)
#define logNs_once(_fmt...) sys_log(Ns(once), ## _fmt)
#define logIs_once(_fmt...) sys_log(Is(once), ## _fmt)
#define logDs_once(_fmt...) sys_log(Ds(once), ## _fmt)
#define logTs_once(_fmt...) sys_log(Ts(once), ## _fmt)

#define logUm_once(_size) sys_log(Um(once), _size)
#define logMm_once(_size) sys_log(Mm(once), _size)
#define logAm_once(_size) sys_log(Am(once), _size)
#define logCm_once(_size) sys_log(Cm(once), _size)
#define logEm_once(_size) sys_log(Em(once), _size)
#define logWm_once(_size) sys_log(Wm(once), _size)
#define logNm_once(_size) sys_log(Nm(once), _size)
#define logIm_once(_size) sys_log(Im(once), _size)
#define logDm_once(_size) sys_log(Dm(once), _size)
#define logTm_once(_size) sys_log(Tm(once), _size)

#define logU_rl(_steps, _max, _fmt...) sys_log(U(rl, _steps, _max), ## _fmt)
#define logM_rl(_steps, _max, _fmt...) sys_log(M(rl, _steps, _max), ## _fmt)
#define logA_rl(_steps, _max, _fmt...) sys_log(A(rl, _steps, _max), ## _fmt)
#define logC_rl(_steps, _max, _fmt...) sys_log(C(rl, _steps, _max), ## _fmt)
#define logE_rl(_steps, _max, _fmt...) sys_log(E(rl, _steps, _max), ## _fmt)
#define logW_rl(_steps, _max, _fmt...) sys_log(W(rl, _steps, _max), ## _fmt)
#define logN_rl(_steps, _max, _fmt...) sys_log(N(rl, _steps, _max), ## _fmt)
#define logI_rl(_steps, _max, _fmt...) sys_log(I(rl, _steps, _max), ## _fmt)
#define logD_rl(_steps, _max, _fmt...) sys_log(D(rl, _steps, _max), ## _fmt)
#define logT_rl(_steps, _max, _fmt...) sys_log(T(rl, _steps, _max), ## _fmt)

#define logUg_rl(_steps, _max, _fmt...) sys_log(Ug(rl, _steps, _max), ## _fmt)
#define logMg_rl(_steps, _max, _fmt...) sys_log(Mg(rl, _steps, _max), ## _fmt)
#define logAg_rl(_steps, _max, _fmt...) sys_log(Ag(rl, _steps, _max), ## _fmt)
#define logCg_rl(_steps, _max, _fmt...) sys_log(Cg(rl, _steps, _max), ## _fmt)
#define logEg_rl(_steps, _max, _fmt...) sys_log(Eg(rl, _steps, _max), ## _fmt)
#define logWg_rl(_steps, _max, _fmt...) sys_log(Wg(rl, _steps, _max), ## _fmt)
#define logNg_rl(_steps, _max, _fmt...) sys_log(Ng(rl, _steps, _max), ## _fmt)
#define logIg_rl(_steps, _max, _fmt...) sys_log(Ig(rl, _steps, _max), ## _fmt)
#define logDg_rl(_steps, _max, _fmt...) sys_log(Dg(rl, _steps, _max), ## _fmt)
#define logTg_rl(_steps, _max, _fmt...) sys_log(Tg(rl, _steps, _max), ## _fmt)

#define logUs_rl(_steps, _max, _fmt...) sys_log(Us(rl, _steps, _max), ## _fmt)
#define logMs_rl(_steps, _max, _fmt...) sys_log(Ms(rl, _steps, _max), ## _fmt)
#define logAs_rl(_steps, _max, _fmt...) sys_log(As(rl, _steps, _max), ## _fmt)
#define logCs_rl(_steps, _max, _fmt...) sys_log(Cs(rl, _steps, _max), ## _fmt)
#define logEs_rl(_steps, _max, _fmt...) sys_log(Es(rl, _steps, _max), ## _fmt)
#define logWs_rl(_steps, _max, _fmt...) sys_log(Ws(rl, _steps, _max), ## _fmt)
#define logNs_rl(_steps, _max, _fmt...) sys_log(Ns(rl, _steps, _max), ## _fmt)
#define logIs_rl(_steps, _max, _fmt...) sys_log(Is(rl, _steps, _max), ## _fmt)
#define logDs_rl(_steps, _max, _fmt...) sys_log(Ds(rl, _steps, _max), ## _fmt)
#define logTs_rl(_steps, _max, _fmt...) sys_log(Ts(rl, _steps, _max), ## _fmt)

#define logUm_rl(_steps, _max, _size) sys_log(Um(rl, _steps, _max), _size)
#define logMm_rl(_steps, _max, _size) sys_log(Mm(rl, _steps, _max), _size)
#define logAm_rl(_steps, _max, _size) sys_log(Am(rl, _steps, _max), _size)
#define logCm_rl(_steps, _max, _size) sys_log(Cm(rl, _steps, _max), _size)
#define logEm_rl(_steps, _max, _size) sys_log(Em(rl, _steps, _max), _size)
#define logWm_rl(_steps, _max, _size) sys_log(Wm(rl, _steps, _max), _size)
#define logNm_rl(_steps, _max, _size) sys_log(Nm(rl, _steps, _max), _size)
#define logIm_rl(_steps, _max, _size) sys_log(Im(rl, _steps, _max), _size)
#define logDm_rl(_steps, _max, _size) sys_log(Dm(rl, _steps, _max), _size)
#define logTm_rl(_steps, _max, _size) sys_log(Tm(rl, _steps, _max), _size)

#define nlogU(_name, _fmt...) sys_log(U(all, _name), ## _fmt)
#define nlogM(_name, _fmt...) sys_log(M(all, _name), ## _fmt)
#define nlogA(_name, _fmt...) sys_log(A(all, _name), ## _fmt)
#define nlogC(_name, _fmt...) sys_log(C(all, _name), ## _fmt)
#define nlogE(_name, _fmt...) sys_log(E(all, _name), ## _fmt)
#define nlogW(_name, _fmt...) sys_log(W(all, _name), ## _fmt)
#define nlogN(_name, _fmt...) sys_log(N(all, _name), ## _fmt)
#define nlogI(_name, _fmt...) sys_log(I(all, _name), ## _fmt)
#define nlogD(_name, _fmt...) sys_log(D(all, _name), ## _fmt)
#define nlogT(_name, _fmt...) sys_log(T(all, _name), ## _fmt)

#define nlogUg(_name, _fmt...) sys_log(Ug(all, _name), ## _fmt)
#define nlogMg(_name, _fmt...) sys_log(Mg(all, _name), ## _fmt)
#define nlogAg(_name, _fmt...) sys_log(Ag(all, _name), ## _fmt)
#define nlogCg(_name, _fmt...) sys_log(Cg(all, _name), ## _fmt)
#define nlogEg(_name, _fmt...) sys_log(Eg(all, _name), ## _fmt)
#define nlogWg(_name, _fmt...) sys_log(Wg(all, _name), ## _fmt)
#define nlogNg(_name, _fmt...) sys_log(Ng(all, _name), ## _fmt)
#define nlogIg(_name, _fmt...) sys_log(Ig(all, _name), ## _fmt)
#define nlogDg(_name, _fmt...) sys_log(Dg(all, _name), ## _fmt)
#define nlogTg(_name, _fmt...) sys_log(Tg(all, _name), ## _fmt)

#define nlogUs(_name, _fmt...) sys_log(Us(all, _name), ## _fmt)
#define nlogMs(_name, _fmt...) sys_log(Ms(all, _name), ## _fmt)
#define nlogAs(_name, _fmt...) sys_log(As(all, _name), ## _fmt)
#define nlogCs(_name, _fmt...) sys_log(Cs(all, _name), ## _fmt)
#define nlogEs(_name, _fmt...) sys_log(Es(all, _name), ## _fmt)
#define nlogWs(_name, _fmt...) sys_log(Ws(all, _name), ## _fmt)
#define nlogNs(_name, _fmt...) sys_log(Ns(all, _name), ## _fmt)
#define nlogIs(_name, _fmt...) sys_log(Is(all, _name), ## _fmt)
#define nlogDs(_name, _fmt...) sys_log(Ds(all, _name), ## _fmt)
#define nlogTs(_name, _fmt...) sys_log(Ts(all, _name), ## _fmt)

#define nlogUm(_name, _size) sys_log(Um(all, _name), _size)
#define nlogMm(_name, _size) sys_log(Mm(all, _name), _size)
#define nlogAm(_name, _size) sys_log(Am(all, _name), _size)
#define nlogCm(_name, _size) sys_log(Cm(all, _name), _size)
#define nlogEm(_name, _size) sys_log(Em(all, _name), _size)
#define nlogWm(_name, _size) sys_log(Wm(all, _name), _size)
#define nlogNm(_name, _size) sys_log(Nm(all, _name), _size)
#define nlogIm(_name, _size) sys_log(Im(all, _name), _size)
#define nlogDm(_name, _size) sys_log(Dm(all, _name), _size)
#define nlogTm(_name, _size) sys_log(Tm(all, _name), _size)

#define nlogU_once(_name, _fmt...) sys_log(U(once, _name), ## _fmt)
#define nlogM_once(_name, _fmt...) sys_log(M(once, _name), ## _fmt)
#define nlogA_once(_name, _fmt...) sys_log(A(once, _name), ## _fmt)
#define nlogC_once(_name, _fmt...) sys_log(C(once, _name), ## _fmt)
#define nlogE_once(_name, _fmt...) sys_log(E(once, _name), ## _fmt)
#define nlogW_once(_name, _fmt...) sys_log(W(once, _name), ## _fmt)
#define nlogN_once(_name, _fmt...) sys_log(N(once, _name), ## _fmt)
#define nlogI_once(_name, _fmt...) sys_log(I(once, _name), ## _fmt)
#define nlogD_once(_name, _fmt...) sys_log(D(once, _name), ## _fmt)
#define nlogT_once(_name, _fmt...) sys_log(T(once, _name), ## _fmt)

#define nlogUg_once(_name, _fmt...) sys_log(Ug(once, _name), ## _fmt)
#define nlogMg_once(_name, _fmt...) sys_log(Mg(once, _name), ## _fmt)
#define nlogAg_once(_name, _fmt...) sys_log(Ag(once, _name), ## _fmt)
#define nlogCg_once(_name, _fmt...) sys_log(Cg(once, _name), ## _fmt)
#define nlogEg_once(_name, _fmt...) sys_log(Eg(once, _name), ## _fmt)
#define nlogWg_once(_name, _fmt...) sys_log(Wg(once, _name), ## _fmt)
#define nlogNg_once(_name, _fmt...) sys_log(Ng(once, _name), ## _fmt)
#define nlogIg_once(_name, _fmt...) sys_log(Ig(once, _name), ## _fmt)
#define nlogDg_once(_name, _fmt...) sys_log(Dg(once, _name), ## _fmt)
#define nlogTg_once(_name, _fmt...) sys_log(Tg(once, _name), ## _fmt)

#define nlogUs_once(_name, _fmt...) sys_log(Us(once, _name), ## _fmt)
#define nlogMs_once(_name, _fmt...) sys_log(Ms(once, _name), ## _fmt)
#define nlogAs_once(_name, _fmt...) sys_log(As(once, _name), ## _fmt)
#define nlogCs_once(_name, _fmt...) sys_log(Cs(once, _name), ## _fmt)
#define nlogEs_once(_name, _fmt...) sys_log(Es(once, _name), ## _fmt)
#define nlogWs_once(_name, _fmt...) sys_log(Ws(once, _name), ## _fmt)
#define nlogNs_once(_name, _fmt...) sys_log(Ns(once, _name), ## _fmt)
#define nlogIs_once(_name, _fmt...) sys_log(Is(once, _name), ## _fmt)
#define nlogDs_once(_name, _fmt...) sys_log(Ds(once, _name), ## _fmt)
#define nlogTs_once(_name, _fmt...) sys_log(Ts(once, _name), ## _fmt)

#define nlogUm_once(_name, _size) sys_log(Um(once, _name), _size)
#define nlogMm_once(_name, _size) sys_log(Mm(once, _name), _size)
#define nlogAm_once(_name, _size) sys_log(Am(once, _name), _size)
#define nlogCm_once(_name, _size) sys_log(Cm(once, _name), _size)
#define nlogEm_once(_name, _size) sys_log(Em(once, _name), _size)
#define nlogWm_once(_name, _size) sys_log(Wm(once, _name), _size)
#define nlogNm_once(_name, _size) sys_log(Nm(once, _name), _size)
#define nlogIm_once(_name, _size) sys_log(Im(once, _name), _size)
#define nlogDm_once(_name, _size) sys_log(Dm(once, _name), _size)
#define nlogTm_once(_name, _size) sys_log(Tm(once, _name), _size)

#define nlogU_rl(_steps, _max, _name, _fmt...) \
    sys_log(U(rl, _steps, _max, _name), ## _fmt)
#define nlogM_rl(_steps, _max, _name, _fmt...) \
    sys_log(M(rl, _steps, _max, _name), ## _fmt)
#define nlogA_rl(_steps, _max, _name, _fmt...) \
    sys_log(A(rl, _steps, _max, _name), ## _fmt)
#define nlogC_rl(_steps, _max, _name, _fmt...) \
    sys_log(C(rl, _steps, _max, _name), ## _fmt)
#define nlogE_rl(_steps, _max, _name, _fmt...) \
    sys_log(E(rl, _steps, _max, _name), ## _fmt)
#define nlogW_rl(_steps, _max, _name, _fmt...) \
    sys_log(W(rl, _steps, _max, _name), ## _fmt)
#define nlogN_rl(_steps, _max, _name, _fmt...) \
    sys_log(N(rl, _steps, _max, _name), ## _fmt)
#define nlogI_rl(_steps, _max, _name, _fmt...) \
    sys_log(I(rl, _steps, _max, _name), ## _fmt)
#define nlogD_rl(_steps, _max, _name, _fmt...) \
    sys_log(D(rl, _steps, _max, _name), ## _fmt)
#define nlogT_rl(_steps, _max, _name, _fmt...) \
    sys_log(T(rl, _steps, _max, _name), ## _fmt)

#define nlogUg_rl(_steps, _max, _name, _fmt...) \
    sys_log(Ug(rl, _steps, _max, _name), ## _fmt)
#define nlogMg_rl(_steps, _max, _name, _fmt...) \
    sys_log(Mg(rl, _steps, _max, _name), ## _fmt)
#define nlogAg_rl(_steps, _max, _name, _fmt...) \
    sys_log(Ag(rl, _steps, _max, _name), ## _fmt)
#define nlogCg_rl(_steps, _max, _name, _fmt...) \
    sys_log(Cg(rl, _steps, _max, _name), ## _fmt)
#define nlogEg_rl(_steps, _max, _name, _fmt...) \
    sys_log(Eg(rl, _steps, _max, _name), ## _fmt)
#define nlogWg_rl(_steps, _max, _name, _fmt...) \
    sys_log(Wg(rl, _steps, _max, _name), ## _fmt)
#define nlogNg_rl(_steps, _max, _name, _fmt...) \
    sys_log(Ng(rl, _steps, _max, _name), ## _fmt)
#define nlogIg_rl(_steps, _max, _name, _fmt...) \
    sys_log(Ig(rl, _steps, _max, _name), ## _fmt)
#define nlogDg_rl(_steps, _max, _name, _fmt...) \
    sys_log(Dg(rl, _steps, _max, _name), ## _fmt)
#define nlogTg_rl(_steps, _max, _name, _fmt...) \
    sys_log(Tg(rl, _steps, _max, _name), ## _fmt)

#define nlogUs_rl(_steps, _max, _name, _fmt...) \
    sys_log(Us(rl, _steps, _max, _name), ## _fmt)
#define nlogMs_rl(_steps, _max, _name, _fmt...) \
    sys_log(Ms(rl, _steps, _max, _name), ## _fmt)
#define nlogAs_rl(_steps, _max, _name, _fmt...) \
    sys_log(As(rl, _steps, _max, _name), ## _fmt)
#define nlogCs_rl(_steps, _max, _name, _fmt...) \
    sys_log(Cs(rl, _steps, _max, _name), ## _fmt)
#define nlogEs_rl(_steps, _max, _name, _fmt...) \
    sys_log(Es(rl, _steps, _max, _name), ## _fmt)
#define nlogWs_rl(_steps, _max, _name, _fmt...) \
    sys_log(Ws(rl, _steps, _max, _name), ## _fmt)
#define nlogNs_rl(_steps, _max, _name, _fmt...) \
    sys_log(Ns(rl, _steps, _max, _name), ## _fmt)
#define nlogIs_rl(_steps, _max, _name, _fmt...) \
    sys_log(Is(rl, _steps, _max, _name), ## _fmt)
#define nlogDs_rl(_steps, _max, _name, _fmt...) \
    sys_log(Ds(rl, _steps, _max, _name), ## _fmt)
#define nlogTs_rl(_steps, _max, _name, _fmt...) \
    sys_log(Ts(rl, _steps, _max, _name), ## _fmt)

#define nlogUm_rl(_steps, _max, _size) \
    sys_log(Um(rl, _steps, _max, _name), _size)
#define nlogMm_rl(_steps, _max, _size) \
    sys_log(Mm(rl, _steps, _max, _name), _size)
#define nlogAm_rl(_steps, _max, _size) \
    sys_log(Am(rl, _steps, _max, _name), _size)
#define nlogCm_rl(_steps, _max, _size) \
    sys_log(Cm(rl, _steps, _max, _name), _size)
#define nlogEm_rl(_steps, _max, _size) \
    sys_log(Em(rl, _steps, _max, _name), _size)
#define nlogWm_rl(_steps, _max, _size) \
    sys_log(Wm(rl, _steps, _max, _name), _size)
#define nlogNm_rl(_steps, _max, _size) \
    sys_log(Nm(rl, _steps, _max, _name), _size)
#define nlogIm_rl(_steps, _max, _size) \
    sys_log(Im(rl, _steps, _max, _name), _size)
#define nlogDm_rl(_steps, _max, _size) \
    sys_log(Dm(rl, _steps, _max, _name), _size)
#define nlogTm_rl(_steps, _max, _size) \
    sys_log(Tm(rl, _steps, _max, _name), _size)

/**
 * Macro to implement LOG() action.
 *
 * This macro is used in conjunction with actions to define the LOG() action.
 */
#define __SYS_LOG(_err, _mode, _msg...) sys_log(_mode, ## _msg)
#define __SYS_ACTION_LOG(_mode, _msg...) \
    __SYS_LOG, _mode, ## _msg

#endif /* __SYS_LOG_H__ */
