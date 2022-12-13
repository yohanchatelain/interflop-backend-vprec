noinst_LTLIBRARIES = libvprec_tools.la

libvprec_tools_la_CFLAGS = -flto -Og -fno-stack-protector
libvprec_tools_la_LDFLAGS = -lquadmath -lm -flto -Og
if WALL_CFLAGS
libvprec_tools_la_CFLAGS += -Wall -Wextra -g
endif
libvprec_tools_la_SOURCES = \
    vprec_tools.h \
    vprec_tools.c
