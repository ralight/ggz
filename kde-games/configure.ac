dnl Configuration for GGZ-KDE-Games
dnl ===============================

AC_INIT([GGZ KDE games], [0.0.11pre], [ggz-dev@mail.ggzgamingzone.org], [ggz-kde-games])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

dnl Check for Qt and KDE
dnl ====================

KDE_SET_PREFIX
KDE_USE_QT(3)
AC_CHECK_COMPILERS
AC_ENABLE_SHARED(yes)
AC_ENABLE_STATIC(no)
AM_PROG_LIBTOOL
AC_PATH_KDE
AC_KNEWSTUFF

dnl KDE-what-have-I-done
dnl ====================

KDE_PLUGIN="-avoid-version -module -no-undefined \$(KDE_RPATH) \$(KDE_MT_LDFLAGS)"
AC_SUBST(KDE_PLUGIN)

dnl Check for GGZ requirements
dnl ==========================

AC_GGZ_INIT(defaults)
AC_GGZ_INTL
AC_GGZ_LIBGGZ
AC_GGZ_GGZMOD
AC_GGZ_CONFIG("true", ignore)

dnl Check for librsvg
dnl =================

AC_RSVG

dnl Be a bit more strict
dnl ====================

CXXFLAGS="$CXXFLAGS -DKDE_NO_COMPAT -DQT_NO_COMPAT -Wall"
AC_SUBST(CXXFLAGS)

dnl Create Makefiles
dnl ================

AC_CONFIG_FILES([Makefile
	KReversi/Makefile
	KReversi/module.dsc
	KReversi/pixmaps/Makefile
	KReversi/pixmaps/blend/Makefile
	KReversi/pixmaps/default/Makefile
	KReversi/pixmaps/gtk-client/Makefile
	krosswater/Makefile
	krosswater/module.dsc
	krosswater/zoneclient/Makefile
	krosswater/gfx/Makefile
	kdots/Makefile
	kdots/module.dsc
	kdots/Dot/Makefile
	ktictactux/Makefile
	ktictactux/module.dsc
	ktictactux/ktictactux.desktop
	ktictactux/themes/Makefile
	koenig/Makefile
	koenig/module.dsc
	koenig/libcgc/Makefile
	koenig/pics/Makefile
	koenig/pieces/Makefile
	muehle/Makefile
	muehle/module.dsc
	muehle/src/Makefile
	muehle/src/ai/Makefile
	muehle/src/ggzwrap/Makefile
	muehle/pics/Makefile
	muehle/pics/muehle.desktop
	muehle/pics/standard/Makefile
	muehle/pics/alt/Makefile
	muehle/variants/Makefile
	muehle/doc/Makefile
	keepalive/Makefile
	keepalive/module.dsc
	keepalive/keepalive.desktop
	keepalive/src/Makefile
	keepalive/pics/Makefile
	man/Makefile
	po/Makefile
	po/KReversi/Makefile
	po/ktictactux/Makefile
	po/kdots/Makefile
	po/krosswater/Makefile
	po/muehle/Makefile
	po/koenig/Makefile
	po/fyrdman/Makefile
	fyrdman/Makefile
	fyrdman/module.dsc
	fyrdman/data/Makefile
	fyrdman/data/fyrdman.desktop
	kcc/Makefile
	kcc/kcc.desktop
	kcc/themes/Makefile
	kcc/themes/default/Makefile
	kcc/module.dsc
	lib/Makefile])
AC_OUTPUT

dnl Report success
dnl ==============

echo ""
echo "GGZ-KDE-Games is now configured. Please type 'make' in order to compile it."
echo "The following installation layout is used:"
echo ""
echo "Generic installation prefix: $prefix (for libraries etc.)"
echo "Game executable directory: $ggzexecmoddir"
echo "Game data directory: $ggzdatadir"
echo ""
if test "x$GGZ_CONFIG" = "x/bin/true"; then
	echo "Warning: game information files will not be installed."
	echo "This should only happen during binary package creation."
	echo "Packages must install the .dsc files upon installation."
	echo ""
fi

