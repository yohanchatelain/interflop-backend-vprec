noinst_LTLIBRARIES = libvfc_rng.la

libvfc_rng_la_LDFLAGS = -static -fPIC
libvfc_rng_la_CFLAGS = -O3
if WALL_CFLAGS
libvfc_rng_la_CFLAGS += -Wall -Wextra
endif
libvfc_rng_la_SOURCES = \
    splitmix64.c \
    xoroshiro128.c \
    vfc_rng.c
