dnl Process this file with autoconf to produce a configure script.
dnl $Id: configure.ac 5814 2004-02-07 09:09:49Z josef $

AC_INIT([GGZ GNOME client], [0.0.8], [ggz-dev@mail.ggzgamingzone.org], [ggz-gnome-client])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

PKG_CHECK_MODULES(GGZ, libgnomeui-2.0)
AC_SUBST(GGZ_CFLAGS)
AC_SUBST(GGZ_LIBS)

AC_PROG_CC
AC_PROG_INSTALL

if test "x${prefix}" = "xNONE"; then
  AC_DEFINE_UNQUOTED([PACKAGE_LOCALE_DIR],
		     "${ac_default_prefix}/share/locale",
                     [The location to put message catalogs])
else
  AC_DEFINE_UNQUOTED([PACKAGE_LOCALE_DIR],
		     "${prefix}/share/locale",
                     [The location to put message catalogs])
fi

AC_GGZ_INIT(defaults, export)
AC_GGZ_INTL([], [])
AC_GGZ_LIBGGZ
AC_GGZ_GGZMOD
AC_GGZ_GGZCORE
AC_GGZ_CONFIG

if test "x$XGETTEXT" = "x"; then
	nls_enabled=0
else
	nls_enabled=1
fi

AC_DEFINE_UNQUOTED([ENABLE_NLS], $nls_enabled,
		   [Define if your system supports NLS])

LDADD="$LDADD $LIB_GGZCORE $LIB_GGZ"
AC_SUBST(LDADD)

AC_CONFIG_FILES([
	Makefile
	pixmaps/Makefile
	motd-editor/Makefile
	src/Makefile
	po/Makefile
])
AC_OUTPUT

