dnl Process this file with autoconf to produce a configure script.
dnl $Id: configure.ac 5708 2003-12-06 02:56:30Z jdorje $

AC_INIT
AC_CONFIG_SRCDIR([configure.ac])

if test -r $srcdir/package; then
   . $srcdir/package
fi

DEFAULT_PACKAGE="ggz-gtk-games"
DEFAULT_VERSION="0.0.7"
DEFAULT_SUBDIRS="chess chinese-checkers combat dots ggzcards hastings lapocha reversi spades tictactoe"

dnl If an override is present, use it
dnl =================================
AC_ARG_WITH(game, [  --with-game=DIR         Dir for game module], game="$withval", game="")

dnl Order of precedence: command-line > OVERRIDE_XXX > DEFAULT_XXX
dnl ==============================================================

if test "$game" != ""; then
   subdirs=$game
   version=`cat $game/module.dsc | grep Version | sed 's/Version = //'`
   game=`cat $game/module.dsc | grep Name | sed 's/Name = //' | tr [A-Z] [a-z]`
   package=ggz-gtk-$game
elif test "$OVERRIDE_PACKAGE" != ""; then 
   package=$OVERRIDE_PACKAGE
   version=$OVERRIDE_VERSION
   subdirs=$OVERRIDE_SUBDIRS
else
   package=$DEFAULT_PACKAGE
   version=$DEFAULT_VERSION
   subdirs=$DEFAULT_SUBDIRS
fi

allsubdirs="common po $subdirs"

AM_INIT_AUTOMAKE($package, $version)
AM_CONFIG_HEADER([config.h])
AM_MAINTAINER_MODE

dnl Check for standard build environment
dnl ====================================

AC_PROG_CC
AC_PROG_CPP
AC_ISC_POSIX
AM_PROG_CC_STDC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB

dnl Compiling Options
dnl =================
AC_ARG_ENABLE(gtk, AC_HELP_STRING([--enable-gtk=[gtk1.2/gtk2]],
                                  [Specify GTK+-1.2 or GTK+-2.0 linkage]))

dnl Check for GGZ requirements
dnl ==========================
AC_GGZ_INIT(defaults, export)
AC_GGZ_LIBGGZ
AC_GGZ_GGZMOD
AC_GGZ_CONFIG
AC_GGZ_INTL(ignore)

dnl Check for header files
dnl ======================
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h libintl.h locale.h netdb.h netinet/in.h \
                  sys/socket.h], [],
                 [AC_MSG_ERROR([cannot find required header file])])
AC_HEADER_DIRENT
AC_HEADER_STDBOOL
AC_HEADER_SYS_WAIT


# Check for library functions
# ===========================
AC_CHECK_FUNCS([localeconv memset mkdir setlocale socket sqrt strcasecmp \
                strchr strdup strrchr strstr malloc realloc stat strtod \
                bzero floor gethostbyname],
               [], [AC_MSG_ERROR([cannot find required function])])
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STAT
AC_FUNC_STRTOD


dnl Check for gettext support
dnl =========================
AM_WITH_NLS
if test "${prefix}" = "NONE"; then
	prefix=${ac_default_prefix}
fi
AC_DEFINE_UNQUOTED([LOCALEDIR], ["{prefix}/share/locale"],
		   [The location to put message catalogs])

dnl Check for typedefs, structures, and compiler characteristics
dnl ============================================================
AC_C_CONST
AC_TYPE_SIZE_T
AC_TYPE_SIGNAL


dnl Check for libs
dnl ==============

# Check for GTK linkage

case "$enable_gtk" in
  gtk)    enable_gtk="gtk" ;;
  gtk1)   enable_gtk="gtk" ;;
  gtk1.2) enable_gtk="gtk" ;;
  gtk2)   enable_gtk="gtk2" ;;
  gtk2.0) enable_gtk="gtk2" ;;
  gtk20)  enable_gtk="gtk2" ;;
  *)      enable_gtk="yes" ;;
esac

if test "$enable_gtk" = "gtk" || test "$enable_gtk" = "yes"; then
  AM_PATH_GTK([1.2.0], [found_gtk="yes"], [found_gtk="no"])
  if test "$found_gtk" = "yes"; then
    enable_gtk=gtk
  elif test "$enable_gtk" = "gtk"; then
    AC_MSG_ERROR([*** Can't build GTK+-1.2 frontend: GTK+-1.2 not found])
  fi
fi

if test "$enable_gtk" = "gtk2" || test "$enable_gtk" = "yes"; then
  AM_PATH_GTK_2_0([2.0.0], [found_gtk2="yes"], [found_gtk2="no"])
  if test "$found_gtk2" = "yes"; then
    enable_gtk=gtk2
    AC_DEFINE(GTK2, 1, [Define to link against GTK+-2.0 libraries])
  elif test "$enable_gtk" = "gtk2"; then
    AC_MSG_ERROR([*** Can't build GTK+-2.0 frontend: GTK+-2.0 not found])
  fi
fi

if test "$enable_gtk" = "yes"; then
  AC_MSG_ERROR([*** Can't build GTK frontend: GTK not found])
fi


dnl Debug modes
dnl ===========
AC_GGZ_DEBUG


dnl Various dirs
dnl ============

SUBDIRS=$allsubdirs

AC_SUBST(LDADD)
AC_SUBST(subdirs)
AC_SUBST(SUBDIRS)

AC_CONFIG_FILES([Makefile 
	   package
	   common/Makefile
	   common/cardsets/Makefile
	   spades/Makefile
	   spades/module.dsc
	   tictactoe/Makefile	
	   tictactoe/module.dsc
	   dots/Makefile
	   dots/module.dsc
	   lapocha/Makefile
	   lapocha/module.dsc
	   ggzcards/Makefile
	   ggzcards/client/Makefile
	   ggzcards/common/Makefile
	   ggzcards/module.dsc
	   reversi/Makefile
	   reversi/module.dsc
	   combat/Makefile
	   combat/module.dsc
	   combat/pixmaps/Makefile
	   combat/pixmaps/default/Makefile
	   combat/maps/Makefile
	   hastings/Makefile
	   hastings/module.dsc
	   chinese-checkers/Makefile
	   chinese-checkers/module.dsc
	   chinese-checkers/pixmaps/Makefile
	   chess/Makefile
	   chess/module.dsc
	   chess/libcgc/Makefile
	   chess/bitmaps/Makefile
	   po/Makefile
	   ])
AC_OUTPUT


dnl Status Output
dnl =============
echo ""
echo "Options"
echo "    General Debugging..$enable_debug"
echo "    GDB Debugging......$enable_debug_gdb"
echo "    Gettext support....$enable_gettext"
echo "    GTK library........$enable_gtk"
echo ""
echo "Paths"
echo "    Installation prefix: $prefix"
echo "    Game executable directory: $ggzexecmoddir"
echo "    Game data directory: $ggzdatadir"
echo ""
echo "-------------------------"
echo "At the prompt type \"make\" to compile the game modules and"
echo "When complete, su to root and type \"make install\" to"
echo "install the client software."
echo ""
