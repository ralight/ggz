# Configuration for libggz
# ========================
AC_INIT([GGZ common libraries], [snapshot-0.99.2], [ggz-dev@mail.ggzgamingzone.org], [libggz])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

# Checks for build environment
# ============================
AC_GGZ_PLATFORM_BASE
AC_PROG_LIBTOOL
AC_GGZ_PLATFORM

# Find pthread library & libanl
# =============================
AC_GGZ_PTHREADS

# Encryption: TLS library and gcrypt
# ==================================
AC_GGZ_GCRYPT
AC_GGZ_TLS

# Debug modes
# ===========
AC_GGZ_DEBUG

# Libggz settings
# ===============
AM_CONDITIONAL(INSTALL_LIBGGZ, true)

# Makefile creation
# =================
AC_CONFIG_FILES([
  Makefile 
  src/Makefile
  src/security/Makefile
  tests/Makefile
  man/Makefile
  libggz.pc
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
echo "At the prompt type \"make\" to compile libggz."
echo ""
echo "When complete, type \"make install\" to install the library.  You "
echo "may need to perform this step as root."

if test "$enable_anl" = yes; then
	echo ""
	echo "*** WARNING ***"
	echo "You have selected GAI support but it only works with"
	echo "GNU libc >= 2.3.7. Make sure that libc is up to date."
fi

