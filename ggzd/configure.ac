# Configuration for the GGZ server
# ================================

AC_INIT([GGZ server], [snapshot-0.99.5+], [ggz-dev@mail.ggzgamingzone.org], [ggz-server])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

# Check for standard build environment
# ====================================
AC_PROG_CC
AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_INSTALL
AC_PROG_CPP
AC_PROG_LIBTOOL
AC_PROG_RANLIB
AC_PROG_MAKE_SET


# Compiling Options
# =================
AC_ARG_ENABLE([server],
	AS_HELP_STRING([--disable-server], [Don't build ggzd server]),
	[enable_server=no], [enable_server=yes])
AC_ARG_ENABLE([games],
	AS_HELP_STRING([--disable-games], [Don't build game servers]),
	[enable_games=no], [enable_games=yes])
AC_ARG_WITH([zeroconf],
	AS_HELP_STRING([--with-zeroconf=TYPE],
	[type of library to use: avahi]),
	[zeroconf=$withval], [zeroconf=yes])
AC_ARG_WITH([reconfiguration],
	AS_HELP_STRING([--with-reconfiguration=TYPE],
	[type of library to use: inotify, fam]),
	[reconfiguration=$withval], [reconfiguration=yes])
AC_ARG_WITH([unicode],
	AS_HELP_STRING([--with-unicode=TYPE],
	[type of library to use: icu]),
	[unicode=$withval], [unicode=yes])
AC_ARG_WITH([passwordcheck],
	AS_HELP_STRING([--with-passwordcheck=TYPE],
	[type of library to use: cracklib, omnicracklib]),
	[passwordcheck=$withval], [passwordcheck=yes])
AC_ARG_WITH([tcp-wrappers],
	AS_HELP_STRING([--with-tcp-wrappers],
	[Build with tcp-wrappers support]),
	[libwrap=yes], [libwrap=no])


# Check for GGZ Requirements
# ==========================
AC_GGZ_INIT(defaults, export)
AC_GGZ_INTL
AC_GGZ_LIBGGZ
AC_GGZ_VERSION(0, 99, 5)

# Check for header files
# ======================
AC_CHECK_HEADERS([arpa/inet.h fcntl.h limits.h netdb.h netinet/in.h \
                  sys/time.h syslog.h termios.h], [],
                 [AC_MSG_ERROR([cannot find required header file])])
AC_CHECK_HEADERS([stdint.h])
AC_HEADER_STDBOOL # FIXME: need header file support
AC_HEADER_TIME

# Check for typedefs, structures, and compiler characteristics
# ============================================================
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_SIGNAL
AC_TYPE_MODE_T
AC_STRUCT_TM
AC_CHECK_TYPES([uint8_t])
AC_C99_INITIALIZERS

# Check for PF_LOCAL/PF_UNIX
# ==========================
# There should be an autoconf macro to check this???
AC_MSG_CHECKING([for PF_LOCAL])
AC_EGREP_CPP(PF_LOCAL,
             [#include <sys/socket.h>
              PF_LOCAL ],
             [AC_MSG_RESULT(no)
              AC_MSG_CHECKING([for PF_UNIX])
              AC_EGREP_CPP(PF_UNIX, [ #include <sys/socket.h>
				      PF_UNIX ],
                           [AC_MSG_RESULT(no)
			    AC_MSG_ERROR([cannot find PF_LOCAL or PF_UNIX])],
                           [AC_MSG_RESULT(yes)
                            AC_DEFINE([PF_LOCAL], PF_UNIX,
				      [PF_UNIX is available])
			    AC_DEFINE([AF_LOCAL], AF_UNIX,
				      [AF_UNIX is available])])],
             AC_MSG_RESULT(yes))


# Check for library functions
# ===========================
save_cflags=$CFLAGS
CFLAGS="$CFLAGS -fno-builtin"
AC_CHECK_FUNCS([strchr strdup strerror strtoul uname bzero dup2 \
                gethostname gethostbyname gettimeofday localtime_r memset \
                mkdir rmdir select socket strcasecmp \
                closedir fork malloc realloc select stat strftime], [],
               [AC_MSG_ERROR([cannot find required function])])
CFLAGS=$save_cflags
AC_CHECK_FUNCS([sqrt]) # FIXME: for some reason this test fails !!!
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_STAT
AC_FUNC_STRFTIME


# Strsep is implemented in ggzd/strsep.c, this is used if needed.
AC_CHECK_FUNCS([strsep])
AH_VERBATIM([strsep],
[/* Implement strsep if it's not available (see ggzd/strsep.c). */
#ifndef HAVE_STRSEP
char* strsep(char **stringp, const char *delim);
#endif])

dnl Find pthread library
dnl ====================
AC_GGZ_PTHREADS


dnl Check for dynamic linker library
dnl ================================
AC_CHECK_LIB([dl], [dlopen], [LIB_DL="-ldl"], [LIB_DL=""])
AC_SUBST(LIB_DL)

dnl Check for expat
dnl ===============
AC_CHECK_LIB([expat], [XML_ParserCreate], [LIB_EXPAT="-lexpat"],
             [AC_MSG_ERROR([cannot find expat library])])
AC_CHECK_HEADER([expat.h], [],
                [AC_MSG_ERROR([cannot find expat.h header])])
AC_SUBST(LIB_EXPAT)

dnl Check for gcrypt
dnl ================
AC_CHECK_LIB([gcrypt], [gcry_check_version], [LIB_GCRYPT="-lgcrypt"],
             [AC_MSG_ERROR([cannot find gcrypt library])])
AC_CHECK_HEADER([gcrypt.h], [],
                [AC_MSG_ERROR([cannot find gcrypt.h header])])
AC_SUBST(LIB_GCRYPT)

dnl Check database libs
dnl ===================
AC_GGZ_DATABASE

dnl Check for zeroconf implementations
dnl ==================================
AC_GGZ_ZEROCONF
AC_GGZ_UNICODE
AC_GGZ_PASSWORDCHECK

dnl Check for reconfiguration support through file watchers
dnl =======================================================
AC_GGZ_RECONFIGURATION

dnl Check for tcp-wrappers support
dnl ==============================
AC_GGZ_LIBWRAP

dnl Debug modes
dnl ===========
AC_GGZ_DEBUG

dnl Versioning
dnl ==========
m4_include(game_servers/ggzcards/config/version.m4)

dnl ggzdmod settings
dnl ================
AM_CONDITIONAL(INSTALL_GGZDMOD, true)

dnl Various dirs
dnl ============

# See docs/ggz-project/packagers for FHS considerations
# We leave the values intact for explicit user overrides
if test "$prefix" = "/usr"; then
	if test "$localstatedir" = "\${prefix}/var"; then
		localstatedir="/var/lib"
	fi
	if test "$sysconfdir" = "\${prefix}/etc"; then
		sysconfdir="/etc"
	fi
elif test "$prefix" = "/usr/local"; then
	if test "$localstatedir" = "\${prefix}/var"; then
		localstatedir="/var/local"
	fi
	if test "$sbindir" = "\${exec_prefix}/sbin"; then
		sbindir="\${bindir}"
	fi
else
	if test "$sbindir" = "\${exec_prefix}/sbin"; then
		sbindir="\${bindir}"
	fi
fi

ggzdconfdir="$sysconfdir/ggzd"
ggzdexecmoddir="$libdir/ggzd"
ggzdstatedir="$localstatedir/ggzd"
ggzddatadir="$datadir/ggz/ggzd"
packagesrcdir=`cd $srcdir && pwd`
ggzdroomdir="$ggzdconfdir/rooms"
ggzdgamedir="$ggzdconfdir/games"

AC_SUBST(ggzdconfdir)
AC_SUBST(ggzdexecmoddir)
AC_SUBST(ggzdstatedir)
AC_SUBST(ggzddatadir)
AC_SUBST(packagesrcdir)
AC_SUBST(ggzdroomdir)
AC_SUBST(ggzdgamedir)
CPPFLAGS="$CPPFLAGS -DGGZDDATADIR=\\\"$ggzddatadir\\\""

AC_DEFINE([GGZ_SERVER], 1, [Tell preprocessor we're compiling server code])

dnl Which progs to compile
dnl ======================

AM_CONDITIONAL([COND_COMPILE_SERVER], [test "$enable_server" = yes])
AM_CONDITIONAL([COND_COMPILE_GAMES], [test "$enable_games" = yes])

dnl Attempt to use as much as we can on systems with glibc
AC_DEFINE([_GNU_SOURCE], 1,
	  [Attempt to use as much as we can on systems with glibc])

AC_GGZ_VISIBILITY

AC_CONFIG_FILES([Makefile
	ggzdmod/Makefile
	ggzdmod++/Makefile
	ggzd/Makefile
	ggzd/database/Makefile
	ggzd/database/schema/Makefile
	ggzd/meta/Makefile
	ggzd/ggzd.conf.example
	ggzd/ggzd.test
	game_servers/Makefile
	game_servers/tictactoe/Makefile
	game_servers/dots/Makefile
	game_servers/ggzcards/Makefile
	game_servers/ggzcards/games/Makefile
	game_servers/ggzcards/ai/Makefile
	game_servers/ggzcards/config/Makefile
	game_servers/ggzcards/config/ggzcards-bridge.dsc
	game_servers/ggzcards/config/ggzcards-euchre.dsc
	game_servers/ggzcards/config/ggzcards-fortytwo.dsc
	game_servers/ggzcards/config/ggzcards-hearts.dsc
	game_servers/ggzcards/config/ggzcards-lapocha.dsc
	game_servers/ggzcards/config/ggzcards-spades.dsc
	game_servers/ggzcards/config/ggzcards-suaro.dsc
	game_servers/ggzcards/config/ggzcards-sueca.dsc
	game_servers/ggzcards/config/ggzcards-whist.dsc
	game_servers/ggzcards/config/ggzcards.dsc
	game_servers/ggzcards/common/Makefile
	game_servers/ggzcards/client/Makefile
	game_servers/reversi/Makefile
	game_servers/combat/Makefile
	game_servers/hastings/Makefile
	game_servers/hastings/maps/Makefile
	game_servers/chess/Makefile
	game_servers/chess/libcgc/Makefile
	game_servers/chinese-checkers/Makefile
	game_servers/krosswater/Makefile
	game_servers/krosswater/stackpath/Makefile
	game_servers/krosswater/zoneserver/Makefile
	game_servers/muehle/Makefile
	game_servers/muehle/qtserv/Makefile
	game_servers/muehle/variants/Makefile
	game_servers/escape/Makefile
	game_servers/geekgame/Makefile
	game_servers/connectx/Makefile
	game_servers/widelands/Makefile
	man/Makefile
	tests/Makefile
	ggzdmod.pc
])
#AC_CONFIG_COMMANDS([default],[[]],[[]])
AC_OUTPUT


dnl Status Output
dnl =============

echo ""
echo "Build options"
echo "    Build Server.........$enable_server"
echo "    Build Game Servers...$enable_games"
echo "Configuration options"
echo "    pgsql database.......$pgsql"
echo "    mysql database.......$mysql"
echo "    sqlite database......$sqlite"
echo "    Zeroconf.............$zeroconf"
echo "    Reconfiguration......$reconfiguration"
echo "    Unicode..............$unicode"
echo "    Password checks......$passwordcheck"
echo "    Tcp wrappers.........$libwrap"
echo "Debugging options"
echo "    General Debuging.....$enable_debug"
echo "    GDB Debugging........$enable_debug_gdb"
echo "    Memory Debugging.....$enable_debug_mem"
echo " "
echo "To compile and install server:"
echo "    'make install'"
echo "To compile and test server locally without installing:"
echo "    'make test && ./ggzd/ggzd -F -f ./ggzd/ggzd.test'"

if test "$enable_visibility" = yes; then
	echo ""
	echo "*** WARNING ***"
	echo "You have enabled symbol visibility restrictions."
	echo "This feature hasn't been tested much yet."
fi
