dnl Process this file with autoconf to produce a configure script.
dnl $Id: configure.ac 5790 2004-02-05 10:46:53Z josef $

AC_INIT([GGZ server], [0.0.8], [ggz-dev@mail.ggzgamingzone.org], [ggz-server])
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
		[type of database to use: db2, db3, db4, pgsql, mysql]),
	    [database=$withval], [database=yes])


# Check for GGZ Requirements
# ==========================
AC_GGZ_INIT(defaults, export)
AC_GGZ_LIBGGZ

# Check for header files
# ======================
AC_CHECK_HEADERS([arpa/inet.h fcntl.h limits.h netdb.h netinet/in.h \
                  sys/time.h syslog.h termios.h], [],
                 [AC_MSG_ERROR([cannot find required header file])])
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


# Check for popt
# ==============
AC_CHECK_LIB([popt], [poptGetNextOpt], [ggzd_LIBS="$ggzd_LIBS -lpopt"],
             [AC_MSG_ERROR([cannot find popt library])])
AC_CHECK_HEADER([popt.h], [],
                [AC_MSG_ERROR([cannot find popt.h header])])

# Strsep is implemented in ggzd/strsep.c, this is used if needed.
AC_REPLACE_FUNCS([strsep])
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
dnl Check for available/requested databases.
case "$database" in
  db4)   database=db4 ;;
  db3)   database=db3 ;;
  db2)   database=db2 ;;
  pgsql) database=pgsql ;;
  mysql) database=mysql ;;
  *)     database=yes ;;
esac

# Order of preference: db3, db4, db2, PgSQL, MySQL.  This is determined
# solely by the order of the checks below.  Since a correct db will be
# auto-detected, we should order these solely based on how good they are
# (in terms of stability and performance).

# Check for db3 database
if test "$database" = db3 || test "$database" = yes; then
  AC_CHECK_LIB(db, db_env_create,
    [
      AC_CHECK_HEADER(db.h,
        [
          database=db3
          ggzd_LIBS="$ggzd_LIBS -ldb"
        ],
        [
          if test "$database" = db3; then
            AC_MSG_ERROR([cannot configure db3 (db3-dev needed)])
          fi
        ])
    ],
    [
      AC_CHECK_LIB(db3, db_env_create,
        [
          AC_CHECK_HEADER([db3/db.h],
            [
              database=db3
              ggzd_LIBS="$ggzd_LIBS -ldb3"
              AC_DEFINE([DB3_IN_DIR], 1,
			[Define if the db3 libs and headers are under db3/])
            ],
            [
              if test "$database" = db3; then
                AC_MSG_ERROR([cannot configure db3 (db3-dev needed)])
              fi
            ])
        ],
        [
          if test "$database" = db3; then
            AC_MSG_ERROR([cannot configure db3 (libdb3 needed)])
          fi
        ])
    ])
fi

# Check for db4 database
if test "$database" = db4 || test "$database" = yes; then
  db4lib=""
  db4inc=""
  AC_CHECK_LIB(db-4.2, db_env_create_4002,
    [
      db4lib="-ldb-4.2"
      database=db4
    ],
    [
      AC_CHECK_LIB(db-4.1, db_env_create_4001,
        [
          db4lib="-ldb-4.1"
          database=db4
        ],
        [
          AC_CHECK_LIB(db, db_env_create_4000,
            [
              db4lib="-ldb"
              database=db4
            ],
            [
              AC_CHECK_LIB(db-4.0, db_env_create_4000,
                [
                  db4lib="-ldb-4.0"
                  database=db4
                ],
                [
                  if test "$database" = db4; then
                    AC_MSG_ERROR([cannot configure db4 (libdb4 needed)])
                  fi
                ])
            ])
        ])
    ])
  if test "x$db4lib" != "x-ldb-4.1" && test "x$db4lib" != "x-ldb-4.2"; then
    AC_CHECK_HEADER(db4/db.h,
      [
        db4inc="db4/db.h"
        database=db4
        AC_DEFINE([DB4_IN_DIR], 1,
        [Define if the db4 libs and headers are under db4/])
      ],
      [
        if test "$database" = db4; then
          AC_MSG_ERROR([cannot configure db4 (db4-dev needed)])
        fi
      ])
  fi
  AC_CHECK_HEADER(db.h,
    [
      db4inc="db.h"
      database=db4
    ],
    [
      if test "$database" = db4; then
        AC_MSG_ERROR([cannot configure db4 (db4-dev needed)])
      fi
    ])

  if test "x$db4inc" != "x"; then
    ggzd_LIBS="$ggzd_LIBS $db4lib"
  fi
fi

# Check for db2 database
if test "$database" = db2 || test "$database" = yes; then
  AC_CHECK_LIB(db, db_appinit,
    [
      AC_CHECK_HEADER(db.h,
        [
          database=db2
          ggzd_LIBS="$ggzd_LIBS -ldb"
        ],
        [
          if test "$database" = db2; then
            AC_MSG_ERROR([cannot configure db2 (db2-dev needed)])
          fi
        ])
    ],
    [
      AC_CHECK_LIB(db2, db_appinit,
        [
          AC_CHECK_HEADER(db2/db.h,
            [
              database=db2
              ggzd_LIBS="$ggzd_LIBS -ldb2"
              AC_DEFINE([DB2_IN_DIR], 1,
			[Define if the db2 libs and headers are under db2/])
            ],
            [
              if test "$database" = db2; then
                AC_MSG_ERROR([cannot configure db2 (db2-dev needed)])
              fi
            ])
        ],
        [
          if test "$database" = db2; then
            AC_MSG_ERROR([cannot configure db2 (libdb2 needed)])
          fi
        ])
    ])
fi

# Check for PgSQL database
if test "$database" = pgsql || test "$database" = yes; then
  AC_CHECK_LIB(pq, PQconnectdb,
    [
      AC_CHECK_HEADER([postgresql/libpq-fe.h],
        [
          database=pgsql
          ggzd_LIBS="$ggzd_LIBS -lpq"
        ],
        [
          if test "$database" = pgsql; then
            AC_MSG_ERROR([cannot configure pgsql (pgsql-dev library needed)])
          fi
        ])
    ],
    [
      if test "$database" = pgsql; then
        AC_MSG_ERROR([cannot configure pgsql (pgsql library needed)])
      fi
    ])
fi


# Check for MySQL database
if test "$database" = mysql || test "$database" = yes; then
  AC_CHECK_LIB(mysqlclient_r, mysql_connect,
    [
      AC_CHECK_HEADER(mysql/mysql.h,
        [
          database=mysql
          ggzd_LIBS="$ggzd_LIBS -lmysqlclient_r"
        ],
        [
          if test "$database" = mysql; then
            AC_MSG_ERROR([cannot configure mysql (mysql-dev library needed)])
          fi
        ])
    ],
    [
      if test "$database" = mysql; then
        AC_MSG_ERROR([cannot configure mysql (mysql library needed)])
      fi
    ])
fi

# Make sure a database was configured
if test "$database" = yes; then
  AC_MSG_ERROR([no usable database library found.  See above messages for more.])
fi


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
    ggzdstatedir="${ac_default_prefix}/var/ggzd"
  else
    ggzdstatedir="${prefix}/var/ggzd"
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

SUBDIRS="man ggzdmod"

if test "$enable_server" = yes; then
    SUBDIRS="$SUBDIRS ggzd"
fi

if test "$enable_games" = yes; then
    SUBDIRS="$SUBDIRS game_servers"
fi

dnl Compile the test code
SUBDIRS="$SUBDIRS tests"

#LTLIBOBJS=`echo $LIBOBJS | sed 's/\.o/\.lo/g'`

AC_SUBST(LDADD)
#AC_SUBST(LIBOBJS)
#AC_SUBST(LTLIBOBJS)
AC_SUBST(ggzd_LIBS)
AC_SUBST(SUBDIRS)
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
	ggzd/Makefile 
	ggzd/database/Makefile
	ggzd/ggzd.conf
	ggzd/ggzd.test
	game_servers/Makefile
	game_servers/spades/Makefile
	game_servers/tictactoe/Makefile
	game_servers/dots/Makefile
	game_servers/lapocha/Makefile
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
	game_servers/muehle/ggz/Makefile
	game_servers/muehle/qtserv/Makefile
	game_servers/muehle/variants/Makefile
	game_servers/escape/Makefile
	game_servers/keepalive/Makefile
	game_servers/keepalive/src/Makefile
	game_servers/geekgame/Makefile
	game_servers/connectx/Makefile
	game_servers/tuxman/Makefile
	game_servers/tuxman/lib/Makefile
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
