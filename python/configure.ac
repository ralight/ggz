dnl Process this file with autoconf to produce a configure script.

AC_INIT([GGZ python package], [0.0.8pre], [ggz-dev@mail.ggzgamingzone.org], [ggz-python])
AC_CONFIG_SRCDIR([configure.ac])
#AC_CONFIG_HEADERS([config.h])
#AM_INIT_AUTOMAKE
#AM_MAINTAINER_MODE

dnl Compiling Options
dnl =====================
#AC_ARG_ENABLE(perl,  [  --disable-perl        Exclude perl modules],, enable_perl=yes)
#AC_ARG_ENABLE(ruby,  [  --disable-ruby        Exclude ruby modules],, enable_ruby=yes)
#AC_ARG_ENABLE(python,[  --disable-python      Exclude python modules],, enable_python=yes)

dnl Check for standard build environment
dnl ====================================
AC_PROG_CC
#AC_ISC_POSIX
#AC_PROG_INSTALL
#AM_PROG_CC_STDC
AC_PROG_MAKE_SET
#AM_PROG_LIBTOOL

dnl GGZ checks
dnl ==========
AC_GGZ_INIT(defaults)
#AC_GGZ_INTL
AC_GGZ_LIBGGZ
#AC_GGZ_GGZCORE
AC_GGZ_GGZMOD("true", ignore)
AC_GGZ_CONFIG

dnl Create files
dnl ============

if test "x$prefix" = "xNONE"; then
	prefix="/usr/local"
fi

AC_CONFIG_FILES([escape-sdl/module.dsc])
AC_OUTPUT

dnl Status output
dnl =============

echo ""
echo "Configured python. Type 'make' to compile it, followed by a 'make install'."
echo ""

