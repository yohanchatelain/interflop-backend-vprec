SUBDIRS=rng
noinst_LTLIBRARIES = libinterflop_stdlib.la libvprec_tools.la libvfc_hashmap.la
libinterflop_stdlib_la_CFLAGS = -fPIC -static
if WALL_CFLAGS
libinterflop_stdlib_la_CFLAGS += -fPIC -static
endif
libinterflop_stdlib_la_SOURCES = \
    interflop_stdlib.h \
    interflop_stdlib.c
libvprec_tools_la_CFLAGS = -fPIC -static
if WALL_CFLAGS
libvprec_tools_la_CFLAGS += -Wall -Wextra
endif
libvprec_tools_la_SOURCES = \
    vprec_tools.h \
    vprec_tools.c
libvfc_hashmap_la_CFLAGS = -fPIC -static
if WALL_CFLAGS
libvfc_hashmap_la_CFLAGS += -Wall -Wextra
endif
libvfc_hashmap_la_SOURCES = \
    vfc_hashmap.h \
    vfc_hashmap.c
