dnl Configuration for the GGZ Python package
dnl ========================================

AC_INIT([GGZ python package], [snapshot-0.99.2], [ggz-dev@mail.ggzgamingzone.org], [ggz-python])
AC_CONFIG_SRCDIR([configure.ac])
#AC_CONFIG_HEADERS([config.h])
#AM_INIT_AUTOMAKE
#AM_MAINTAINER_MODE

dnl Compilation Options
dnl ===================
AC_ARG_ENABLE(ggzmod,   [  --disable-ggzmod    Exclude pyggzmod wrapper  ],, enable_ggzmod=yes)
AC_ARG_ENABLE(ggzdmod,  [  --disable-ggzdmod   Exclude pyggzdmod wrapper ],, enable_ggzdmod=yes)
AC_ARG_ENABLE(ggzcore,  [  --disable-ggzcore   Exclude pyggzcore wrapper ],, enable_ggzcore=yes)
AC_ARG_ENABLE(ggzchess, [  --disable-ggzchess  Exclude pyggzchess wrapper],, enable_ggzchess=yes)
AC_ARG_ENABLE(rsvgsdl,  [  --disable-rsvgsdl   Exclude pyrsvgsdl wrapper ],, enable_rsvgsdl=yes)
AC_ARG_ENABLE(ancillary,[  --disable-ancillary Exclude libggz/easysock wrapper],, enable_ancillary=yes)

AC_ARG_WITH(rsvg-dir,
	AS_HELP_STRING([--with-rsvg-dir], [prefix of the rsvg library (default is /usr)]),
	[rsvgprefix=$withval],
	[rsvgprefix=""]
)

dnl Check for standard build environment
dnl ====================================
AC_PROG_CC
AC_PROG_MAKE_SET

dnl GGZ checks
dnl ==========
AC_GGZ_INIT([defaults], [export])
AC_GGZ_LIBGGZ(["true"], [disable_wrappers=true])
if test "x$disable_wrappers" != "xtrue"; then
	AC_GGZ_GGZMOD(["true"], [enable_ggzmod=no])
	AC_GGZ_GGZDMOD(["true"], [enable_ggzdmod=no])
	AC_GGZ_GGZCORE(["true"], [enable_ggzcore=no])
else
	enable_ggzmod=no
	enable_ggzdmod=no
	enable_ggzcore=no
	enable_ancillary=no
fi
AC_GGZ_CONFIG(["true"], [ignore])
AC_GGZ_INTL

dnl Other checks
dnl ============

python_found=no
check="import sys;"
check="$check print str(sys.version_info@<:@0@:>@)"
check="$check + \".\" +"
check="$check str(sys.version_info@<:@1@:>@)"
#distutils.sysconfig.get_python_lib()
result=`python -c "$check" 2>/dev/null || echo no`
if test "x$result" = "xno"; then
	python_found=no
else
	python_found="yes ($result)"
	PYVERSION=$result
fi
echo "checking for: python installation... $python_found"

if test "$python_found" = "no"; then
	echo "ERROR:"
	echo "Python has not been found."
	echo ""
	exit
fi

pygame_found=yes
result=`python -c "import pygame" 2>/dev/null || echo no`
if test "x$result" = "xno"; then
	pygame_found=no
fi
echo "checking for: pygame installation... $pygame_found"

if test "$enable_rsvgsdl" = "yes"; then
	#AC_CHECK_PROG([PKGCONFIG], ["pkg-config"], [pkgconfig=yes], [pkgconfig=no])
	# FIXME: we cannot use pkg-config since it doesn't support python-distutils

	if test "x$rsvgprefix" = "x"; then
		rsvgprefix=/usr
		rsvg_libraries="$rsvgprefix/lib"
		rsvg_includes="$rsvgprefix/include"
	else
		rsvg_libraries="$rsvgprefix/lib"
		rsvg_includes="$rsvgprefix/include"
	fi
	AC_SUBST(rsvg_libraries)
	AC_SUBST(rsvg_includes)

	save_cppflags=$CPPFLAGS
	save_ldflags=$LDFLAGS
	CPPFLAGS="$CPPFLAGS -I $rsvg_includes"
	LDFLAGS="$LDFLAGS -L $rsvg_libraries"

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
		AC_MSG_ERROR([The RSVG library is not or incompletely installed.])
	fi

	CPPFLAGS=$save_cppflags
	LDFLAGS=$save_ldflags
fi

dnl Create files
dnl ============

if test "x$xdg_datadir" = "x"; then
	xdg_datadir="\${prefix}/share/applications"
fi

if test "x$ggzd_confdir" = "x"; then
	ggzd_confdir="\${sysconfdir}/ggzd"
fi

AC_SUBST(prefix)
AC_SUBST(bindir)
AC_SUBST(datadir)
AC_SUBST(xdg_datadir)
AC_SUBST(ggzd_confdir)

AC_SUBST(enable_ggzmod)
AC_SUBST(enable_ggzdmod)
AC_SUBST(enable_ggzcore)
AC_SUBST(enable_ggzchess)
AC_SUBST(enable_rsvgsdl)
AC_SUBST(enable_ancillary)

AC_SUBST(PYVERSION)

AC_CONFIG_FILES([
	Makefile
	lib/ggzsettings.py
	escape-sdl/escape-sdl.desktop
	escape-sdl/module.dsc
	xadrez-chines/xadrez-chines.desktop
	xadrez-chines/module.dsc
	go/module.dsc
	vibora/vibora.desktop
	pyancillary/setup.py
	pyrsvgsdl/setup.py
	pyggzmod/setup.py
	pyggzdmod/setup.py
	pyggzcore/setup.py
	ggzboard/ggzboard.desktop
	ggzboard/module_go.dsc
	ggzboard/module_chess.dsc
	ggzboard/module_checkers.dsc
	ggzboard/module_reversi.dsc
	ggzboard/module_hnefatafl.dsc
	ggzboard/module_connectx.dsc
	ggzboard/module_ludo.dsc
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
echo "Build libggz/ancillary wrapper: $enable_ancillary"
echo "Chess wrapper for Python: $enable_ggzchess"
echo "RSVG wrapper for SDL: $enable_rsvgsdl"
echo ""

if test "$pygame_found" = "no"; then
	echo "WARNING:"
	echo "Pygame has not been found."
	echo "The wrappers will be compiled, but no game can be run."
	echo ""
fi
if test "x$GGZ_CONFIG" = "xtrue"; then
	echo "Warning: game information files will not be installed."
	echo "This should only happen during binary package creation."
	echo "Packages must install the .dsc files upon installation."
	echo ""
fi

