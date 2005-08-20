dnl Process this file with autoconf to produce a configure script.
dnl $Id: configure.ac 7453 2005-08-20 12:55:32Z josef $

AC_INIT([GGZ server], [0.0.11], [ggz-dev@mail.ggzgamingzone.org], [ggz-server])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

# Check for standard build environment
# ====================================
AC_PROG_CC
AC_PROG_CXX
AC_ISC_POSIX
AC_PROG_INSTALL
AC_PROG_CPP
AM_PROG_LIBTOOL
AC_PROG_RANLIB
AM_PROG_CC_STDC
AC_PROG_MAKE_SET


# Compiling Options
# =================
AC_ARG_ENABLE([server],
	      AC_HELP_STRING([--disable-server],[Don't build ggzd server]),
	      [enable_server=no], [enable_server=yes])
AC_ARG_ENABLE([games],
	      AC_HELP_STRING([--disable-games],[Don't build ggzd games]),
	      [enable_games=no], [enable_games=yes])
AC_ARG_WITH([tmpdir],
	    AC_HELP_STRING([--with-tmpdir=PATH],
		[directory for temporary files (default=/tmp/ggzd)]),
	    [tmpdir=$withval], [tmpdir="/tmp/ggzd"])
AC_ARG_WITH([database],
	    AC_HELP_STRING([--with-database=DB],
		[type of database to use: db2, db3, db4, pgsql, mysql, sqlite]),
	    [database=$withval], [database=yes])


# Check for GGZ Requirements
# ==========================
AC_GGZ_INIT(defaults, export)
AC_GGZ_INTL
AC_GGZ_LIBGGZ

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
AC_CHECK_FUNCS([strchr strdup strerror strtoul uname bzero dup2 \
                gethostname gethostbyname gettimeofday localtime_r memset \
                mkdir rmdir select socket strcasecmp \
                closedir fork malloc realloc select stat strftime], [],
               [AC_MSG_ERROR([cannot find required function])])
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
AC_CHECK_HEADER(pthread.h,
    [AC_CHECK_LIB([pthread], [pthread_create],
        [ggzd_LIBS="$ggzd_LIBS -lpthread"
         CFLAGS="$CFLAGS -D_REENTRANT"],
    [AC_CHECK_LIB(c_r, pthread_create,
        [ggzd_LIBS="$ggzd_LIBS -pthread"
         CFLAGS="$CFLAGS -D_THREAD_SAFE"],
            AC_MSG_ERROR([cannot find pthread library]))])],
    AC_MSG_ERROR([cannot find pthread.h header]))


dnl Check for expat
dnl ===============
AC_CHECK_LIB([expat], [XML_ParserCreate], [ggzd_LIBS="$ggzd_LIBS -lexpat"],
             [AC_MSG_ERROR([cannot find expat library])])
AC_CHECK_HEADER([expat.h], [],
                [AC_MSG_ERROR([cannot find expat.h header])])

dnl Check database libs
dnl ===================
AC_GGZ_DATABASE

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


dnl Debug modes
dnl ===========
AC_GGZ_DEBUG


dnl Various dirs
dnl ============

if test "x${sysconfdir}" = 'x${prefix}/etc'; then
  if test "x${prefix}" = "xNONE"; then
    ggzdconfdir="${ac_default_prefix}/etc/ggzd"
  else
    ggzdconfdir="${prefix}/etc/ggzd"
  fi
else
  ggzdconfdir="${sysconfdir}/ggzd"
fi
AC_DEFINE_UNQUOTED([GGZDCONFDIR], "${ggzdconfdir}",
		   [Directory containing GGZd configuration files])
ggzdroomdir="${ggzdconfdir}/rooms"
ggzdgamedir="${ggzdconfdir}/games"


if test "x${libdir}" = 'x${exec_prefix}/lib'; then
  if test "x${exec_prefix}" = "xNONE"; then
    if test "x${prefix}" = "xNONE"; then
      ggzdexecmoddir="${ac_default_prefix}/lib/ggzd"
    else
      ggzdexecmoddir="${prefix}/lib/ggzd"
    fi
  else
    ggzdexecmoddir="${exec_prefix}/lib/ggzd"
  fi
else
  ggzdexecmoddir="${libdir}/ggzd"
fi
AC_DEFINE_UNQUOTED(GAMEDIR, "${ggzdexecmoddir}",
		   [Directory containing GGZd game module executables])

if test "x${localstatedir}" = 'x${prefix}/var'; then
  if test "x${prefix}" = "xNONE"; then
    ggzdstatedir="${ac_default_prefix}/var/lib/ggzd"
  else
    ggzdstatedir="${prefix}/var/lib/ggzd"
  fi
else
  ggzdstatedir="${localstatedir}/ggzd"
fi
AC_DEFINE_UNQUOTED([DATADIR], "${ggzdstatedir}",
		   [Directory containing GGZ shared data])

AC_DEFINE_UNQUOTED([TMPDIR], "${tmpdir}",
		   [Directory used for temporary storage])

if test "x${prefix}" = "xNONE"; then
  ggzddatadir="${ac_default_prefix}/share/ggzd"
else
  ggzddatadir="${prefix}/share/ggzd"
fi
AC_DEFINE_UNQUOTED([GGZDDATADIR], "${ggzddatadir}",
		   [Location of GGZd module data])

packagesrcdir=`cd $srcdir && pwd`
AC_DEFINE_UNQUOTED([PACKAGE_SOURCE_DIR], "${packagesrcdir}",
		   [The top-level source directory])


dnl Which progs to compile
dnl ======================

COMPILE_SUBDIRS="man ggzdmod ggzdmod++"

if test "$enable_server" = yes; then
    COMPILE_SUBDIRS="$COMPILE_SUBDIRS ggzd"
fi

if test "$enable_games" = yes; then
    COMPILE_SUBDIRS="$COMPILE_SUBDIRS game_servers"
fi

dnl Compile the test code
COMPILE_SUBDIRS="$COMPILE_SUBDIRS tests"

AC_SUBST(LDADD)
AC_SUBST(ggzd_LIBS)
AC_SUBST(COMPILE_SUBDIRS)
AC_SUBST(ggzdconfdir)
AC_SUBST(ggzdroomdir)
AC_SUBST(ggzdgamedir)
AC_SUBST(ggzddatadir)
AC_SUBST(ggzdexecmoddir)
AC_SUBST(ggzdstatedir)
AC_SUBST(packagesrcdir)
AC_SUBST(database)

dnl Attempt to use as much as we can on systems with glibc
AC_DEFINE([_GNU_SOURCE], 1,
	  [Attempt to use as much as we can on systems with glibc])

AC_CONFIG_FILES([Makefile
	ggzdmod/Makefile
	ggzdmod++/Makefile
	ggzd/Makefile 
	ggzd/database/Makefile
	ggzd/ggzd.conf
	ggzd/ggzd.test
	game_servers/Makefile
	game_servers/spades/Makefile
	game_servers/tictactoe/Makefile
	game_servers/dots/Makefile
	game_servers/ggzcards/Makefile
	game_servers/ggzcards/games/Makefile
	game_servers/ggzcards/ai/Makefile
	game_servers/ggzcards/config/Makefile
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
	game_servers/keepalive/Makefile
	game_servers/keepalive/src/Makefile
	game_servers/geekgame/Makefile
	game_servers/connectx/Makefile
	game_servers/tuxman/Makefile
	game_servers/widelands/Makefile
	man/Makefile
	tests/Makefile
])
AC_OUTPUT


dnl Status Output
dnl =============

echo "Options"  
echo "    Build Server.........$enable_server" 
echo "    Build Game Servers...$enable_games" 
echo "    Database Type........$database" 
echo "    General Debuging.....$enable_debug" 
echo "    GDB Debugging........$enable_debug_gdb" 
echo "    Memory Debugging.....$enable_debug_mem" 
echo " " 
echo "To compile and install server:" 
echo "    'make install'" 
echo "To compile and test server locally without installing:" 
echo "    'make test && ./ggzd/ggzd -F -f ./ggzd/ggzd.test'"
