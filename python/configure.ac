dnl Process this file with autoconf to produce a configure script.

AC_INIT([GGZ python package], [0.0.9pre], [ggz-dev@mail.ggzgamingzone.org], [ggz-python])
AC_CONFIG_SRCDIR([configure.ac])
#AC_CONFIG_HEADERS([config.h])
#AM_INIT_AUTOMAKE
#AM_MAINTAINER_MODE

dnl Compiling Options
dnl =====================
#AC_ARG_ENABLE(ggzdmod,[  --disable-ggzdmod  Exclude pyggzdmod wrapper],,enable_ggzdmod=yes)

dnl Check for standard build environment
dnl ====================================
AC_PROG_CC
AC_PROG_MAKE_SET

dnl GGZ checks
dnl ==========
AC_GGZ_INIT(defaults)
AC_GGZ_LIBGGZ
AC_GGZ_GGZMOD("true", ignore)
AC_GGZ_GGZDMOD("true", ignore)
AC_GGZ_GGZCORE("true", ignore)
AC_GGZ_CONFIG

dnl Create files
dnl ============

if test "x$prefix" = "xNONE"; then
	prefix="/usr/local"
fi

AC_CONFIG_FILES([
	escape-sdl/module.dsc
	xadrez-chines/module.dsc
	xadrez-chines/xadrez-chines-server.dsc
	go/module.dsc
	go/goserver.dsc
	pyggzcore/setup.py
])
AC_OUTPUT

dnl Status output
dnl =============

echo ""
echo "Configured python stuff. Type 'make' to compile it, followed by a 'make install'."
echo ""

