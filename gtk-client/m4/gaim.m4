dnl ======================================
dnl Gaim Configuration
dnl ======================================
dnl
dnl Copyright (C) 2004 Josef Spillner <josef@ggzgamingzone.org>
dnl

AC_DEFUN([AC_GAIM],
[
AC_MSG_CHECKING([for Gtk+ library: libgaim])

ac_gaim_includes=NO
gaim_includes=""

AC_CACHE_VAL(ac_cv_have_gaim,
[
AC_GGZ_FIND_FILE(gaim/gaim.h, "/usr/include", gaim_incdir)
ac_gaim_includes="$gaim_incdir"

if test "$ac_gaim_includes" = NO; then
  ac_cv_have_gaim="have_gaim=no"
  ac_gaim_notfound=""
else
  have_gaim="yes"
fi
])

eval "$ac_cv_have_gaim"

if test "$have_gaim" != yes; then
  AC_MSG_RESULT([$have_gaim])
else
  AC_MSG_RESULT([$have_gaim (headers $ac_gaim_includes)])

  GAIMPLUGIN='gaim-plugin'
  AC_SUBST(GAIMPLUGIN)
fi

])

