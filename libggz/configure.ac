dnl Process this file with autoconf to produce a configure script.

AC_INIT(configure.ac)
AM_INIT_AUTOMAKE(libggz, 0.0.7pre)
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

dnl Checks for programs
dnl ===================
AC_PROG_CC
AC_ISC_POSIX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_CPP
AM_PROG_CC_STDC
AM_PROG_LIBTOOL
AC_PROG_MAKE_SET


dnl Compiling Options
dnl =================
AC_ARG_ENABLE(debug,        [  --enable-debug          Enable debugging ],, enable_debug=no)
AC_ARG_ENABLE(debug-gdb,    [  --enable-debug-gdb      Enable GDB debugging ],,enable_debug_gdb=no)
AC_ARG_WITH(gcrypt,       [  --with-gcrypt           Add encryption support], enable_gcrypt=yes, enable_gcrypt=no)


dnl Check for header files
dnl ======================
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([errno.h fcntl.h stdlib.h string.h unistd.h])

dnl Check for typedefs, structures, and compiler characteristics
dnl ============================================================
AC_C_CONST
AC_TYPE_MODE_T

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


dnl Check for SUN_LEN
dnl =================
AC_MSG_CHECKING([for SUN_LEN])
AC_TRY_LINK([
    #include<sys/types.h>
    #include<sys/un.h>], 
    [struct sockaddr_un su; int i = SUN_LEN(&su);], 
    AC_MSG_RESULT(yes)
    AC_DEFINE(HAVE_SUN_LEN), AC_MSG_RESULT(no))


dnl Check for msghdr member msg_controllen
dnl ======================================
AC_EGREP_HEADER(msg_controllen, sys/socket.h, AC_DEFINE(HAVE_MSGHDR_MSG_CONTROL)
			AC_MSG_CHECKING([for CMSG_ALIGN])
			AC_TRY_LINK([#include<sys/types.h>
						#include<sys/socket.h>],
				[int i = CMSG_ALIGN(0);], 
				AC_MSG_RESULT(yes)
				AC_DEFINE(HAVE_CMSG_ALIGN), AC_MSG_RESULT(no))

			AC_MSG_CHECKING([for CMSG_LEN])
			AC_TRY_LINK([#include<sys/types.h>
						#include<sys/socket.h>],
				[int i = CMSG_LEN(0);], 
				AC_MSG_RESULT(yes)
				AC_DEFINE(HAVE_CMSG_LEN), AC_MSG_RESULT(no))

			AC_MSG_CHECKING([for CMSG_SPACE])
			AC_TRY_LINK([#include<sys/types.h>
						#include<sys/socket.h>],
				[int i = CMSG_SPACE(0);], 
				AC_MSG_RESULT(yes)
				AC_DEFINE(HAVE_CMSG_SPACE), AC_MSG_RESULT(no))
)
				       

dnl Check for library functions
dnl ===========================
dnl AC_FUNC_MALLOC
dnl AC_FUNC_STAT
AC_CHECK_FUNCS([memset mkdir strdup strerror malloc stat poll])

AC_REPLACE_FUNCS([strsep])

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

dnl Find pthread library
dnl ====================
AC_CHECK_HEADER(pthread.h,
    [AC_CHECK_LIB(pthread, pthread_create,
        LDADD="$LDADD -lpthread"
    CFLAGS="$CFLAGS -D_REENTRANT",
    [AC_CHECK_LIB(c_r, pthread_create,
        [LDADD="$LDADD -pthread"
        CFLAGS="$CFLAGS -D_THREAD_SAFE"],
            AC_MSG_ERROR(*** Cannot find pthread library); exit)])],
    AC_MSG_ERROR(*** Cannot find pthread.h header); exit)

dnl Encryption library: libgcrypt
dnl =============================
SRCSUBDIRS="."
if test "$enable_gcrypt" = yes; then
	AC_CHECK_LIB(gcrypt, gcry_check_version,
		[
			AC_CHECK_HEADER(gcrypt.h,
				[
					AC_DEFINE_UNQUOTED(USE_GCRYPT, 1)
					LDADD="$LDADD -lgcrypt"
				],
				[AC_MSG_WARN([*** No gcrypt-dev found - compiling without encryption support])])
		],
		[AC_MSG_WARN([*** No gcrypt found - compiling without encryption support])])
fi

dnl Encryption: TLS library
AC_GGZ_TLS
LDADD="$LDADD $GGZTLS_LDFLAGS $GGZTLS_LIB"
CFLAGS="$CFLAGS $GGZTLS_INCLUDES"

dnl Use -Wall if we have gcc.
dnl =========================
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
    AC_DEFINE(DEBUG)
    DFLAGS="DEBUG"
else
    AC_DEFINE(NDEBUG)
    DFLAGS="NDEBUG"
fi

if test "$enable_debug_gdb" = yes; then
    CFLAGS="$CFLAGS -ggdb"
fi

AC_SUBST(LDADD)

AC_OUTPUT([Makefile 
	   src/Makefile
	   src/security/Makefile
	   tests/Makefile
	   man/Makefile])


dnl Status Output
dnl =============
echo ""
echo "Options"
echo "    General Debugging..$enable_debug"
echo "    GDB Debugging......$enable_debug_gdb"
echo "    Encryption support.$enable_gcrypt"
if test "$enable_gcrypt" = yes; then
	echo "      TLS library support: $TLS_TYPE"
fi
echo ""
echo "-------------------------"
echo "At the prompt type \"make\" to"
echo "compile libggz"
echo ""
echo "When complete, type \"make install\""
echo "to install the library.  You may"
echo "need to perform this step as root."
