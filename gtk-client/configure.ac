dnl Configuration for the Gtk+ core client
# $Id: configure.ac 10946 2009-09-30 05:35:44Z josef $
dnl ======================================

AC_INIT([GGZ Gtk+ client], [snapshot-0.99.5+], [ggz-dev@mail.ggzgamingzone.org], [ggz-gtk-client])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

dnl Check for standard build environment
dnl ====================================

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
AC_PROG_RANLIB


dnl Check for GGZ Requirements
dnl ==========================
AC_GGZ_INIT(defaults, export)
AC_GGZ_LIBGGZ
AC_GGZ_VERSION(0, 99, 5)
AC_GGZ_GGZMOD
AC_GGZ_GGZCORE
AC_GGZ_CONFIG("true", ignore, "ggz-gtk-client")
AC_GGZ_INTL


dnl Pidgin plugin check
dnl =================
AC_PIDGIN(2, 2, 0)

dnl Temporary: Check for windows resource compiler
#AC_PATH_PROG([WINDRES], [i586-mingw32msvc-windres], [])
#AM_CONDITIONAL(WIN32, test "x$WINDRES" != "x")

dnl Check for header files
dnl ======================
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_HEADER_TIME
AC_CHECK_HEADERS([arpa/inet.h netinet/in.h sys/time.h sys/socket.h winsock.h])
AC_CHECK_HEADERS([sys/stat.h unistd.h assert.h], [],
                 [AC_MSG_ERROR([cannot find required header file])])


# Check for library functions
# ===========================
# We don't use AC_FUNC_MALLOC or AC_FUNC_REALLOC.  See the manual.

AC_CHECK_LIB(m, sqrt, [LIBS="$LIBS -lm"])
AC_CHECK_LIB(socket, socket, [LIBS="$LIBS -lsocket"])
AC_CHECK_LIB(nsl, gethostbyname, [LIBS="$LIBS -lnsl"])
if test "$MINGW32" = "yes"; then
  LIBS="$LIBS -lwsock32"
fi

AC_CHECK_FUNCS([memset setlocale strcasecmp strchr strncasecmp strrchr \
                malloc realloc stat strdup strstr rand srand], [],
               [AC_MSG_ERROR([cannot find required function])])
AC_CHECK_FUNCS([execvp])
AC_FUNC_FORK
AC_FUNC_STAT
AC_FUNC_CLOSEDIR_VOID
FUNC_MKDIR_TAKES_ONE_ARG
AC_CHECK_FUNCS([localeconv memset mkdir setlocale strcasecmp \
                strchr strdup strrchr strstr malloc realloc stat \
	        sqrt floor rand srand],
               [], [AC_MSG_ERROR([cannot find required function])])
AC_CHECK_FUNCS([socket gethostbyname gettimeofday])


dnl Check for NLS
dnl ============================================================
AM_WITH_NLS
AC_DEFINE_UNQUOTED([PACKAGE_LOCALE_DIR], ["${prefix}/share/locale"],
                   [The location to put message catalogs])

dnl Check for typedefs, structures, and compiler characteristics
dnl ============================================================
AC_C_CONST
AC_C_INLINE
AC_PATH_X
AC_TYPE_SIZE_T
AC_TYPE_SIGNAL
AC_CHECK_TYPES([uint8_t])

dnl Check for libraries
dnl ===========================

AM_PATH_GTK_2_0([2.0.0], [],
		AC_MSG_ERROR([*** Can't build GTK+-2.0 frontend: GTK+-2.0 not found]))

dnl Debug modes
dnl ===========
AC_GGZ_DEBUG
if test "$enable_debug" = yes; then
  EXTRA_GAME_PARAMETERS="$EXTRA_GAME_PARAMETERS --g-fatal-warnings"
  AC_DEFINE([G_DISABLE_DEPRECATED], 1, [Disable deprecated glib functions])
  AC_DEFINE([GDK_DISABLE_DEPRACATED], 1, [Disable deprecated GDK functions])
#  AC_DEFINE([GTK_DISABLE_DEPRECATED], 1, [Disable deprecated GTK functions])
fi
AC_SUBST(EXTRA_GAME_PARAMETERS)

# Library settings
# ================
AM_CONDITIONAL(INSTALL_GGZ_GTK, true)
AM_CONDITIONAL([COND_PIDGIN], [test "$have_pidgin" = yes])
AC_GGZ_VISIBILITY

dnl Game options
dnl ========================================
AC_ARG_WITH(game, AC_HELP_STRING([--with-game=DIR],[Dir for game module]),
            game="$withval", game="")

AC_ARG_WITH(game-wrapper,
            AC_HELP_STRING([--with-game-wrapper=WRAPPER],
                           [Wrapper for running games (e.g., "valgrind")]),
            GAME_WRAPPER="$withval", GAME_WRAPPER="")
AC_SUBST(GAME_WRAPPER)

dnl Versioning
dnl ==========
m4_include(games/chess/version.m4)
m4_include(games/chinese-checkers/version.m4)
m4_include(games/combat/version.m4)
m4_include(games/dots/version.m4)
m4_include(games/ggzcards/version.m4)
m4_include(games/hastings/version.m4)
m4_include(games/reversi/version.m4)

dnl Various dirs
dnl ============
AC_SUBST(LDADD)

AC_CONFIG_FILES([Makefile 
	   ggz-gtk/Makefile
	   gtk-client/Makefile
	   pidgin-plugin/Makefile
	   po/Makefile
	   doc/Makefile
	   man/Makefile
	   games/Makefile 
	   games/common/Makefile
	   games/common/cardsets/Makefile
	   games/tictactoe/Makefile	
	   games/tictactoe/module.dsc
	   games/dots/Makefile
	   games/dots/module.dsc
	   games/ggzcards/Makefile
	   games/ggzcards/client/Makefile
	   games/ggzcards/common/Makefile
	   games/ggzcards/module.dsc
	   games/reversi/Makefile
	   games/reversi/module.dsc
	   games/combat/Makefile
	   games/combat/module.dsc
	   games/combat/pixmaps/Makefile
	   games/combat/pixmaps/default/Makefile
	   games/combat/maps/Makefile
	   games/hastings/Makefile
	   games/hastings/module.dsc
	   games/hastings/pixmaps/Makefile
	   games/chinese-checkers/Makefile
	   games/chinese-checkers/module.dsc
	   games/chinese-checkers/pixmaps/Makefile
	   games/chess/Makefile
	   games/chess/module.dsc
	   games/chess/libcgc/Makefile
	   games/chess/bitmaps/Makefile
	   games/po/Makefile
	   games/po/chess/Makefile
	   games/po/chinese-checkers/Makefile
	   games/po/combat/Makefile
	   games/po/common/Makefile
	   games/po/dots/Makefile
	   games/po/ggzcards/Makefile
	   games/po/hastings/Makefile
	   games/po/reversi/Makefile
	   games/po/tictactoe/Makefile
	   ])
AC_OUTPUT


dnl Status Output
dnl =============
echo ""
echo "Options"
echo "    General Debugging..$enable_debug"
echo "    GDB Debugging......$enable_debug_gdb"
echo "    Dmalloc Debugging..$enable_debug_mem"
echo "    Pidgin plugin......$have_pidgin"
echo ""
echo "Paths"
echo "    Installation prefix: $prefix"
echo "    Game executable directory: $ggzexecmoddir"
echo "    Game data directory: $ggzdatadir"
echo ""
echo "At the prompt type \"make\" to compile the GGZ gtk client"
echo ""
echo "When complete, su to root and type \"make install\" to"
echo "install the software."
echo ""
if test "x$GGZ_CONFIG" = "xtrue"; then
	echo "Warning: game information files will not be installed."
	echo "This should only happen during binary package creation."
	echo "Packages must install the .dsc files upon installation."
	echo ""
fi

if test "$enable_visibility" = yes; then
	echo ""
	echo "*** WARNING ***"
	echo "You have enabled symbol visibility restrictions."
	echo "This feature hasn't been tested much yet."
fi
