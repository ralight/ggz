dnl Process this file with autoconf to produce a configure script.
dnl $Id: configure.ac 5387 2003-02-04 17:38:06Z jdorje $

AC_INIT
AC_CONFIG_SRCDIR([configure.ac])
AM_INIT_AUTOMAKE(ggz-gnome-client, 0.0.7pre)
AC_CONFIG_HEADERS([config.h])

PKG_CHECK_MODULES(GGZ, libgnomeui-2.0)
AC_SUBST(GGZ_CFLAGS)
AC_SUBST(GGZ_LIBS)

AC_PROG_CC
AC_PROG_INSTALL

if test "x${prefix}" = "xNONE"; then
  AC_DEFINE_UNQUOTED([PACKAGE_LOCALE_DIR],
		     "${ac_default_prefix}/${DATADIRNAME}/locale",
                     [The location to put message catalogs])
else
  AC_DEFINE_UNQUOTED([PACKAGE_LOCALE_DIR],
		     "${prefix}/${DATADIRNAME}/locale",
                     [The location to put message catalogs])
fi

AC_GGZ_INIT(defaults)
AC_GGZ_LIBGGZ
AC_GGZ_GGZCORE

ggzdatadir=`ggz-config --datadir`
AC_DEFINE_UNQUOTED([GGZDATADIR], "${ggzdatadir}",
		   [Location of GGZ module data])
AC_SUBST(ggzdatadir)

LDADD="$LDADD $LIB_GGZCORE $LIB_GGZ"
AC_SUBST(LDADD)

AC_CONFIG_FILES([
Makefile
pixmaps/Makefile
src/Makefile
])
AC_OUTPUT

