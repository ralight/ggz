dnl Process this file with autoconf to produce a configure script.
dnl $Id: configure.ac 6920 2005-02-04 16:13:04Z josef $

AC_INIT([GGZ GTK games], [0.0.11pre], [ggz-dev@mail.ggzgamingzone.org], [gtk-games])
AC_CONFIG_SRCDIR([configure.ac])

if test -r $srcdir/package; then
   . $srcdir/package
fi

DEFAULT_PACKAGE="ggz-gtk-games"
DEFAULT_VERSION="0.0.10"
DEFAULT_SUBDIRS="chess chinese-checkers combat dots ggzcards hastings reversi spades tictactoe"

dnl If an override is present, use it
dnl =================================
AC_ARG_WITH(game, AC_HELP_STRING([--with-game=DIR],[Dir for game module]),
            game="$withval", game="")

AC_ARG_WITH(game-wrapper,
            AC_HELP_STRING([--with-game-wrapper=WRAPPER],
                           [Wrapper for running games (e.g., "valgrind")]),
            GAME_WRAPPER="$withval", GAME_WRAPPER="")
AC_SUBST(GAME_WRAPPER)

dnl Order of precedence: command-line > OVERRIDE_XXX > DEFAULT_XXX
dnl ==============================================================

if test "$game" != ""; then
   subdirs=$game
   version=`cat $game/module.dsc | grep Version | grep -v ProtocolVersion | sed 's/Version = //'`
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

allsubdirs="common $subdirs po"

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

# Special case for compiling on windows
AC_CANONICAL_HOST
case $host_os in
  *mingw32* ) MINGW32=yes;;
          * ) MINGW32=no;;
esac

dnl Compiling Options
dnl =================

dnl Check for GGZ requirements
dnl ==========================
AC_GGZ_INIT(defaults, export)
AC_GGZ_LIBGGZ
AC_GGZ_GGZMOD
AC_GGZ_CONFIG("true", ignore)
AC_GGZ_INTL

dnl Check for header files
dnl ======================
AC_HEADER_STDC
AC_HEADER_TIME
AC_HEADER_DIRENT
AC_HEADER_STDBOOL
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([fcntl.h libintl.h locale.h], [],
                 [AC_MSG_ERROR([cannot find required header file])])
AC_CHECK_HEADERS([netdb.h netinet/in.h socket.h winsock.h sys/time.h \
		  sys/socket.h stdint.h])


# Check for library functions
# ===========================
# We don't use AC_FUNC_MALLOC or AC_FUNC_REALLOC.  See the manual.
AC_CHECK_LIB(m, sqrt, [LIBS="$LIBS -lm"])
AC_CHECK_LIB(socket, socket, [LIBS="$LIBS -lsocket"])
AC_CHECK_LIB(nsl, gethostbyname, [LIBS="$LIBS -lnsl"])
if test "$MINGW32" = "yes"; then
  LIBS="$LIBS -lwsock32"
fi

AC_FUNC_STAT
AC_FUNC_STRTOD
AC_FUNC_CLOSEDIR_VOID
FUNC_MKDIR_TAKES_ONE_ARG
AC_CHECK_FUNCS([localeconv memset mkdir setlocale strcasecmp \
                strchr strdup strrchr strstr malloc realloc stat \
	        sqrt floor rand srand],
               [], [AC_MSG_ERROR([cannot find required function])])
AC_CHECK_FUNCS([socket gethostbyname gettimeofday])


dnl Check for gettext support
dnl =========================
AM_WITH_NLS
if test "${prefix}" = "NONE"; then
	prefix=${ac_default_prefix}
fi
AC_DEFINE_UNQUOTED([LOCALEDIR], ["${prefix}/share/locale"],
		   [The location to put message catalogs])

dnl Check for typedefs, structures, and compiler characteristics
dnl ============================================================
AC_C_CONST
AC_TYPE_SIZE_T
AC_TYPE_SIGNAL
AC_CHECK_TYPES([uint8_t])


dnl Check for libs
dnl ==============

# Check for GTK linkage
AM_PATH_GTK_2_0([2.0.0], [],
    [AC_MSG_ERROR([*** Can't build GTK+-2.0 frontend: GTK+-2.0 not found])])

dnl Debug modes
dnl ===========
AC_GGZ_DEBUG
if test "$enable_debug" = yes; then
  EXTRA_GAME_PARAMETERS="$EXTRA_GAME_PARAMETERS --g-fatal-errors"
  EXTRA_GAME_PARAMETERS="$EXTRA_GAME_PARAMETERS --g-fatal-warnings"
  AC_DEFINE([G_DISABLE_DEPRECATED], 1, [Disable deprecated glib functions])
  AC_DEFINE([GDK_DISABLE_DEPRACATED], 1, [Disable deprecated GDK functions])
fi
if test "yes" = "no"; then
  AC_DEFINE([GTK_DISABLE_DEPRECATED], 1, [Disable deprecated GTK functions])
fi
AC_SUBST(EXTRA_GAME_PARAMETERS)

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
	   hastings/pixmaps/Makefile
	   chinese-checkers/Makefile
	   chinese-checkers/module.dsc
	   chinese-checkers/pixmaps/Makefile
	   chess/Makefile
	   chess/module.dsc
	   chess/libcgc/Makefile
	   chess/bitmaps/Makefile
	   po/Makefile
	   po/chess/Makefile
	   po/chinese-checkers/Makefile
	   po/combat/Makefile
	   po/common/Makefile
	   po/dots/Makefile
	   po/ggzcards/Makefile
	   po/hastings/Makefile
	   po/reversi/Makefile
	   po/spades/Makefile
	   po/tictactoe/Makefile
	   ])
AC_OUTPUT


dnl Status Output
dnl =============
echo ""
echo "Options"
echo "    General Debugging..$enable_debug"
echo "    GDB Debugging......$enable_debug_gdb"
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
if test "x$GGZ_CONFIG" = "x/bin/true"; then
	echo "Warning: game information files will not be installed."
	echo "This should only happen during binary package creation."
	echo "Packages must install the .dsc files upon installation."
	echo ""
fi
