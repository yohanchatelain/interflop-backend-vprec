AC_INIT([interflop-backend-vprec],[1.0],[interflop.project@gmail.com])
AM_SILENT_RULES([yes])
AC_CONFIG_AUX_DIR(autoconf)
AM_INIT_AUTOMAKE([subdir-objects -Wall -Werror foreign])
AC_CONFIG_MACRO_DIRS([m4])
AC_PROG_CC
AM_PROG_AR
AC_CONFIG_HEADERS([config.h])
LT_INIT
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET


AC_CHECK_HEADERS([interflop_stdlib.h], [], [AC_MSG_ERROR("interflop_stdlib.h is missing")])

AC_ARG_ENABLE(wall, AS_HELP_STRING([--enable-wall],[Enable -Wall compilation flag]), [WALL_CFLAGS="yes"])
AM_CONDITIONAL([WALL_CFLAGS], [test "x$WALL_CFLAGS" = "xyes"])
if test "x$WALL_CFLAGS" = "xyes"; then
   AC_DEFINE([WALL_CFLAGS], [],  ["Enable -Wall compilation flag"])
   AC_MSG_NOTICE([-Wall is set])
fi

AC_ARG_ENABLE(interflop-stdlib, AS_HELP_STRING([--with-interflop-stdlib],[Link with interflop stdlib]), [INTERFLOP_STDLIB="yes"])
AM_CONDITIONAL([INTERFLOP_STDLIB], [test "x$INTERFLOP_STDLIB" = "xyes"])
if test "x$INTERFLOP_STDLIB" = "xyes"; then
   AC_DEFINE([INTERFLOP_STDLIB], [],  ["Link with interflop stdlib"])
   AC_MSG_NOTICE([link with interflop stdlib])
fi


AC_CONFIG_FILES([
 Makefile
 common/Makefile
 common/rng/Makefile
])
AC_OUTPUT