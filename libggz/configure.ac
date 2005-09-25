# Configuration for libggz
# ========================

AC_INIT([GGZ common libraries], [0.0.12], [ggz-dev@mail.ggzgamingzone.org], [libggz])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

# Checks for build environment
# ============================

AC_GGZ_PLATFORM_BASE
AC_PROG_LIBTOOL
AC_GGZ_PLATFORM

# Compiling Options
# =================
AC_ARG_WITH([gcrypt],
  AC_HELP_STRING([--with-gcrypt], [Add encryption support]),
  [enable_gcrypt=$enableval],
  [enable_gcrypt=no])
AC_ARG_ENABLE([threading],
  AC_HELP_STRING([--disable-threading], [Don't build in threading support]),
  [enable_threading=$enableval],
  [enable_threading=yes])

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

# Debug modes
# ===========
AC_GGZ_DEBUG

# Makefile creation
# =================
AC_SUBST(LDADD)

AC_CONFIG_FILES([Makefile 
  src/Makefile
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
if test "$enable_gcrypt" = yes; then
	echo "      TLS library support: $TLS_TYPE"
fi
echo "    Threading support..$enable_threading"
echo ""
echo "-------------------------"
echo "At the prompt type \"make\" to compile libggz."
echo ""
echo "When complete, type \"make install\" to install the library.  You "
echo "may need to perform this step as root."
