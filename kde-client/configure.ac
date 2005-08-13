dnl Configuration for KGGZ
dnl ======================

AC_INIT([GGZ KDE client], [0.0.11], [ggz-dev@mail.ggzgamingzone.org], [ggz-kde-client])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

dnl Options for building KGGZ
dnl =========================

AC_ARG_ENABLE(ggzcorepp, [  --disable-ggzcorepp  don't build GGZCore++              ], enable_ggzcorepp=no, enable_ggzcorepp=yes)
AC_ARG_ENABLE(browser,   [  --disable-browser    include KHTML browser [default=yes]], enable_browser=no, enable_browser=yes)
AC_ARG_ENABLE(ggzfinal,  [  --enable-ggzfinal    perform compile tests  [default=no]], enable_ggzfinal=yes, enable_ggzfinal=no)

dnl Check for Qt and KDE
dnl ====================

KDE_SET_PREFIX
KDE_USE_QT(3)
AC_CHECK_COMPILERS
AC_ENABLE_SHARED(yes)
AC_ENABLE_STATIC(no)
AM_PROG_LIBTOOL
AC_PATH_KDE

SUBDIRS="src data po doc"
COMPILEOPTIONS=""

dnl Check for easysock and ggzcore
dnl ==============================

AC_GGZ_INIT(defaults, export)
AC_GGZ_INTL
AC_GGZ_LIBGGZ
AC_GGZ_GGZCORE
AC_GGZ_CONFIG

if test "$enable_ggzcorepp" = yes; then
	SUBDIRS="ggzcore++ $SUBDIRS"
fi

dnl Find temporary categories_and_ratings patch
func=ggzcore_gametype_get_category
tmp=`grep $func $ggzcore_includes/ggzcore.h`
if test "$tmp" = ""; then
	patch_c_and_r=0
else
	patch_c_and_r=1
	AC_DEFINE_UNQUOTED([KGGZ_PATCH_C_AND_R], $patch_c_and_r, [Categories and ratings patch to ggzcore])
fi
echo "checking for GGZ patch 'Categories and Ratings' (C_and_R): $patch_c_and_r"

func=ggzcore_server_get_tls
tmp=`grep $func $ggzcore_includes/ggzcore.h`
if test "$tmp" = ""; then
	patch_encryption=0
else
	patch_encryption=1
	AC_DEFINE_UNQUOTED([KGGZ_PATCH_ENCRYPTION], $patch_encryption, [Encryption patch to ggzcore])
fi
echo "checking for GGZ patch 'Encryption' (Encryption): $patch_encryption"

#func=ggzcore_room_join_table_spectator
#tmp=`grep $func $ggzcore_includes/ggzcore.h`
#if test "$tmp" = ""; then
#	patch_spectators=0
#else
#	patch_spectators=1
#	AC_DEFINE_UNQUOTED([KGGZ_PATCH_SPECTATORS], $patch_spectators, [Spectators patch to ggzcore])
#fi
#echo "checking for GGZ patch 'Spectators' (Spectators): $patch_spectators"

echo "checking for GGZ patch 'Spectators' (Spectators): permanent"
AC_DEFINE_UNQUOTED([KGGZ_PATCH_SPECTATORS], 1, [Spectators patch to ggzcore])

dnl Check for zeroconf implementation Howl
dnl ======================================
AC_CHECK_LIB(howl, sw_discovery_init,
[
	AC_CHECK_HEADERS([howl/howl_config.h],
	[
		AC_DEFINE(WITH_HOWL, 1, [Use howl for zeroconf mechanism])
		HOWL_INCLUDES="-I/usr/include/howl"
		LIB_HOWL="-lhowl -lpthread"
		AC_SUBST(HOWL_INCLUDES)
		AC_SUBST(LIB_HOWL)
	],
	[
		AC_MSG_WARN([Zeroconf diabled, libhowl (headers) not found.])
	])
],
[
	AC_MSG_WARN([Zeroconf disabled, libhowl (libs) not found.])
],
[-lpthread])

dnl Set up variables for KGGZ
dnl =========================

if test "$enable_browser" = yes; then
	kggzbrowservar=1;
	BROWSERSUBDIR="more";
	BROWSERLIB="\$(top_builddir)/ggz-kde/src/widgets/more/libkggzuimore.a -L\$(kde_libraries) \$(LIB_KHTML)";
	AC_DEFINE_UNQUOTED([KGGZ_BROWSER], ${kggzbrowservar}, [Build embedded browser])
else
	kggzbrowservar=;
	BROWSERSUBDIR="";
	BROWSERLIB="";
fi

dnl Test for compilation options
dnl ============================

if test "$enable_final" = yes; then
	enable_final=yes
else
	enable_final=no
fi
if test "$enable_ggzfinal" = yes; then
	if test "$enable_final" = no; then
		COMPILEOPTIONS="$COMPILEOPTIONS -Wall -pedantic -ansi -Wpointer-arith"
		COMPILEOPTIONS="$COMPILEOPTIONS -Wmissing-prototypes -Wwrite-strings"
		dnl Do not use -Wshadow here! Qt will say thank you :)
	fi
fi

#AC_DEFINE_UNQUOTED(KGGZ_DIRECTORY, "${kde_datadir}/kggz")
if test "x${prefix}" = "xNONE"; then
	prefix=/usr/local
fi
AC_DEFINE_UNQUOTED([KGGZ_DIRECTORY], "${prefix}/share/apps/kggz", [Data directory])
AC_DEFINE_UNQUOTED([PREFIX], "${prefix}", [Prefix directory])

dnl Be a bit more strict
dnl ====================
CXXFLAGS="$CXXFLAGS -DKDE_NO_COMPAT -DQT_NO_COMPAT -Wall"
COMPILEOPTIONS="$COMPILEOPTIONS $CXXFLAGS"

AC_SUBST(CXXFLAGS)
AC_SUBST(SUBDIRS)
AC_SUBST(COMPILEOPTIONS)
AC_SUBST(BROWSERSUBDIR)
AC_SUBST(BROWSERLIB)

dnl Create Makefiles
dnl ================

AC_CONFIG_FILES([Makefile
	kgrubby/Makefile
	kgrubby/src/Makefile
	kgrubby/data/Makefile
	shadowbridge/Makefile
	shadowbridge/pics/Makefile
	ggzap/Makefile
	kio_ggz/Makefile
	kio_ggz/kioslave/Makefile
	kio_ggzmeta/Makefile
	kio_ggzmeta/kioslave/Makefile
	kcm_ggz/Makefile
	kcm_ggz/modules/Makefile
	compcal/Makefile
	compcal/src/Makefile
	compcal/src/lib/Makefile
	compcal/data/Makefile
	keepalivecontrol/Makefile
	keepalivecontrol/src/Makefile
	keepalivecontrol/data/Makefile
	ggz-kde/Makefile
	ggz-kde/ggzcore++/Makefile
	ggz-kde/ggzcore++/src/Makefile
	ggz-kde/ggzcore++/doc/Makefile
	ggz-kde/ggzcore++/doc/html/Makefile
	ggz-kde/src/Makefile
	ggz-kde/src/widgets/Makefile
	ggz-kde/src/widgets/more/Makefile
	ggz-kde/src/abstract/Makefile
	ggz-kde/data/Makefile
	ggz-kde/data/browser/Makefile
	ggz-kde/data/icons/Makefile
	ggz-kde/data/icons/games/Makefile
	ggz-kde/data/icons/players/Makefile
	ggz-kde/data/kde/Makefile
	ggz-kde/data/splash/Makefile
	ggz-kde/po/Makefile
	ggz-kde/doc/Makefile
	ggz-kde/doc/de/Makefile
	po/Makefile
	po/ggzap/Makefile
	po/shadowbridge/Makefile
	po/kcm_ggz/Makefile
	po/compcal/Makefile
	po/kgrubby/Makefile
	])
AC_OUTPUT

dnl Report success
dnl ==============

echo ""
echo "KGGZ is now configured. Please type 'make' in order to compile it."
echo "The following installation layout is used:"
echo ""
echo "Installation prefix: $prefix"
echo "Data: $prefix/share/apps/kggz/"
echo "Build included GGZCore++: $enable_ggzcorepp"
echo "Build optional KGGZBrowser: $enable_browser"
echo "Perform additional compile tests: $enable_ggzfinal (KDE: $enable_final)"
echo ""

