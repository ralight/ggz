dnl Process this file with autoconf to produce a configure script.
dnl $Id: configure.ac 5358 2003-02-04 01:55:26Z jdorje $

AC_INIT(configure.ac)
AM_INIT_AUTOMAKE(ggz-gnome-client, 0.0.7pre)
AM_CONFIG_HEADER(config.h)

PKG_CHECK_MODULES(GGZ, libgnomeui-2.0)
AC_SUBST(GGZ_CFLAGS)
AC_SUBST(GGZ_LIBS)

AC_PROG_CC
AC_PROG_INSTALL

if test "x${prefix}" = "xNONE"; then
  AC_DEFINE_UNQUOTED(PACKAGE_LOCALE_DIR, "${ac_default_prefix}/${DATADIRNAME}/locale")
else
  AC_DEFINE_UNQUOTED(PACKAGE_LOCALE_DIR, "${prefix}/${DATADIRNAME}/locale")
fi

AC_GGZ_INIT(defaults)
AC_GGZ_LIBGGZ
AC_GGZ_GGZCORE

ggzdatadir=`ggz-config --datadir`
AC_DEFINE_UNQUOTED(GGZDATADIR, "${ggzdatadir}")
AC_SUBST(ggzdatadir)

LDADD="$LDADD $LIB_GGZCORE $LIB_GGZ"
AC_SUBST(LDADD)

AC_OUTPUT([
Makefile
pixmaps/Makefile
src/Makefile
])

