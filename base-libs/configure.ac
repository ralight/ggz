# Configuration for GGZ Base Libraries
# ====================================
AC_INIT([GGZ base libraries], [snapshot-0.99.4], [ggz-dev@mail.ggzgamingzone.org], [ggz-base-libs])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

# Checks for build environment
# ============================
AC_GGZ_PLATFORM_BASE
AC_PROG_LIBTOOL
AC_GGZ_PLATFORM
AC_GGZ_INTL

# Check for header files
# ======================
AC_CHECK_HEADERS([fcntl.h limits.h sys/time.h time.h \
                  unistd.h], [],
                 [AC_MSG_ERROR([cannot find required header file])])
AC_CHECK_HEADERS([netdb.h sys/socket.h winsock2.h])
AC_HEADER_SYS_WAIT
AC_HEADER_TIME


# Check for typedefs, structures, and compiler characteristics
# ============================================================
AC_C_CONST
AC_TYPE_SIZE_T
AC_TYPE_SIGNAL

AC_GGZ_PLATFORM_POSIX_NET
AH_BOTTOM(
[/* Controls whether socketpair or local sockets are used for ggzmod connection */
#if defined HAVE_FORK && defined HAVE_SOCKETPAIR
#  define USE_SOCKETPAIR
#endif

/* Controls whether ggzmod-ggz or ggzmod-game makes the channel connection */
#if defined GGZ_HAVE_SENDMSG && defined USE_SOCKETPAIR
#  define USE_SOCKETFD
#endif])

# Check for library functions
# ===========================
AC_CHECK_FUNCS([dup2 strcasecmp strchr strerror \
                malloc vprintf], [],
               [AC_MSG_ERROR([cannot find required function])])
AC_CHECK_FUNCS([fork select]) # These fail; dunno why
AC_CHECK_FUNCS([close hstrerror kill socketpair alarm getenv setenv])
AC_FUNC_FORK
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_VPRINTF
FUNC_MKDIR_TAKES_ONE_ARG

# Find pthread library & libanl
# =============================
AC_GGZ_PTHREADS

# Encryption: TLS library and gcrypt
# ==================================
AC_GGZ_GCRYPT
AC_GGZ_TLS

# Check for expat
# ===============
AC_CHECK_LIB(expat, XML_ParserCreate, [LIB_EXPAT="-lexpat"],
             [AC_MSG_ERROR(cannot find expat library)])
AC_CHECK_HEADER(expat.h, [],
                [AC_MSG_ERROR(cannot find expat.h header)])
AC_SUBST(LIB_EXPAT)


# Debug modes
# ===========
AC_GGZ_DEBUG

# Libggz settings
# ===============
AM_CONDITIONAL(INSTALL_LIBGGZ, true)
AM_CONDITIONAL(INSTALL_GGZMOD, true)
AM_CONDITIONAL(INSTALL_GGZCORE, true)
AC_GGZ_VISIBILITY

# Makefile creation
# =================
AC_CONFIG_FILES([
	   Makefile 
	   libggz.pc
	   ggzmod.pc
	   ggzcore.pc
	   libggz/Makefile
	   libggz/security/Makefile
	   tests/Makefile
	   ggzcore/Makefile
	   ggzmod/Makefile
	   ggzmod-ggz/Makefile
	   ggz-config/Makefile
	   man/Makefile
	   man/ggz-config.6
	   man/ggz.modules.5
	   man/ggzmod_h.3
	   man/ggzcore_h.3
	   desktop/Makefile
	   po/Makefile
	   po/ggzcore/Makefile
	   po/ggz-config/Makefile
])
AC_OUTPUT

# Status Output
# =============
echo ""
echo "Options"
echo "    General Debugging..$enable_debug"
echo "    GDB Debugging......$enable_debug_gdb"
echo "    Memory Debugging...$enable_debug_mem"
echo "    Encryption support.$enable_gcrypt"
if test "$enable_gcrypt" = yes; then
	echo "      TLS library support: $TLS_TYPE"
fi
echo "    Threading support..$enable_threading"
if test "$enable_threading" = yes; then
	echo "      GAI asynchronous resolver: $enable_anl"
fi
echo ""
echo "-------------------------"
echo "At the prompt type \"make\" to compile the GGZ base libraries."
echo ""
echo "When complete, type \"make install\" to install the library.  You "
echo "may need to perform this step as root."

if test "$enable_anl" = yes; then
	echo ""
	echo "*** WARNING ***"
	echo "You have selected GAI support but it only works with"
	echo "GNU libc >= 2.3.7. Make sure that libc is up to date."
fi

if test "$enable_visibility" = yes; then
	echo ""
	echo "*** WARNING ***"
	echo "You have enabled symbol visibility restrictions."
	echo "This feature hasn't been tested much yet."
fi
