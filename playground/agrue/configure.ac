dnl Configuration for Agrue
dnl =======================

AC_INIT([Agrue], [0.0.14pre], [ggz-dev@mail.ggzgamingzone.org], [agrue])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

dnl Check for standard build environment
dnl ====================================
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

dnl GGZ checks
dnl ==========
AC_GGZ_INIT(defaults, export)
AC_GGZ_INTL
AC_GGZ_LIBGGZ
AC_GGZ_GGZCORE

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

dnl Create files
dnl ============

AC_CONFIG_FILES([
	Makefile
	agrue/Makefile
])
AC_OUTPUT

dnl Status output
dnl =============

echo ""
echo "Configured agrue. Type 'make' to compile it, followed by a 'make install'."
echo ""

