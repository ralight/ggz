dnl Process this file with autoconf to produce a configure script.
dnl $Id: configure.ac 5850 2004-02-08 21:30:50Z jdorje $

AC_INIT([GGZ common libraries], [0.0.8], [ggz-dev@mail.ggzgamingzone.org], [libggz])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE


# Checks for programs
# ===================
AC_PROG_CC
AC_PROG_CXX
AC_ISC_POSIX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_CPP
AM_PROG_CC_STDC
AM_PROG_LIBTOOL
AC_PROG_RANLIB
AC_PROG_MAKE_SET


# Compiling Options
# =================
AC_ARG_WITH([gcrypt],
	    AC_HELP_STRING([--with-gcrypt], [Add encryption support]),
	    [enable_gcrypt=yes], [enable_gcrypt=no])
AC_ARG_ENABLE([threading],
              AC_HELP_STRING([--disable-threading],
                             [Don't build in threading support]),
              [enable_threading=$enableval], [enable_threading=yes])


# Check for header files
# ======================
AC_CHECK_HEADERS([fcntl.h sys/param.h sys/wait.h], [],
                 [AC_MSG_ERROR([cannot find required header file])])
AC_CHECK_HEADERS([arpa/inet.h netdb.h netinet/in.h sys/socket.h \
                  sys/uio.h sys/un.h sys/wait.h winsock.h])
AC_HEADER_SYS_WAIT


# Check for typedefs, structures, and compiler characteristics
# ============================================================
AC_C_CONST
AC_TYPE_MODE_T
AC_TYPE_PID_T
AC_TYPE_SIGNAL
AC_TYPE_SIZE_T


# Signedness overrides
# ====================
save_cflags=$CFLAGS
CFLAGS="-Wall -Werror"
AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
	[[void signedness(void){char c;if(c==-1)c=0;}]])],
	[],
	[save_cflags="$save_cflags -fsigned-char"])
CFLAGS=$save_cflags


# Check for debug types in syslog.h
# =================================
# If syslog is present, we take the log types from it.
# If not, define our own enumeration.
AC_CHECK_HEADER([syslog.h], [GGZ_HAVE_SYSLOG_H="1"], [GGZ_HAVE_SYSLOG_H="0"])
AC_SUBST(GGZ_HAVE_SYSLOG_H)


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


# Check for SUN_LEN
# =================
AC_MSG_CHECKING([for SUN_LEN])
AC_LINK_IFELSE([
	AC_LANG_PROGRAM([[
		#include<sys/types.h>
		#include<sys/un.h>
		]],
		[[struct sockaddr_un su; int i = SUN_LEN(&su);]])
	],
	[AC_MSG_RESULT(yes)
	 AC_DEFINE([HAVE_SUN_LEN], 1, [Define if the SUN_LEN macro exists])],
	[AC_MSG_RESULT(no)])


# Check for msghdr member msg_controllen
# ======================================
AC_EGREP_HEADER(msg_controllen, sys/socket.h,
		AC_DEFINE([HAVE_MSGHDR_MSG_CONTROL], 1,
			  [Define if msghdr has a msg_controllen member])
			AC_MSG_CHECKING([for CMSG_ALIGN])
			AC_LINK_IFELSE([
				AC_LANG_PROGRAM([[
					#include<sys/types.h>
					#include<sys/socket.h>
					]],
					[[int i = CMSG_ALIGN(0);]])
				],
				[AC_MSG_RESULT(yes)
				 AC_DEFINE([HAVE_CMSG_ALIGN], 1,
					  [Define if CMSG_ALIGN is available])],
				[AC_MSG_RESULT(no)])

			AC_MSG_CHECKING([for CMSG_LEN])
			AC_LINK_IFELSE([
				AC_LANG_PROGRAM([[
					#include<sys/types.h>
					#include<sys/socket.h>
					]],
					[[int i = CMSG_LEN(0);]])
				],
				[AC_MSG_RESULT(yes)
				 AC_DEFINE([HAVE_CMSG_LEN], 1,
					  [Define if CMSG_LEN is available])],
				[AC_MSG_RESULT(no)])

			AC_MSG_CHECKING([for CMSG_SPACE])
			AC_LINK_IFELSE([
				IF_LANG_PROGRAM([[
					#include<sys/types.h>
					#include<sys/socket.h>
					]],
					[[int i = CMSG_SPACE(0);]])
				],
				[AC_MSG_RESULT(yes)
				 AC_DEFINE([HAVE_CMSG_SPACE], 1,
					  [Define if CMSG_SPACE is available])],
				[AC_MSG_RESULT(no)])
)


# Check for library functions
# ===========================
# On some systems additional libraries may be needed.  Simply include them
# if they're there.
AC_CHECK_LIB(socket, socket, [LDADD="$LDADD -lsocket"])
AC_CHECK_LIB(nsl, gethostbyname, [LDADD="$LDADD -lnsl"])

AC_CHECK_FUNCS([gethostbyname getpagesize memmove memset mkdir socket \
                stat strcasecmp strerror strstr], [],
               [AC_MSG_ERROR([cannot find required function])])
AC_CHECK_FUNCS([alarm inet_pton strtok strtok_r])
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_REALLOC
AC_FUNC_STAT

AC_REPLACE_FUNCS([strsep])


# Find pthread library
# ====================
if test $enable_threading != "no"; then
  # Threading support is not available on all platforms.  It's dangerous
  # to disable it so this must be done explicitly.
  AC_CHECK_HEADER(pthread.h, [],
                  AC_MSG_ERROR([*** Cannot find pthread.h header]))
  AC_CHECK_LIB(pthread, pthread_create,
               [LDADD="$LDADD -lpthread"
                CFLAGS="$CFLAGS -D_REENTRANT"],
               [AC_CHECK_LIB(c_r, pthread_create,
                             [LDADD="$LDADD -pthread"
                              CFLAGS="$CFLAGS -D_THREAD_SAFE"],
               AC_MSG_ERROR([*** Cannot find pthread library]))])
else
  AC_DEFINE([NO_THREADING], 1, [Define if threading support is disabled])
fi

# Encryption library: libgcrypt
# =============================
SRCSUBDIRS="."
if test "$enable_gcrypt" = yes; then
  AC_CHECK_LIB(gcrypt, gcry_check_version,
    [
      AC_CHECK_HEADER(gcrypt.h,
        [
          AC_DEFINE_UNQUOTED([USE_GCRYPT], 1,
                             [Define if you have the gcryp lib])
          LDADD="$LDADD -lgcrypt"
	],
	[
          AC_MSG_WARN([*** No gcrypt-dev found - compiling without encryption support])
        ])
    ],
    [
      AC_MSG_WARN([*** No gcrypt found - compiling without encryption support])
    ])
fi

# Encryption: TLS library
# =======================
AC_GGZ_TLS
LDADD="$LDADD $GGZTLS_LDFLAGS $GGZTLS_LIB"
CFLAGS="$CFLAGS $GGZTLS_INCLUDES"


# Debug modes
# ===========
AC_GGZ_DEBUG


AC_SUBST(LDADD)

AC_CONFIG_FILES([Makefile 
	   src/Makefile
	   src/ggz.h
	   src/security/Makefile
	   tests/Makefile
	   man/Makefile])
AC_OUTPUT


# Status Output
# =============
echo ""
echo "Options"
echo "    General Debugging..$enable_debug"
echo "    GDB Debugging......$enable_debug_gdb"
echo "    Memory Debugging...$enable_debug_mem"
echo "    Encryption support.$enable_gcrypt"
echo "    Threading support..$enable_threading"
if test "$enable_gcrypt" = yes; then
	echo "      TLS library support: $TLS_TYPE"
fi
echo ""
echo "-------------------------"
echo "At the prompt type \"make\" to compile libggz."
echo ""
echo "When complete, type \"make install\ to install the library.  You "
echo "may need to perform this step as root."
