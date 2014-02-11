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

#ifndef __SYS_ARGS_H__
#define __SYS_ARGS_H__

#define _SYS_ARGS_MACRO(_macro, _args...) _macro(_args)
#define SYS_ARGS_MACRO(_macro, _idx, _extra, _arg) \
    _SYS_ARGS_MACRO(_macro, (_idx SYS_MARG(SYS_EXPAND(_arg))) \
                            SYS_MARG(SYS_EXPAND(_extra)))
#define SYS_ARGS_APPLY_1(_macro, _extra, _arg, _args...) \
    SYS_EXPAND(SYS_SELECT((SYS_ARGS_MACRO(_macro, 15, _extra, _arg)), (), _arg))
#define SYS_ARGS_APPLY_2(_macro, _extra, _arg, _args...) \
    SYS_EXPAND(SYS_SELECT((SYS_ARGS_MACRO(_macro, 14, _extra, _arg)), (), _arg)) \
    SYS_MARG(SYS_ARGS_APPLY_1(_macro, _extra, _args))
#define SYS_ARGS_APPLY_3(_macro, _extra, _arg, _args...) \
    SYS_EXPAND(SYS_SELECT((SYS_ARGS_MACRO(_macro, 13, _extra, _arg)), (), _arg)) \
    SYS_MARG(SYS_ARGS_APPLY_2(_macro, _extra, _args))
#define SYS_ARGS_APPLY_4(_macro, _extra, _arg, _args...) \
    SYS_EXPAND(SYS_SELECT((SYS_ARGS_MACRO(_macro, 12, _extra, _arg)), (), _arg)) \
    SYS_MARG(SYS_ARGS_APPLY_3(_macro, _extra, _args))
#define SYS_ARGS_APPLY_5(_macro, _extra, _arg, _args...) \
    SYS_EXPAND(SYS_SELECT((SYS_ARGS_MACRO(_macro, 11, _extra, _arg)), (), _arg)) \
    SYS_MARG(SYS_ARGS_APPLY_4(_macro, _extra, _args))
#define SYS_ARGS_APPLY_6(_macro, _extra, _arg, _args...) \
    SYS_EXPAND(SYS_SELECT((SYS_ARGS_MACRO(_macro, 10, _extra, _arg)), (), _arg)) \
    SYS_MARG(SYS_ARGS_APPLY_5(_macro, _extra, _args))
#define SYS_ARGS_APPLY_7(_macro, _extra, _arg, _args...) \
    SYS_EXPAND(SYS_SELECT((SYS_ARGS_MACRO(_macro, 9, _extra, _arg)), (), _arg)) \
    SYS_MARG(SYS_ARGS_APPLY_6(_macro, _extra, _args))
#define SYS_ARGS_APPLY_8(_macro, _extra, _arg, _args...) \
    SYS_EXPAND(SYS_SELECT((SYS_ARGS_MACRO(_macro, 8, _extra, _arg)), (), _arg)) \
    SYS_MARG(SYS_ARGS_APPLY_7(_macro, _extra, _args))
#define SYS_ARGS_APPLY_9(_macro, _extra, _arg, _args...) \
    SYS_EXPAND(SYS_SELECT((SYS_ARGS_MACRO(_macro, 7, _extra, _arg)), (), _arg)) \
    SYS_MARG(SYS_ARGS_APPLY_8(_macro, _extra, _args))
#define SYS_ARGS_APPLY_10(_macro, _extra, _arg, _args...) \
    SYS_EXPAND(SYS_SELECT((SYS_ARGS_MACRO(_macro, 6, _extra, _arg)), (), _arg)) \
    SYS_MARG(SYS_ARGS_APPLY_9(_macro, _extra, _args))
#define SYS_ARGS_APPLY_11(_macro, _extra, _arg, _args...) \
    SYS_EXPAND(SYS_SELECT((SYS_ARGS_MACRO(_macro, 5, _extra, _arg)), (), _arg)) \
    SYS_MARG(SYS_ARGS_APPLY_10(_macro, _extra, _args))
#define SYS_ARGS_APPLY_12(_macro, _extra, _arg, _args...) \
    SYS_EXPAND(SYS_SELECT((SYS_ARGS_MACRO(_macro, 4, _extra, _arg)), (), _arg)) \
    SYS_MARG(SYS_ARGS_APPLY_11(_macro, _extra, _args))
#define SYS_ARGS_APPLY_13(_macro, _extra, _arg, _args...) \
    SYS_EXPAND(SYS_SELECT((SYS_ARGS_MACRO(_macro, 3, _extra, _arg)), (), _arg)) \
    SYS_MARG(SYS_ARGS_APPLY_12(_macro, _extra, _args))
#define SYS_ARGS_APPLY_14(_macro, _extra, _arg, _args...) \
    SYS_EXPAND(SYS_SELECT((SYS_ARGS_MACRO(_macro, 2, _extra, _arg)), (), _arg)) \
    SYS_MARG(SYS_ARGS_APPLY_13(_macro, _extra, _args))
#define SYS_ARGS_APPLY_15(_macro, _extra, _arg, _args...) \
    SYS_EXPAND(SYS_SELECT((SYS_ARGS_MACRO(_macro, 1, _extra, _arg)), (), _arg)) \
    SYS_MARG(SYS_ARGS_APPLY_14(_macro, _extra, _args))
#define SYS_ARGS_APPLY_16(_macro, _extra, _arg, _args...) \
    SYS_EXPAND(SYS_SELECT((SYS_ARGS_MACRO(_macro, 0, _extra, _arg)), (), _arg)) \
    SYS_MARG(SYS_ARGS_APPLY_15(_macro, _extra, _args))

#define _SYS_ARGS_APPLY(_args...) SYS_ARGS_APPLY_16(_args)
#define SYS_ARGS_APPLY(_macro, _args, _extra...) \
    _SYS_ARGS_APPLY(_macro, (_extra), SYS_EXPAND(_args))

#define _SYS_ARG_GET(_macro, _arg, _more...) _macro(_arg, ## _more)
#define SYS_ARG_GET(_what, _arg, _more...) \
    SYS_EXPAND(SYS_SELECT( \
        (_SYS_ARG_GET( \
             SYS_GLUE(SYS_ARG_, SYS_GET(3, SYS_EXPAND(_arg)), _, _what), \
             _arg, ## _more) \
        ), \
        (SYS_ARG_##_what(_arg, ## _more)), \
        SYS_GET(3, SYS_EXPAND(_arg)) \
    ))

#define SYS_ARG_INDEX(_arg, _more...) SYS_GET(0, SYS_EXPAND(_arg))

#define SYS_ARG_TYPE(_arg, _more...) SYS_GET(1, SYS_EXPAND(_arg))
#define SYS_ARG_NAME(_arg, _more...) SYS_GET(2, SYS_EXPAND(_arg))
#define SYS_ARG_FIELD(_arg, _more...) SYS_ARG_TYPE(_arg) SYS_ARG_NAME(_arg)
#define SYS_ARG_DECL(_arg, _more...) SYS_ARG_FIELD(_arg)
#define SYS_ARG_SAVE(_arg, _dst, _more...) \
    (_dst)->SYS_ARG_NAME(_arg) = SYS_ARG_NAME(_arg)
#define SYS_ARG_LOAD(_arg, _src, _more...) (_src)->SYS_ARG_NAME(_arg)
#define SYS_ARG_FREE(_arg, _src, _more...) do { } while (0)
#define SYS_ARG_INIT(_arg, _more...) \
    SYS_ARG_FIELD(_arg, ## _more) = (SYS_ARG_TYPE(_arg, ## _more))0

#define SYS_ARG_PTR_TYPE(_arg, _more...) SYS_ARG_TYPE(_arg, ## _more) *
#define SYS_ARG_PTR_NAME(_arg, _more...) SYS_ARG_NAME(_arg, ## _more)
#define SYS_ARG_PTR_FIELD(_arg, _more...) SYS_ARG_FIELD(_arg, ## _more)
#define SYS_ARG_PTR_DECL(_arg, _more...) \
    SYS_ARG_TYPE(_arg, ## _more) * SYS_ARG_NAME(_arg, ## _more)
#define SYS_ARG_PTR_SAVE(_arg, _dst, _more...) \
    SYS_GET(4, SYS_EXPAND(_arg))(&(_dst)->SYS_ARG_NAME(_arg, ## _more), \
                                 SYS_ARG_NAME(_arg, ## _more))
#define SYS_ARG_PTR_LOAD(_arg, _src, _more...) \
    &(_src)->SYS_ARG_NAME(_arg, ## _more)
#define SYS_ARG_PTR_FREE(_arg, _src, _more...) \
    SYS_GET(5, SYS_EXPAND(_arg))(&(_src)->SYS_ARG_NAME(_arg, ## _more))
#define SYS_ARG_PTR_INIT(_arg, _more...) \
    SYS_ARG_TYPE(_arg, ## _more) * SYS_ARG_NAME(_arg, ## _more) = NULL

#define SYS_ARG_COPY_TYPE(_arg, _more...) SYS_ARG_TYPE(_arg, ## _more)
#define SYS_ARG_COPY_NAME(_arg, _more...) SYS_ARG_NAME(_arg, ## _more)
#define SYS_ARG_COPY_FIELD(_arg, _more...) SYS_ARG_FIELD(_arg, ## _more)
#define SYS_ARG_COPY_DECL(_arg, _more...) SYS_ARG_DECL(_arg, ## _more)
#define SYS_ARG_COPY_SAVE(_arg, _dst, _more...) \
    SYS_GET(4, SYS_EXPAND(_arg))(&(_dst)->SYS_ARG_NAME(_arg, ## _more), \
                                 SYS_ARG_NAME(_arg, ## _more))
#define SYS_ARG_COPY_LOAD(_arg, _src, _more...) \
    SYS_ARG_LOAD(_arg, _src, ## _more)
#define SYS_ARG_COPY_FREE(_arg, _src, _more...) \
    SYS_GET(5, SYS_EXPAND(_arg))((_src)->SYS_ARG_NAME(_arg, ## _more))
#define SYS_ARG_COPY_INIT(_arg, _more...) \
    SYS_ARG_FIELD(_arg, ## _more) = NULL

#define SYS_ARG_ARRAY_TYPE(_arg, _more...) SYS_ARG_TYPE(_arg, ## _more)
#define SYS_ARG_ARRAY_NAME(_arg, _more...) SYS_ARG_NAME(_arg, ## _more)
#define SYS_ARG_ARRAY_FIELD(_arg, _more...) \
    uint8_t SYS_ARG_NAME(_arg)[SYS_GET(4, SYS_EXPAND(_arg))]
#define SYS_ARG_ARRAY_DECL(_arg, _more...) SYS_ARG_DECL(_arg, ## _more)
#define SYS_ARG_ARRAY_SAVE(_arg, _dst, _more...) \
    memcpy((_dst)->SYS_ARG_NAME(_arg, ## _more), \
           SYS_ARG_NAME(_arg, ## _more), \
           SYS_GET(4, SYS_EXPAND(_arg)))
#define SYS_ARG_ARRAY_LOAD(_arg, _src, _more...) \
    *(SYS_ARG_TYPE(_arg, ## _more) *)&(_src)->SYS_ARG_NAME(_arg, ## _more)
#define SYS_ARG_ARRAY_FREE(_arg, _src, _more...) \
    SYS_ARG_FREE(_arg, _src, ## _more)
#define SYS_ARG_ARRAY_INIT(_arg, _more...) \
    SYS_ARG_TYPE(_arg, ## _more) SYS_ARG_NAME(_arg, ## _more) = NULL

#define ARG_TYPE(_arg, _more...) SYS_ARG_GET(TYPE, _arg, ## _more)
#define ARG_NAME(_arg, _more...) SYS_ARG_GET(NAME, _arg, ## _more)
#define ARG_FIELD(_arg, _more...) SYS_ARG_GET(FIELD, _arg, ## _more)
#define ARG_DECL(_arg, _more...) SYS_ARG_GET(DECL, _arg, ## _more)
#define ARG_SAVE(_arg, _more...) SYS_ARG_GET(SAVE, _arg, ## _more)
#define ARG_LOAD(_arg, _more...) SYS_ARG_GET(LOAD, _arg, ## _more)
#define ARG_FREE(_arg, _more...) SYS_ARG_GET(FREE, _arg, ## _more)
#define ARG_INIT(_arg, _more...) SYS_ARG_GET(INIT, _arg, ## _more)

#define SYS_ARGS_STRUCT(_name) struct _sys_args_##_name
#define SYS_ARGS_TYPE(_name) sys_args_##_name##_t

#define SYS_ARGS_STRUCT_DEFINE(_name, _args, _more...) \
    SYS_ARGS_STRUCT(_name) \
    { \
        SYS_JOIN(SEMIC, SYS_ARGS_APPLY(ARG_FIELD, _args, ## _more)); \
    }
#define SYS_ARGS_TYPE_DEFINE(_name, _args, _more...) \
    typedef SYS_ARGS_STRUCT_DEFINE(_name, _args, ## _more) SYS_ARGS_TYPE(_name)

#define SYS_ARGS_NAMES(_args, _more...) \
    SYS_ARGS_APPLY(ARG_NAME, _args, ## _more)
#define SYS_ARGS_DECL(_args, _more...) \
    SYS_ARGS_APPLY(ARG_DECL, _args, ## _more)
#define SYS_ARGS_SAVE(_dst, _args, _more...) \
    SYS_JOIN(SEMIC, SYS_ARGS_APPLY(ARG_SAVE, _args, _dst, ## _more))
#define SYS_ARGS_LOAD(_src, _args, _more...) \
    SYS_ARGS_APPLY(ARG_LOAD, _args, _src, ## _more)
#define SYS_ARGS_FREE(_src, _args, _more...) \
    SYS_JOIN(SEMIC, SYS_ARGS_APPLY(ARG_FREE, _args, _src, ## _more))
#define SYS_ARGS_INIT(_args, _more...) \
    SYS_JOIN(SEMIC, SYS_ARGS_APPLY(ARG_INIT, _args, ## _more))

#define __SYS_ARGS_COMBINE(_dummy, _args...) (_args)
#define _SYS_ARGS_COMBINE(_args...) __SYS_ARGS_COMBINE(_args)
#define SYS_ARGS_COMBINE(_args1, _args2...) \
    _SYS_ARGS_COMBINE(DUMMY \
                      SYS_MARG(SYS_EXPAND(_args1)) \
                      SYS_MARG(SYS_EXPAND(_args2)))

#endif /* SYS_ARSG_H__ */
