dnl Process this file with autoconf to produce a configure script.

AC_INIT([GGZ python package], [0.0.10pre], [ggz-dev@mail.ggzgamingzone.org], [ggz-python])
AC_CONFIG_SRCDIR([configure.ac])
#AC_CONFIG_HEADERS([config.h])
#AM_INIT_AUTOMAKE
#AM_MAINTAINER_MODE

dnl Compiling Options
dnl =====================
AC_ARG_ENABLE(ggzmod,   [  --disable-ggzmod   Exclude pyggzmod wrapper  ],, enable_ggzmod=yes)
AC_ARG_ENABLE(ggzdmod,  [  --disable-ggzdmod  Exclude pyggzdmod wrapper ],, enable_ggzdmod=yes)
AC_ARG_ENABLE(ggzcore,  [  --disable-ggzcore  Exclude pyggzcore wrapper ],, enable_ggzcore=yes)
AC_ARG_ENABLE(ggzchess, [  --disable-ggzchess Exclude pyggzchess wrapper],, enable_ggzchess=yes)
AC_ARG_ENABLE(rsvgsdl,  [  --disable-rsvgsdl  Exclude pyrsvgsdl wrapper ],, enable_rsvgsdl=yes)

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

AC_SUBST(prefix)
AC_SUBST(enable_ggzmod)
AC_SUBST(enable_ggzdmod)
AC_SUBST(enable_ggzcore)
AC_SUBST(enable_ggzchess)
AC_SUBST(enable_rsvgsdl)

AC_CONFIG_FILES([
	Makefile
	escape-sdl/module.dsc
	xadrez-chines/module.dsc
	xadrez-chines/xadrez-chines-server.dsc
	go/module.dsc
	go/goserver.dsc
	pyggzmod/setup.py
	pyggzdmod/setup.py
	pyggzcore/setup.py
])
AC_OUTPUT

dnl Status output
dnl =============

echo ""
echo "Configured python stuff. Type 'make' to compile it, followed by a 'make install'."
echo ""
echo "Build ggzmod wrapper: $enable_ggzmod"
echo "Build ggzdmod wrapper: $enable_ggzdmod"
echo "Build ggzcore wrapper: $enable_ggzcore"
echo "Chess wrapper for Python: $enable_ggzchess"
echo "RSVG wrapper for SDL: $enable_rsvgsdl"
echo ""

