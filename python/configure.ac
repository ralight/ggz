dnl Process this file with autoconf to produce a configure script.

AC_INIT([GGZ python package], [0.0.11pre], [ggz-dev@mail.ggzgamingzone.org], [ggz-python])
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
AC_GGZ_CONFIG("true", ignore)
AC_GGZ_INTL

dnl Other checks
dnl ============

pygame_found=yes
result=`python -c "import pygame" 2>/dev/null || echo no`
if test "x$result" = "xno"; then
	pygame_found=no
fi
echo "checking for: pygame installation... $python_found"

if test "$enable_rsvgsdl" = "yes"; then
	AC_CHECK_LIB(
		[rsvg-2],
		[rsvg_pixbuf_from_file_at_size],
		[rsvg_found=yes],
		[rsvg_found=no]
	)
	if test "$rsvg_found" = "yes"; then
		AC_CHECK_HEADER(
			[librsvg-2/librsvg/librsvg-features.h],
			[rsvg_found=yes],
			[rsvg_found=no]
		)
	fi
	if test "$rsvg_found" = "no"; then
		AC_MSG_ERROR("RSVG is not or incompletely installed.")
	fi
fi

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
	escape-sdl/escape-sdl.desktop
	escape-sdl/module.dsc
	xadrez-chines/xadrez-chines.desktop
	xadrez-chines/module.dsc
	xadrez-chines/xadrez-chines-server.dsc
	go/module.dsc
	go/goserver.dsc
	pyggzmod/setup.py
	pyggzdmod/setup.py
	pyggzcore/setup.py
	ggzboard/ggzboard.desktop
	ggzboard/module_go.dsc
	ggzboard/module_chess.dsc
	ggzboard/module_checkers.dsc
	ggzboard/module_reversi.dsc
	ggzboard/module_hnefatafl.dsc
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
if test "$pygame_found" = "no"; then
	echo "WARNING:"
	echo "Pygame has not been found."
	echo "The wrappers will be compiled, but no game can be run."
	echo ""
fi
if test "x$GGZ_CONFIG" = "x/bin/true"; then
	echo "Warning: game information files will not be installed."
	echo "This should only happen during binary package creation."
	echo "Packages must install the .dsc files upon installation."
	echo ""
fi

