dnl Process this file with autoconf to produce a configure script.

AC_INIT([GGZ utilities], [0.0.8pre], [ggz-dev@lists.sourceforge.net], [ggz-utils])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

dnl Compiling Options
dnl =====================
AC_ARG_ENABLE(ggzcomm, [  --disable-ggzcomm       Dont' build GGZComm],, enable_ggzcomm=yes)
AC_ARG_ENABLE(telggz, [  --disable-telggz       Dont' build TelGGZ],, enable_telggz=yes)
AC_ARG_ENABLE(metaserv, [  --disable-metaserv     Dont' build MetaServ],, enable_metaserv=yes)
AC_ARG_ENABLE(cmd_client, [  --disable-cmd-client Don't build command-line client],, enable_cmd_client=yes)
AC_ARG_ENABLE(debug,  [  --disable-debug         Turn off debugging (on by default)],, enable_debug=yes)


dnl Check for standard build environment
dnl ====================================
AC_PROG_CC
AC_ISC_POSIX
AC_PROG_INSTALL
AC_PROG_CPP
AM_PROG_CC_STDC
AC_PROG_MAKE_SET
AM_PROG_LIBTOOL

dnl Check for GGZ libraries
dnl =======================
AC_GGZ_INIT(defaults)
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


dnl Check for library functions
dnl ===========================
AC_CHECK_FUNC(select)
AC_CHECK_FUNC(socket, ,
    [AC_CHECK_LIB(socket, socket,
	[AC_CHECK_HEADER(sys/socket.h, LDADD="-lsocket",
	    AC_MSG_ERROR(*** Cannot find socket headers ); exit )],
	AC_MSG_ERROR(*** Cannot find socket library ); exit )])

AC_CHECK_FUNC(gethostbyname, ,
    [AC_CHECK_LIB(nsl, gethostbyname, LDADD="$LDADD -lnsl",
	AC_MSG_ERROR(*** Cannot find nsl library ); exit )])

AC_CHECK_FUNC(inet_ntoa, ,
    [AC_CHECK_LIB(nsl, inet_ntoa, LDADD="$LDADD -lnsl",
	AC_MSG_ERROR(*** Cannot find nsl library ); exit )])

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

SUBDIRS="man"

if test "$enable_cmd_client" = yes || test "$enable_telggz" = yes; then
	AC_GGZ_GGZCORE
	if test "$enable_cmd_client" = yes; then
		SUBDIRS="$SUBDIRS cmd-client"
	fi
fi

if test "$enable_metaserv" = yes; then
	SUBDIRS="$SUBDIRS metaserv"
	if test "$enable_telggz" = yes; then
		SUBDIRS="$SUBDIRS telggz"
	fi
	if test "$enable_ggzcomm" = yes; then
		SUBDIRS="$SUBDIRS ggzcomm"
	fi
fi

AC_SUBST(LDADD)
AC_SUBST(SUBDIRS)

prefix="${prefix}"
if test "x$prefix" = "xNONE"; then
	prefix="${ac_default_prefix}"
fi
AC_DEFINE_UNQUOTED([PREFIX], "$prefix", [Set to installation prefix])

AC_CONFIG_FILES([Makefile
		cmd-client/Makefile
		telggz/Makefile
		metaserv/Makefile
		metaserv/minidom/Makefile
		metaserv/metaserv/Makefile
		ggzcomm/Makefile
		man/Makefile])
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
