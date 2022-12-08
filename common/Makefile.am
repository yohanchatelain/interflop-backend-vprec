noinst_LTLIBRARIES = libvprec_tools.la

libvprec_tools_la_CFLAGS = -fno-stack-protector
libvprec_tools_la_LDFLAGS = -lquadmath -lm
if WALL_CFLAGS
libvprec_tools_la_CFLAGS += -Wall -Wextra 
endif
libvprec_tools_la_SOURCES = \
    vprec_tools.h \
    vprec_tools.c
