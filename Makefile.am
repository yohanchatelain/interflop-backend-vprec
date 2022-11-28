ACLOCAL_AMFLAGS=-I m4
SUBDIRS=common
lib_LTLIBRARIES = libinterflop_vprec.la

libinterflop_vprec_la_SOURCES = \
    interflop_vprec.c \
    interflop_vprec_function_instrumentation.c \
    common/logger.c
libinterflop_vprec_la_CFLAGS = -DBACKEND_HEADER="interflop_vprec" -fno-stack-protector
if WALL_CFLAGS
libinterflop_vprec_la_CFLAGS += -Wall -Wextra -Wno-varargs -g  
endif
libinterflop_vprec_la_LIBADD = \
    common/libvprec_tools.la \
    common/libvfc_hashmap.la \
    common/libinterflop_fma.la 
if INTERFLOP_STDLIB
libinterflop_vprec_la_LIBADD += libinterflop_stdlib.a 
endif
libinterflop_vprec_la_includedir =$(includedir)/
libinterflop_vprec_la_HEADER = interflop_stdlib.h
    

