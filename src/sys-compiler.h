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

#ifndef __SYS_COMPILER_H__
#define __SYS_COMPILER_H__

#include "sys-base.h"

#ifdef __GNUC__
#include "sys-compiler-gcc.h"
#else
#error "Unrecognized compiler"
#endif

#ifndef __must_check
#define __must_check
#endif

#ifndef __align
#define __align(_n)
#endif

#ifndef __inline
#define __inline inline
#endif

#ifndef __noinline
#define __noinline
#endif

#ifndef __deprecated
#define __deprecated
#endif

#ifndef __noreturn
#define __noreturn
#endif

#ifndef __section
#define __section(_name)
#endif

#ifndef __used
#define __used
#endif

#ifndef __unused
#define __unused
#endif

#ifndef __warn
#define __warn(_text)
#endif

#ifndef __error
#define __error(_text)
#endif

#ifndef likely
#define likely(_x) (_x)
#endif

#ifndef unlikely
#define unlikely(_x) (_x)
#endif

#endif /* __SYS_COMPILER_H__ */
