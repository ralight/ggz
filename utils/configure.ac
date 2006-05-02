dnl Configuration for the utilities package
dnl =======================================

AC_INIT([GGZ utilities], [0.0.14pre], [ggz-dev@mail.ggzgamingzone.org], [ggz-utils])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

dnl Compiling Options
dnl =====================
AC_ARG_ENABLE([ggzcomm],
	AC_HELP_STRING([--disable-ggzcomm], [Dont' build GGZComm]),,
	enable_ggzcomm=yes)
AC_ARG_ENABLE([telggz],
	AC_HELP_STRING([--disable-telggz], [Dont' build TelGGZ]),,
	enable_telggz=yes)
AC_ARG_ENABLE([metaserv],
	AC_HELP_STRING([--disable-metaserv], [Dont' build MetaServ]),,
	enable_metaserv=yes)
AC_ARG_ENABLE([cmd_client],
	AC_HELP_STRING([--disable-cmd-client], [Don't build command-line client]),,
	enable_cmd_client=yes)
AC_ARG_ENABLE([debug],
	AC_HELP_STRING([--disable-debug], [Turn off debugging (on by default)]),,
	enable_debug=yes)

dnl Check for standard build environment
dnl ====================================
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_CPP
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
AC_PROG_RANLIB

dnl Check for GGZ libraries
dnl =======================
AC_GGZ_INIT([defaults], [export])
AC_GGZ_INTL
AC_GGZ_LIBGGZ

dnl Check for header files
dnl ======================
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_HEADER_TIME
AC_CHECK_HEADERS([sys/time.h unistd.h socket.h winsock.h arpa/inet.h fcntl.h \
	netdb.h netinet/in.h pthread.h])


dnl Check for typedefs, structures, and compiler characteristics
dnl ============================================================
AC_C_CONST
AC_TYPE_SIZE_T
AC_TYPE_SIGNAL


dnl Check for library functions
dnl ===========================
AC_CHECK_LIB(socket, socket, LDADD="$LDADD -lsocket")
AC_CHECK_LIB(nsl, gethostbyname, LDADD="$LDADD -lnsl")
AC_FUNC_VPRINTF
AC_FUNC_SELECT_ARGTYPES
AC_CHECK_FUNCS([select socket gethostbyname inet_ntoa memmove strcasecmp \
	strchr strdup malloc realloc gethostname])


dnl Use -Wall if we have gcc.
dnl ========================
changequote(,)dnl
if test "x$GCC" = "xyes"; then
  case " $CFLAGS " in
  *[\ \	]-Wall[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wall" ;;
  esac
fi
changequote([,])dnl


dnl Debug modes
dnl ===========
dnl This defines either DEBUG or NDEBUG.  We could provide a third option
dnl where neither are defined.
if test "$enable_debug" = yes; then
    AC_DEFINE([DEBUG], 1, [Define to enable debugging])
    DFLAGS="DEBUG"
else
    AC_DEFINE([NDEBUG], 1, [Define to disable debugging])
    DFLAGS="NDEBUG"
fi

if test "$enable_debug_socket" = yes; then
    AC_DEFINE([DEBUG_SOCKET], 1, [Define to enable socket debugging])
    DFLAGS="$DFLAGS DEBUG_SOCKET"
fi

if test "$enable_debug_mem" = yes; then
    AC_DEFINE([DEBUG_MEM], 1, [Define to enable memory debugging])
    DFLAGS="$DFLAGS DEBUG_MEM"
    LDADD="$LDADD -ldmalloc"
fi

if test "$enable_debug_gdb" = yes; then
    CFLAGS="$CFLAGS -ggdb"
fi


dnl Which progs to compile
dnl ======================

if test "$enable_metaserv" = no; then
	enable_telggz=no
fi

if test "$enable_cmd_client" = yes || test "$enable_telggz" = yes; then
	AC_GGZ_GGZCORE
fi

AM_CONDITIONAL([COND_CMDCLIENT], [test "$enable_cmd_client" = yes])
AM_CONDITIONAL([COND_METASERV], [test "$enable_metaserv" = yes])
AM_CONDITIONAL([COND_GGZCOMM], [test "$enable_ggzcomm" = yes])
AM_CONDITIONAL([COND_TELGGZ], [test "$enable_telggz" = yes])

AC_SUBST(LDADD)

AC_CONFIG_FILES([
	Makefile
	cmd-client/Makefile
	telggz/Makefile
	metaserv/Makefile
	metaserv/minidom/Makefile
	metaserv/libmeta/Makefile
	metaserv/metaserv/Makefile
	ggzcomm/Makefile
	man/Makefile
])
AC_OUTPUT

dnl Output Status
dnl =============

echo ""
echo "Modules to build:"
echo "    TelGGZ......$enable_telggz"
echo "    MetaServ....$enable_metaserv"
echo "    cmd-client..$enable_cmd_client"
echo "    GGZComm.....$enable_ggzcomm"
echo "Build options:"
echo "    Debug.......$enable_debug"
