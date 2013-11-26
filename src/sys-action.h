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

#ifndef __SYS_ACTION_H__
#define __SYS_ACTION_H__

/* NOTE: These macros seem ugly, but they are very useful to write less, more
 *       compact, more clean and easy to follow code.
 */

/**
 * Actions.
 *
 * Actions are small fragments of code executed as part of another macro,
 * generally on error conditions.
 *
 * This header defines the following actions:
 *
 *      NULL([<args>])
 *          Does nothing. Args are ignored.
 *
 *      BREAK([<save>])
 *          Executes a 'break'. If <save> is present, the error code will be
 *          assigned to it.
 *
 *      CONTINUE([<save>])
 *          Executes a 'continue'. If <save> is present, the error code will be
 *          assigned to it.
 *
 *      GOTO(<label>[, <save>])
 *          Executes a 'goto <label>'. If <save> is present, the error code
 *          will be assigned to it.
 *
 *      RETURN([<save>])
 *          Executes a 'return'. If <save> is present, the error code will be
 *          assigned to it.
 *
 *      RETVAL(<value>[, <save>])
 *          Executes a 'return <value>'. If <save> is present, the error code
 *          will be assigned to it.
 *
 *      RETERR([<save>])
 *          Executes a 'return <error code>'. If <save> is present, the error
 *          code will be assigned to it.
 *      SAVE(<save>)
 *          Simply stores the error code into <save>.
 *
 * All these actions change the flow of execution. This means that actions
 * following them will be ignored.
 *
 * Examples:
 *
 *      SYS_ERRNO(
 *          &fd,
 *          open, (file, "r"),
 *          E(),
 *          RETERR()
 *      )
 *
 *          If open(file, "r") fails, it will log an error message and return
 *          the error detected.
 *
 *      SYS_ERRNO(
 *          &fd,
 *          open, (file, "r"),
 *          W(),
 *          GOTO(failed)
 *      )
 *
 *          If open(file, "r") fails, it will log a warning message and jump to
 *          failed label.
 *
 *      SYS_ERRNO(
 *          &fd,
 *          open, (file, "r"),
 *          W(),
 *          GOTO(failed, &err)
 *      )
 *
 *          If open(file, "r") fails, it will log a warning message, store the
 *          detected error into 'err' and jump to 'failed' label.
 *
 *      SYS_ERRNO(
 *          &fd,
 *          open, (file, "r"),
 *          D(),
 *          LOG(W(), "Failed to open file, trying again"),
 *          CONTINUE()
 *      )
 *
 *          In this case, if open(file, "r") fails, it will log a debug message
 *          with the exact error, a warning message with the text "Failed to
 *          open file, trying again", and execute a 'continue' statement (this
 *          assumes that this fragment of code is inside a loop).
 *
 * More actions can be defined by declaring a macro like this:
 *
 *     __SYS_ACTION_<name>([<arguments>]) <macro>[, <fields>]
 *
 * Where <name> is the name we want to use as an action and <arguments> are
 * optional arguments to the action. <macro> is the name of a macro that will
 * be called with an error code, followed by <fields> if present. <fields> can
 * be anything, but generally they will come from <arguments>.
 */

/**
 * Internal macro to store a value into an optional variable.
 *
 * It is used in conjunction with __SYS_ACTION_DO() to save the error value
 * into a user specified variable or a dummy variable if none specified.
 *
 * @see __SYS_ACTION_DO
 */
#define __SYS_SAVE(_value, _var, _dummy...) (*(_var) = (_value))

/**
 * Internal macro to optionally save the error code and execute an action.
 *
 * It executes the _action specified, logging it with TRACE level, and stores
 * the current error into a user supplied value. If none is supplied, the error
 * code is lost.
 *
 * @see __SYS_SAVE
 */
#define __SYS_ACTION_DO_2(_tmp, _err, _name, _action, _save...) \
    ({ \
        int32_t _tmp; \
        (void)_tmp; \
        __SYS_SAVE(_err, ## _save, &_tmp); \
        __SYS_LOG_TYPE_g("action", T, "Executing '" #_name "' due to error " \
                         "%d", _err); \
        _action; \
    })
#define __SYS_ACTION_DO_1(_args...) __SYS_ACTION_DO_2(_args)
#define __SYS_ACTION_DO(_err, _name, _action, _save...) \
    __SYS_ACTION_DO_1(SYS_TMP(1), _err, _name, _action, ## _save)

/**
 * Internal macros to be used to execute specific actions.
 *
 * @see __SYS_ACTION_DO
 */
#define __SYS_NULL(_err) \
    do { } while (0)

#define __SYS_BREAK(_err, _save...) \
    __SYS_ACTION_DO(_err, break, break, ## _save)

#define __SYS_CONTINUE(_err, _save...) \
    __SYS_ACTION_DO(_err, continue, continue, ## _save)

#define __SYS_GOTO(_err, _label, _save...) \
    __SYS_ACTION_DO(_err, goto, goto _label, ## _save)

#define __SYS_RETURN(_err, _save...) \
    __SYS_ACTION_DO(_err, return, return, ## _save)

#define __SYS_RETVAL(_err, _val, _save...) \
    __SYS_ACTION_DO(_err, return, return (_val), ## _save)

#define __SYS_RETERR(_err, _save...) \
    __SYS_ACTION_DO(_err, return, return _err, ## _save)

/**
 * Internal macros used to expand user supplied actions.
 *
 * These are used to split a user supplied action into a macro name and a list
 * of arguments that will be recomposed with an addicional error argument.
 *
 * @see __SYS_ACTION_MACRO
 */
#define __SYS_ACTION_                       __SYS_NULL
#define __SYS_ACTION_NULL(_dummy...)        __SYS_NULL
#define __SYS_ACTION_BREAK(_save...)        __SYS_BREAK,            ## _save
#define __SYS_ACTION_CONTINUE(_save...)     __SYS_CONTINUE,         ## _save
#define __SYS_ACTION_GOTO(_label, _save...) __SYS_GOTO,     _label, ## _save
#define __SYS_ACTION_RETURN(_save...)       __SYS_RETURN,           ## _save
#define __SYS_ACTION_RETVAL(_val, _save...) __SYS_RETVAL,   _val,   ## _save
#define __SYS_ACTION_RETERR(_save...)       __SYS_RETERR,           ## _save
#define __SYS_ACTION_SAVE(_save)            __SYS_SAVE,     _save

/**
 * Internal macros used to add an argument to other macros.
 *
 * These are used to add the _err argument to a call to another macro
 * indirectly called through a user supplied action.
 */
#define __SYS_ACTION_MACRO_1(_err, _macro, _args...) \
    _macro(_err, ## _args)
#define __SYS_ACTION_MACRO(_err, _macro, _args...) \
    __SYS_ACTION_MACRO_1(_err, _macro, ## _args)

/**
 * Internal macros used to expand a list of user supplied actions.
 *
 * Multiple macros are needed because they cannot be used recursively. A
 * maximum of 5 actions is currently supported. If more actions are needed,
 * more macros need to be created.
 */
#define __SYS_ACTIONS_DO_5(_err, _first) \
    __SYS_ACTION_MACRO(_err, __SYS_ACTION_##_first)
#define __SYS_ACTIONS_DO_4(_err, _first, _other...) \
    __SYS_ACTION_MACRO(_err, __SYS_ACTION_##_first); \
    __SYS_ACTIONS_DO_5(_err, _other)
#define __SYS_ACTIONS_DO_3(_err, _first, _other...) \
    __SYS_ACTION_MACRO(_err, __SYS_ACTION_##_first); \
    __SYS_ACTIONS_DO_4(_err, _other)
#define __SYS_ACTIONS_DO_2(_err, _first, _other...) \
    __SYS_ACTION_MACRO(_err, __SYS_ACTION_##_first); \
    __SYS_ACTIONS_DO_3(_err, _other)
#define __SYS_ACTIONS_DO_1(_err, _first, _other...) \
    __SYS_ACTION_MACRO(_err, __SYS_ACTION_##_first); \
    __SYS_ACTIONS_DO_2(_err, _other)
#define SYS_ACTIONS_DO(_err, _actions...) \
    __SYS_ACTIONS_DO_1(_err, _actions)

#endif /* __SYS_ACTION_H__ */
