dnl Process this file with autoconf to produce a configure script.
dnl $Id: configure.ac 5358 2003-02-04 01:55:26Z jdorje $

AC_INIT(configure.ac)
AM_INIT_AUTOMAKE(ggz-server, 0.0.7pre)
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

dnl Check for standard build environment
dnl ====================================
AC_PROG_CC
AC_PROG_CXX
AC_ISC_POSIX
AC_PROG_INSTALL
AC_PROG_CPP
AM_PROG_LIBTOOL
AM_PROG_CC_STDC
AC_PROG_MAKE_SET


dnl Compiling Options
dnl =====================
AC_ARG_ENABLE(server, [  --disable-server        Don't build ggzd server],, enable_server=yes)
AC_ARG_ENABLE(games,  [  --disable-games         Don't build ggzd games],, enable_games=yes)
AC_ARG_ENABLE(debug,  [  --disable-debug         Turn off debugging (on by default)],, enable_debug=yes)
AC_ARG_ENABLE(debug-gdb,    [  --enable-debug-gdb      enable GDB debugging ],,enable_debug_gdb=no)
AC_ARG_ENABLE(debug-mem,    [  --enable-debug-mem      enable mem debugging ],,enable_debug_mem=no)
AC_ARG_WITH(tmpdir,         [  --with-tmpdir=PATH      directory for temporary files [default=/tmp/ggzd]], tmpdir=$withval, tmpdir="/tmp/ggzd")
AC_ARG_WITH(database,       [  --with-database=DB      type of database to use: db2, db3, db4, pgsql, mysql], database=$withval, database=yes)


dnl Check for GGZ Requirements
dnl ==============
AC_GGZ_INIT(defaults, export)
AC_GGZ_LIBGGZ

dnl Check for header files
dnl ======================
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS(sys/time.h unistd.h)


dnl Check for typedefs, structures, and compiler characteristics
dnl ============================================================
AC_C_CONST
AC_TYPE_SIZE_T
AC_TYPE_SIGNAL
AC_HEADER_TIME

dnl Check for PF_LOCAL/PF_UNIX
dnl ==========================
AC_MSG_CHECKING([for PF_LOCAL])
AC_EGREP_CPP(PF_LOCAL, [ #include <sys/socket.h>
                 PF_LOCAL ], AC_MSG_RESULT(no) 
			     AC_MSG_CHECKING([for PF_UNIX])
			     AC_EGREP_CPP(PF_UNIX, [ #include <sys/socket.h>
				          PF_UNIX ], AC_MSG_RESULT(no)
				                     AC_MSG_ERROR(*** Cannot PF_LOCAL or PF_UNIX),
                                          AC_MSG_RESULT(yes)
					  AC_DEFINE(PF_LOCAL, PF_UNIX)
			                  AC_DEFINE(AF_LOCAL, AF_UNIX)), 
             AC_MSG_RESULT(yes))


dnl Check for library functions
dnl ===========================
AC_CHECK_FUNCS([select scandir gethostname strcasecmp])

AC_CHECK_LIB(popt, poptGetNextOpt,
    [AC_CHECK_HEADER(popt.h, ggzd_LIBS="$ggzd_LIBS -lpopt",
        AC_MSG_ERROR(*** Cannot find popt.h header); exit)],
    AC_MSG_ERROR(*** Cannot find popt library ); exit)

AC_REPLACE_FUNCS([strsep])

dnl Find pthread library
dnl ====================
AC_CHECK_HEADER(pthread.h,
    [AC_CHECK_LIB(pthread, pthread_create,
        ggzd_LIBS="$ggzd_LIBS -lpthread"
    CFLAGS="$CFLAGS -D_REENTRANT",
    [AC_CHECK_LIB(c_r, pthread_create,
        [ggzd_LIBS="$ggzd_LIBS -pthread"
        CFLAGS="$CFLAGS -D_THREAD_SAFE"],
            AC_MSG_ERROR(*** Cannot find pthread library); exit)])],
    AC_MSG_ERROR(*** Cannot find pthread.h header); exit)


dnl Check for expat
dnl ==============
AC_CHECK_LIB(expat, XML_ParserCreate,
    [AC_CHECK_HEADER(expat.h, ggzd_LIBS="$ggzd_LIBS -lexpat",
        AC_MSG_ERROR(*** Cannot find expat.h header); exit)],
    AC_MSG_ERROR(*** Cannot find expat library ); exit)

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
      db3_style=0
      AC_CHECK_HEADER(db.h,
        [
          database=db3
          ggzd_LIBS="$ggzd_LIBS -ldb"
        ],
        [
          if test "$database" = db3; then
            AC_MSG_ERROR([*** Cannot configure db3 (db3-dev needed)])
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
              AC_DEFINE(DB4_IN_DIR)
            ],
            [
              if test "$database" = db3; then
                AC_MSG_ERROR([*** Cannot configure db3 (db3-dev needed)])
              fi
            ])
        ],
        [
          if test "$database" = db3; then
            AC_MSG_ERROR([*** Cannot configure db3 (libdb3 needed)])
          fi
        ])
    ])
fi

# Check for db4 database
if test "$database" = db4 || test "$database" = yes; then
  AC_CHECK_LIB(db, db_env_create_4000,
    [
      db4_style=0
      AC_CHECK_HEADER(db.h,
        [
          database=db4
          ggzd_LIBS="$ggzd_LIBS -ldb"
        ],
        [
          if test "$database" = db4; then
            AC_MSG_ERROR([*** Cannot configure db4 (db4-dev needed)])
          fi
        ])
    ],
    [
      AC_CHECK_LIB(db-4.0, db_env_create_4000,
        [
          AC_CHECK_HEADER(db.h,
            [
              database=db4
              ggzd_LIBS="$ggzd_LIBS -ldb-4.0"
              AC_DEFINE(DB4_IN_DIR)
            ],
            [
              if test "$database" = db4; then
                AC_MSG_ERROR([*** Cannot configure db4 (db4-dev needed)])
              fi
            ])
        ],
        [
          if test "$database" = db4; then
            AC_MSG_ERROR([*** Cannot configure db4 (libdb4 needed)])
          fi
        ])
    ])
fi


# Check for db2 database
if test "$database" = db2 || test "$database" = yes; then
  AC_CHECK_LIB(db, db_appinit,
    [
      db2_style=0
      AC_CHECK_HEADER(db.h,
        [
          database=db2
          ggzd_LIBS="$ggzd_LIBS -ldb"
        ],
        [
          if test "$database" = db2; then
            AC_MSG_ERROR([*** Cannot configure db2 (db2-dev needed)])
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
              AC_DEFINE(DB2_IN_DIR)
            ],
            [
              if test "$database" = db2; then
                AC_MSG_ERROR([*** Cannot configure db2 (db2-dev needed)])
              fi
            ])
        ],
        [
          if test "$database" = db2; then
            AC_MSG_ERROR([*** Cannot configure db2 (libdb2 needed)])
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
            AC_MSG_ERROR([*** Cannot configure pgsql (pgsql-dev library needed)])
          fi
        ])
    ],
    [
      if test "$database" = pgsql; then
        AC_MSG_ERROR([*** Cannot configure pgsql (pgsql library needed)])
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
            AC_MSG_ERROR([*** Cannot configure mysql (mysql-dev library needed)])
          fi
        ])
    ],
    [
      if test "$database" = mysql; then
        AC_MSG_ERROR([*** Cannot configure mysql (mysql library needed)])
      fi
    ])
fi

# Make sure a database was configured
if test "$database" = yes; then
  AC_MSG_ERROR([*** No usable database library found.  See above messages for more.])
fi

dnl Use -Wall if we have gcc.
dnl ========================
changequote(,)dnl
if test "x$GCC" = "xyes"; then
  case " $CFLAGS " in
  *[\ \	]-Wall[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wall" ;
     CXXFLAGS="$CXXFLAGS -Wall" ;;
  esac
fi
changequote([,])dnl


dnl Debug modes
dnl ===========
dnl This defines either DEBUG or NDEBUG.  We could provide a third option
dnl where neither are defined.
if test "$enable_debug" = yes; then
    AC_DEFINE(DEBUG)
    DFLAGS="DEBUG"
else
    AC_DEFINE(NDEBUG)
    DFLAGS="NDEBUG"
fi

if test "$enable_debug_mem" = yes; then
    AC_DEFINE(DEBUG_MEM)
    DFLAGS="$DFLAGS DEBUG_MEM"
    LDADD="$LDADD -ldmalloc"
fi

if test "$enable_debug_gdb" = yes; then
    CFLAGS="$CFLAGS -ggdb"
fi


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
AC_DEFINE_UNQUOTED(GGZDCONFDIR, "${ggzdconfdir}")
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
AC_DEFINE_UNQUOTED(GAMEDIR, "${ggzdexecmoddir}")

if test "x${localstatedir}" = 'x${prefix}/var'; then
  if test "x${prefix}" = "xNONE"; then
    ggzdstatedir="${ac_default_prefix}/var/ggzd"
  else
    ggzdstatedir="${prefix}/var/ggzd"
  fi
else
  ggzdstatedir="${localstatedir}/ggzd"
fi
AC_DEFINE_UNQUOTED(DATADIR, "${ggzdstatedir}")

AC_DEFINE_UNQUOTED(TMPDIR, "${tmpdir}")

if test "x${prefix}" = "xNONE"; then
  ggzddatadir="${ac_default_prefix}/share/ggzd"
else
  ggzddatadir="${prefix}/share/ggzd"
fi
AC_DEFINE_UNQUOTED(GGZDDATADIR, "${ggzddatadir}")

packagesrcdir=`cd $srcdir && pwd`
AC_DEFINE_UNQUOTED(PACKAGE_SOURCE_DIR, "${packagesrcdir}")


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
AC_DEFINE(_GNU_SOURCE)

AC_OUTPUT([Makefile
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
	man/Makefile
	tests/Makefile
])


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
