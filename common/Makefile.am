SUBDIRS=rng
noinst_LTLIBRARIES = libinterflop_stdlib.la libvprec_tools.la libvfc_hashmap.la libinterflop_fma.la

libinterflop_stdlib_la_CFLAGS = -fPIC -static -nostdlib -fno-stack-protector
# libinterflop_stdlib_la_LDFLAGS = -nostdlib
if WALL_CFLAGS
libinterflop_stdlib_la_CFLAGS += -fPIC -static   -fno-stack-protector
endif
libinterflop_stdlib_la_SOURCES = \
    interflop_stdlib.h \
    interflop_stdlib.c

libvprec_tools_la_CFLAGS = -fPIC -static -nostdlib  -fno-stack-protector
libvprec_tools_la_LDFLAGS = -lquadmath -lm
if WALL_CFLAGS
libvprec_tools_la_CFLAGS += -Wall -Wextra 
endif
libvprec_tools_la_SOURCES = \
    vprec_tools.h \
    vprec_tools.c

libvfc_hashmap_la_CFLAGS = -fPIC -static -nostdlib -fno-stack-protector
# libvfc_hashmap_la_LDFLAGS = -nostdlib
if WALL_CFLAGS
libvfc_hashmap_la_CFLAGS += -Wall -Wextra 
endif
libvfc_hashmap_la_SOURCES = \
    vfc_hashmap.h \
    vfc_hashmap.c

libinterflop_fma_la_CFLAGS = -fPIC -static -nostdlib -fno-stack-protector
# libinterflop_fma_la_LDFLAGS = -nostdlib
if WALL_CFLAGS
libinterflop_fma_la_CFLAGS += -Wall -Wextra 
endif
libinterflop_fma_la_SOURCES = \
    fmaqApproc.h \
    fmaqApprox.c 

include_HEADERS=interflop.h interflop_stdlib.h