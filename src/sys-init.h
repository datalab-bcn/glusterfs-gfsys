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

#ifndef __SYS_INIT_H__
#define __SYS_INIT_H__

extern sys_mutex_t sys_init_mutex;

#define __SYS_INIT(_err, _init, _func, _args, _log, _acts...) \
    ({ \
        static bool _init = false; \
        err_t _err = EALREADY; \
        if (!_init) \
        { \
            sys_mutex_lock(&sys_init_mutex); \
            if (!_init) \
            { \
                _err = _func _args; \
                _init = (_err == 0); \
            } \
            sys_mutex_unlock(&sys_init_mutex); \
            if (unlikely(!_init)) \
            { \
                sys_log(_log, #_func "() failed. Returned error %d", _err); \
                SYS_ACTIONS_DO(_err, _acts); \
            } \
        } \
        else \
        { \
            logT(#_func "() already succeeded"); \
        } \
        _err; \
    })

#define _SYS_INIT(_args...) __SYS_INIT(_args)

#define SYS_INIT(_func, _args, _log, _acts...) \
    _SYS_INIT(SYS_TMP(2), _func, _args, _log, ## _acts)

#endif /* __SYS_INIT_H__ */
