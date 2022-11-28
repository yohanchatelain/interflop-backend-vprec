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

AC_ARG_ENABLE(wall, AS_HELP_STRING([--enable-wall],[Enable -Wall compilation flag]), [WALL_CFLAGS="yes"])
AM_CONDITIONAL([WALL_CFLAGS], [test "x$WALL_CFLAGS" = "xyes"])
if test "x$WALL_CFLAGS" = "xyes"; then
   AC_DEFINE([WALL_CFLAGS], [],  ["Enable -Wall compilation flag"])
   AC_MSG_NOTICE([-Wall is set])
fi

AC_ARG_WITH([interflop-stdlib],
  AS_HELP_STRING([--with-interflop-stdlib@<:@=DIR@:>@],
    [use interflop-stdlib located in DIR]),
  [with_interflop_stdlib="$withval"],
  [with_interflop_stdlib=yes])
AM_CONDITIONAL([BUILD_INTERFLOP_STDLIB], [test "x$with_interflop_stdlib" != "xno"])

if test "x$with_interflop_stdlib" = "xno"; then
   AC_MSG_NOTICE([--with-interflop-stdlib=no was given. Disabling interflop-stdlib linkage.])
else

   if test "x$with_interflop_stdlib" = "xyes"; then
      with_interflop_stdlib_path=$PWD/interflop-stdlib/install/
   else
      with_interflop_stdlib_path="$with_interflop_stdlib"
   fi
   AC_DEFINE([BUILD_INTERFLOP_STDLIB], [],  ["Link with interflop stdlib"])
fi

AC_CONFIG_FILES([
 Makefile
 common/Makefile
 common/rng/Makefile
])
AC_OUTPUT