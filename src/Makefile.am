
MAKEFLAGS = $(AM_MAKEFLAGS)

lib_LTLIBRARIES := libgfsys.la libsys.la

libgfsys_la_SOURCES := sys-memory.c
libgfsys_la_SOURCES += sys-event.c
libgfsys_la_SOURCES += sys-gf.c
libgfsys_la_SOURCES += sys-ratelimit.c
libgfsys_la_SOURCES += sys-time.c
libgfsys_la_SOURCES += sys-mutex.c
libgfsys_la_SOURCES += sys-cond.c
libgfsys_la_SOURCES += sys-thread.c
libgfsys_la_SOURCES += sys-tsc.c
libgfsys_la_SOURCES += sys-buffer.c
libgfsys_la_SOURCES += sys-async.c
libgfsys_la_SOURCES += sys-calls.c
libgfsys_la_SOURCES += sys-io.c
libgfsys_la_SOURCES += sys-lock.c
libgfsys_la_SOURCES += sys-numfmt.c
libgfsys_la_SOURCES += sys-dict.c
libgfsys_la_SOURCES += sys-dirent.c
libgfsys_la_SOURCES += sys.c

libgfsys_la_LIBADD = $(gfdir)/libglusterfs/src/libglusterfs.la

# Hack to precompile a header

libsys_la_CFLAGS = -x c-header

libsys_la_SOURCES := gfsys.c
libsys.la:

$(libgfsys_la_SOURCES): gfsys.h.gch

gfsys.h.gch:	libsys_la-gfsys.lo
				@ln -sf `sed -n "s/^pic_object='\(.*\)'.*/\1/p" libsys_la-gfsys.lo` gfsys.h.gch
				@touch gfsys.h.gch

CLEANFILES = gfsys.h.gch

