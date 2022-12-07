ACLOCAL_AMFLAGS=-I m4

SUBDIRS=common
lib_LTLIBRARIES = libinterflop_vprec.la
libinterflop_vprec_la_SOURCES = \
    interflop_vprec.c \
    interflop_vprec_function_instrumentation.c \
    @INTERFLOP_STDLIB_PATH@/include/interflop-stdlib/iostream/logger.c
libinterflop_vprec_la_CFLAGS = -DBACKEND_HEADER="interflop_vprec" -fno-stack-protector
if WALL_CFLAGS
libinterflop_vprec_la_CFLAGS += -Wall -Wextra -Wno-varargs
endif
libinterflop_vprec_la_LIBADD = \
    common/libvprec_tools.la \
    @INTERFLOP_STDLIB_PATH@/lib/libinterflop_fma.la \
    @INTERFLOP_STDLIB_PATH@/lib/libinterflop_hashmap.la 
if LINK_INTERFLOP_STDLIB
libinterflop_vprec_la_LIBADD += @INTERFLOP_STDLIB_PATH@/lib/libinterflop_stdlib.la
endif
headersdir = $(includedir)/
nobase_headers_HEADERS = interflop_vprec.h common/vprec_tools.h