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

#ifndef __SYS_ERROR_H__
#define __SYS_ERROR_H__

#include <errno.h>

/* NOTE: These macros seem ugly, but they are very useful to write less, more
 *       compact, more clean and easy to follow code.
 */

typedef int32_t err_t;

/**
 * Macro to report an irrecoverable error and stop the program immediately.
 *
 * @param _fmt message and optional arguments to log (printf-like).
 */
#define sys_panic(_fmt...) \
    do \
    { \
        logMs(_fmt); \
        abort(); \
    } while (0)

/**
 * Internal macro to make sure that an error is not 0.
 *
 * When an error condition involving an errno call is detected, the errno
 * variable should have an error code != 0. If that is not the case, this
 * macro will return EDOM and log a message.
 *
 * @param _err  private local variable to store the computed error code.
 * @param _code error code to check.
 *
 * @return _code if it is not 0, EDOM otherwise.
 *
 * @see SYS_SAFE_ERROR
 */
#define __SYS_SAFE_ERROR(_err, _code) \
    ({ \
        err_t _err = (_code); \
        if (unlikely(_err == 0)) \
        { \
            logE("Detected error with no error code. Returning EDOM"); \
            _err = EDOM; \
        } \
        _err; \
    })

/**
 * Internal macro to test a condition.
 *
 * It checks if _test is true. If not, it logs a message using the _log mode
 * specified, executes actions _acts and returns SYS_SAFE_ERROR(_code).
 *
 * @param _res  private local variable to store the returning code.
 * @param _test condition to check.
 * @param _code error code to return if _test fails.
 * @param _log  log mode to use if _test fails.
 * @param _acts list of actions to execute if _test fails.
 *
 * @return 0 if _test is true. SYS_SAFE_ERROR(_code) otherwise.
 *
 * @see SYS_TEST
 * @see SYS_SAFE_ERROR
 */
#define __SYS_TEST(_res, _test, _code, _log, _acts...) \
    ({ \
        err_t _res = !(_test); \
        if (unlikely(_res != 0)) \
        { \
            _res = SYS_SAFE_ERROR(_code); \
            sys_log(_log, "Test failed (" #_test ")"); \
            SYS_ACTIONS_DO(_res, _acts); \
        } \
        else \
        { \
            logT("Test succeeded (" #_test ")"); \
        } \
        _res; \
    })

/**
 * Internal macro to execute a block of code if _err is 0.
 *
 * The code in _block is only executed if *_pcode is 0. Otherwise it returns
 * *_pcode.
 *
 * @param _err   private local variable to store the computed _pcode address.
 * @param _pcode pointer to current error code. It is updated.
 * @param _block code to execute. Must return an error code.
 *
 * @return *_pcode if it is different than 0. Otherwise the returned value
 *         from _block.
 *
 * @see SYS_CHAIN
 */
#define __SYS_CHAIN(_err, _pcode, _block) \
    ({ \
        err_t * _err = (_pcode); \
        if (unlikely(*_err != 0)) \
        { \
            logT("Skipping block due to a previous error (%d)", *_err); \
        } \
        else \
        { \
            *_err = (_block); \
        } \
        *_err; \
    })

/**
 * Internal macro to handle a generic function call.
 *
 * It calls the function _func with arguments _args and verifies that the
 * returned value satisfies the condition _check. If it passes the test, the
 * macro finishes returning 0. Otherwise it logs a message using the _log
 * mode specified, executes actions _acts and returns SYS_SAFE_ERROR(_code).
 *
 * @param _err   private local variable to store the returning error code.
 * @param _res   private local variable to store the computed _pres address.
 * @param _pres  pointer to the variable that will contain the result.
 * @param _func  function to call.
 * @param _args  arguments to pass to the function _func.
 * @param _check condition that must satisfy the returned value.
 * @param _code  error code to return if _check fails.
 * @param _log   log mode to use if _check_fails.
 * @param _acts  list of actions to execute if _check fails.
 *
 * @return 0 if _check passes. SYS_SAFE_ERROR(_code) otherwise.
 *
 * @see SYS_FUNC
 * @see SYS_SAFE_ERROR
 */
#define __SYS_FUNC(_err, _res, _pres, _func, _args, _check, _code, _log, \
                _acts...) \
    ({ \
        err_t _err = 0; \
        typeof(_pres) _res = (_pres); \
        *_res = _func _args; \
        if (unlikely(!(_check))) \
        { \
            _err = SYS_SAFE_ERROR(_code); \
            sys_log(_log, #_func "() failed. Returned %ld, error %d", \
                    (intptr_t)*_res, _err); \
            SYS_ACTIONS_DO(_err, _acts); \
        } \
        else \
        { \
            logT(#_func "() returned %ld", (intptr_t)*_res); \
        } \
        _err; \
    })

/**
 * Internal macro to handle a function call returning an error code.
 *
 * It calls the function _func with arguments _args and verifies that the
 * returned value is 0. If it is, the macro finishes returning 0. Otherwise it
 * logs a message using the _log mode specified, executes actions _acts and
 * returns the error from _func.
 *
 * @param _err  private local variable to store the returning error code.
 * @param _func function to call.
 * @param _args arguments to pass to the function _func.
 * @param _log  log mode to use if _func fails.
 * @param _acts list of actions to execute if _func fails.
 *
 * @return the result of the function call.
 *
 * @see SYS_CALL
 * @see SYS_FUNC
 */
#define __SYS_CALL(_err, _func, _args, _log, _acts...) \
    ({ \
        err_t _err; \
        SYS_FUNC(&_err, _func, _args, _err == 0, _err, _log, ## _acts); \
    })

/**
 * Internal macro to return a specific error if a function call fails.
 *
 * It calls the function _func with arguments _args and verifies that the
 * returned value is 0. If it is, the macro finishes returning 0. Otherwise it
 * logs a message using the _log mode specified, executes actions _acts and
 * returns _code.
 *
 * @param _err  private local variable to store the returning error code.
 * @param _func function to call.
 * @param _args arguments to pass to the function _func.
 * @param _code error code to return if _func fails.
 * @param _log  log mode to use if _func fails.
 * @param _acts list of actions to execute if _func fails.
 *
 * @return 0 if _func succeeds. SYS_SAFE_ERROR(_code) otherwise.
 *
 * @see SYS_CODE
 * @see SYS_FUNC
 */
#define __SYS_CODE(_err, _func, _args, _code, _log, _acts...) \
    ({ \
        err_t _err; \
        SYS_FUNC(&_err, _func, _args, _err == 0, _code, _log, ## _acts); \
    })

/**
 * Internal macro to handle a function call returning a value or an errno code.
 *
 * It calls the function _func with arguments _args and verifies that the
 * returned value is not an error. If it succeeds, the macro finishes returning
 * 0. Otherwise it logs a message using the _log mode specified, executes
 * actions _acts and returns errno.
 *
 * @param _res  private local variable to store the computed _pres address.
 * @param _pres pointer to the variable that will contain the result.
 * @param _func function to call.
 * @param _args arguments to pass to the function _func.
 * @param _log  log mode to use if _func fails.
 * @param _acts list of actions to execute if _func fails.
 *
 * @return 0 if _func succeeds. SYS_SAFE_ERROR(errno) otherwise.
 *
 * @see SYS_ERRNO
 * @see SYS_FUNC
 */
#define __SYS_ERRNO(_res, _pres, _func, _args, _log, _acts...) \
    ({ \
        err_t * _res = (_pres); \
        SYS_FUNC(_res, _func, _args, *_res >= 0, errno, _log, ## _acts); \
    })

/**
 * Internal macro to handle a function call returning 0 or an errno code.
 *
 * It calls the function _func with arguments _args and verifies that the
 * returned value is 0. If it is, the macro finishes returning 0. Otherwise it
 * logs a message using the _log mode specified, executes actions _acts and
 * returns errno.
 *
 * @param _res  private local variable to store the result.
 * @param _func function to call.
 * @param _args arguments to pass to the function _func.
 * @param _log  log mode to use if _func fails.
 * @param _acts list of actions to execute if _func fails.
 *
 * @return 0 if _func succeeds. SYS_SAFE_ERROR(errno) otherwise.
 *
 * @see SYS_ERRNO0
 * @see SYS_FUNC
 */
#define __SYS_ERRNO0(_res, _func, _args, _log, _acts...) \
    ({ \
        err_t _res; \
        SYS_FUNC(&_res, _func, _args, _res == 0, errno, _log, ## _acts); \
    })

/**
 * Internal macro to handle a function call returning a pointer.
 *
 * It calls the function _func with arguments _args and verifies that the
 * returned value is not NULL. If it succeeds, the macro finishes returning
 * 0. Otherwise it logs a message using the _log mode specified, executes
 * actions _acts and returns _code.
 *
 * @param _res  private local variable to store the computed _pres address.
 * @param _pres pointer to the variable that will contain the result.
 * @param _func function to call.
 * @param _args arguments to pass to the function _func.
 * @param _code error code to return if _func fails.
 * @param _log  log mode to use if _func fails.
 * @param _acts list of actions to execute if _func fails.
 *
 * @return 0 if _func succeeds. SYS_SAFE_ERROR(_code) otherwise.
 *
 * @see SYS_PTR
 * @see SYS_ERRNO_PTR
 * @see SYS_FUNC
 */
#define __SYS_PTR(_res, _pres, _func, _args, _code, _log, _acts...) \
    ({ \
        typeof(_pres) _res = (_pres); \
        SYS_FUNC(_res, _func, _args, *_res != NULL, _code, _log, ## _acts); \
    })

#define __SYS_RETRY(_err, _dly, _sum, _t, _last, _max, _start, _flast, \
                    _fcurrent, _thing, _log, _acts...) \
    ({ \
        err_t _err = 0; \
        int32_t _dly, _sum = (_max), _t = (_start), _last = 0; \
        while (_sum > 0) \
        { \
            _err = (_thing); \
            if (_err == 0) \
            { \
                break; \
            } \
            _sum -= _t; \
            if (_sum > 0) \
            { \
                _dly = _t * _fcurrent + _last * _flast; \
                _last = _t; \
                _t = _dly; \
                logI("Will retry the action in %d seconds", _dly); \
                sleep(_dly); \
            } \
        } \
        if (_err != 0) \
        { \
            sys_log(_log, "Maximum number of retries reached"); \
            SYS_ACTIONS_DO(_err, _acts); \
        } \
        _err; \
    })

/**
 * Intermediate internal macros to expand SYS_TMP().
 *
 * See related macros for more information.
 */
#define _SYS_SAFE_ERROR(_args...) __SYS_SAFE_ERROR(_args)
#define _SYS_TEST(_args...) __SYS_TEST(_args)
#define _SYS_CHAIN(_args...) __SYS_CHAIN(_args)
#define _SYS_FUNC(_args...) __SYS_FUNC(_args)
#define _SYS_CALL(_args...) __SYS_CALL(_args)
#define _SYS_CODE(_args...) __SYS_CODE(_args)
#define _SYS_ERRNO(_args...) __SYS_ERRNO(_args)
#define _SYS_ERRNO0(_args...) __SYS_ERRNO0(_args)
#define _SYS_PTR(_args...) __SYS_PTR(_args)
#define _SYS_RETRY(_args...) __SYS_RETRY(_args)

/**
 * Macro to make sure that an error is not 0.
 *
 * Wrapper to __SYS_SAFE_ERROR().
 *
 * @param _code error code to check.
 *
 * @return _code if it is not 0, EDOM otherwise.
 *
 * @see __SYS_SAFE_ERROR
 */
#define SYS_SAFE_ERROR(_code) \
    _SYS_SAFE_ERROR(SYS_TMP(1), _code)

/**
 * Macro to test a condition.
 *
 * Wrapper to __SYS_TEST().
 *
 * @param _test condition to check.
 * @param _code error code to return if _test fails.
 * @param _log  log mode to use if _test fails.
 * @param _acts list of actions to execute if _test fails.
 *
 * @return 0 if _test is true. SYS_SAFE_ERROR(_code) otherwise.
 *
 * @see __SYS_TEST
 */
#define SYS_TEST(_test, _code, _log, _acts...) \
    _SYS_TEST(SYS_TMP(1), _test, _code, _log, ## _acts)

/**
 * Macro to execute a block of code if _err is 0.
 *
 * Wrapper to __SYS_CHAIN().
 *
 * @param _pcode pointer to current error code. It is updated.
 * @param _block code to execute. Must return an error code.
 *
 * @return *_pcode if it is different than 0. Otherwise the returned value
 *         from _block.
 *
 * @see __SYS_CHAIN
 */
#define SYS_CHAIN(_pcode, _block) \
    _SYS_CHAIN(SYS_TMP(1), _pcode, _block)

/**
 * Macro to handle a generic function call.
 *
 * Wrapper to __SYS_FUNC().
 *
 * @param _pres  pointer to the variable that will contain the result.
 * @param _func  function to call.
 * @param _args  arguments to pass to the function _func.
 * @param _check condition that must satisfy the returned value.
 * @param _code  error code to return if _check fails.
 * @param _log   log mode to use if _check_fails.
 * @param _acts  list of actions to execute if _check fails.
 *
 * @return 0 if _check passes. SYS_SAFE_ERROR(_code) otherwise.
 *
 * @see __SYS_FUNC
 */
#define SYS_FUNC(_pres, _func, _args, _check, _code, _log, _acts...) \
    _SYS_FUNC(SYS_TMP(2), _pres, _func, _args, _check, _code, _log, ## _acts)

/**
 * Macro to handle a function call returning an error code.
 *
 * Wrapper to __SYS_CALL().
 *
 * @param _func function to call.
 * @param _args arguments to pass to the function _func.
 * @param _log  log mode to use if _func fails.
 * @param _acts list of actions to execute if _func fails.
 *
 * @return the result of the function call.
 *
 * @see __SYS_CALL
 */
#define SYS_CALL(_func, _args, _log, _acts...) \
    _SYS_CALL(SYS_TMP(1), _func, _args, _log, ## _acts)

/**
 * Macro to return a specific error if a function call fails.
 *
 * Wrapper to __SYS_CODE().
 *
 * @param _func function to call.
 * @param _args arguments to pass to the function _func.
 * @param _code error code to return if _func fails.
 * @param _log  log mode to use if _func fails.
 * @param _acts list of actions to execute if _func fails.
 *
 * @return 0 if _func succeeds. SYS_SAFE_ERROR(_code) otherwise.
 *
 * @see __SYS_CODE
 */
#define SYS_CODE(_func, _args, _code, _log, _acts...) \
    _SYS_CODE(SYS_TMP(1), _func, _args, _code, _log, ## _acts)

/**
 * Macro to handle a function call returning a value or an errno code.
 *
 * Wrapper to __SYS_ERRNO().
 *
 * @param _pres pointer to the variable that will contain the result.
 * @param _func function to call.
 * @param _args arguments to pass to the function _func.
 * @param _log  log mode to use if _func fails.
 * @param _acts list of actions to execute if _func fails.
 *
 * @return 0 if _func succeeds. SYS_SAFE_ERROR(errno) otherwise.
 *
 * @see __SYS_ERRNO
 */
#define SYS_ERRNO(_pres, _func, _args, _log, _acts...) \
    _SYS_ERRNO(SYS_TMP(1), _pres, _func, _args, _log, ## _acts)

/**
 * Macro to handle a function call returning 0 or an errno code.
 *
 * Wrapper to __SYS_ERRNO0().
 *
 * @param _func function to call.
 * @param _args arguments to pass to the function _func.
 * @param _log  log mode to use if _func fails.
 * @param _acts list of actions to execute if _func fails.
 *
 * @return 0 if _func succeeds. SYS_SAFE_ERROR(errno) otherwise.
 *
 * @see __SYS_ERRNO0
 */
#define SYS_ERRNO0(_func, _args, _log, _acts...) \
    _SYS_ERRNO0(SYS_TMP(1), _func, _args, _log, ## _acts)

/**
 * Macro to handle a function call returning a pointer without errno.
 *
 * Wrapper to __SYS_PTR().
 *
 * @param _pres pointer to the variable that will contain the result.
 * @param _func function to call.
 * @param _args arguments to pass to the function _func.
 * @param _code error code to return if _func fails.
 * @param _log  log mode to use if _func fails.
 * @param _acts list of actions to execute if _func fails.
 *
 * @return 0 if _func succeeds. SYS_SAFE_ERROR(_code) otherwise.
 *
 * @see __SYS_PTR
 */
#define SYS_PTR(_pres, _func, _args, _code, _log, _acts...) \
    _SYS_PTR(SYS_TMP(1), _pres, _func, _args, _code, _log, ## _acts)

/**
 * Macro to handle a function call returning a pointer with errno.
 *
 * Wrapper to __SYS_PTR().
 *
 * @param _pres pointer to the variable that will contain the result.
 * @param _func function to call.
 * @param _args arguments to pass to the function _func.
 * @param _log  log mode to use if _func fails.
 * @param _acts list of actions to execute if _func fails.
 *
 * @return 0 if _func succeeds. SYS_SAFE_ERROR(errno) otherwise.
 *
 * @see __SYS_PTR
 */
#define SYS_ERRNO_PTR(_pres, _func, _args, _log, _acts...) \
    SYS_PTR(_pres, _func, _args, errno, _log, ## _acts)

#define SYS_RETRY(_max, _start, _flast, _fcurrent, _thing, _log, _acts...) \
    _SYS_RETRY(SYS_TMP(5), _max, _start, _flast, _fcurrent, _thing, _log, \
               ## _acts)

#define SYS_TEST(_test, _code, _log, _acts...) \
    _SYS_TEST(SYS_TMP(1), _test, _code, _log, ## _acts)

#define SYS_ASSERT(_test, _msg...) \
    do \
    { \
        if (unlikely(!(_test))) \
        { \
            sys_panic(_msg); \
        } \
    } while (0)

/**
 * New action to abort execution.
 *
 * It simply calls sys_panic().
 *
 * @param _err ignored.
 * @param _msg message to log before aborting.
 *
 * @see sys_panic
 */
#define __SYS_ASSERT(_err, _msg...) \
    sys_panic(_msg)

/**
 * ASSERT action declaration.
 *
 * @param _msg message to log.
 */
#define __SYS_ACTION_ASSERT(_msg...) \
    __SYS_ASSERT, ## _msg

#endif /* __SYS_ERROR_H__ */
