dnl Process this file with autoconf to produce a configure script.
dnl $Id: configure.ac 5368 2003-02-04 08:53:23Z jdorje $

AC_INIT(configure.ac)
AM_INIT_AUTOMAKE(ggz-gtk-client, 0.0.7pre)
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

dnl Check for standard build environment
dnl ====================================

AC_PROG_CC
AC_ISC_POSIX
AC_PROG_INSTALL
AM_PROG_CC_STDC
AC_PROG_MAKE_SET


dnl Compiling Options
dnl =================
AC_ARG_ENABLE(debug,           [  --disable-debug         Disable debugging ],, enable_debug=yes)
AC_ARG_ENABLE(debug-gdb,       [  --enable-debug-gdb      enable GDB debugging ],,enable_debug_gdb=no)
AC_ARG_ENABLE(gtk, AC_HELP_STRING([--enable-gtk=[gtk1.2/gtk2]],
                                  [Specify GTK+-1.2 or GTK+-2.0 linkage]))

dnl Check for GGZ Requirements
dnl ==========================
AC_GGZ_INIT(defaults, export)
AC_GGZ_GGZCORE
AC_GGZ_LIBGGZ
AC_GGZ_INTL(ignore)


dnl Check for header files
dnl ======================
AC_HEADER_STDC

subdirs="ggz-gtk man"


dnl Check for NLS
dnl ============================================================
if test "x$MSGFMT" != "x"; then
	AC_DEFINE([ENABLE_NLS], 1, [Define to use NLS (i18n)])
	allsubdirs="$subdirs po"
	enable_gettext="yes"
	if test "x${prefix}" = "xNONE"; then
		AC_DEFINE_UNQUOTED([PACKAGE_LOCALE_DIR],
                                   "${ac_default_prefix}/share/locale",
                                   [The location to put message catalogs])
	else
		AC_DEFINE_UNQUOTED([PACKAGE_LOCALE_DIR],
                                   "${prefix}/share/locale",
                                   [The location to put message catalogs])
	fi

else
	allsubdirs="$subdirs"
	enable_gettext="no"
fi

dnl Check for typedefs, structures, and compiler characteristics
dnl ============================================================
AC_C_CONST
AC_TYPE_SIZE_T
AC_HEADER_TIME


dnl Check for library functions
dnl ===========================

# Check for GTK linkage

case "$enable_gtk" in
  gtk)    enable_gtk="gtk" ;;
  gtk1)   enable_gtk="gtk" ;;
  gtk1.2) enable_gtk="gtk" ;;
  gtk2)   enable_gtk="gtk2" ;;
  gtk2.0) enable_gtk="gtk2" ;;
  gtk20)  enable_gtk="gtk2" ;;
  *)      enable_gtk="yes" ;;
esac

if test "$enable_gtk" = "gtk2" || test "$enable_gtk" = "yes"; then
  AM_PATH_GTK_2_0([2.0.0], [found_gtk2="yes"], [found_gtk2="no"])
  if test "$found_gtk2" = "yes"; then
    enable_gtk="gtk2"
    AC_DEFINE(GTK2, 1, [Define to link against GTK+-2.0 libraries])
  elif test "$enable_gtk" = "gtk2"; then
    AC_MSG_ERROR([*** Can't build GTK+-2.0 frontend: GTK+-2.0 not found])
  fi
fi

if test "$enable_gtk" = "gtk" || test "$enable_gtk" = "yes"; then
  AM_PATH_GTK([1.2.0], [found_gtk="yes"], [found_gtk="no"])
  if test "$found_gtk" = "yes"; then
    enable_gtk="gtk"
  elif test "$enable_gtk" = "gtk"; then
    AC_MSG_ERROR([*** Can't build GTK+-1.2 frontend: GTK+-1.2 not found])
  fi
fi

if test "$enable_gtk" = "yes"; then
  AC_MSG_ERROR([*** Can't build GTK frontend: GTK not found])
fi


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
    AC_DEFINE([DEBUG], 1, [Define to enable debugging])
    DFLAGS="DEBUG"
else
    AC_DEFINE([NDEBUG], 1, [Define to disable debugging])
    DFLAGS="NDEBUG"
fi

if test "$enable_debug_gdb" = yes; then
    CFLAGS="$CFLAGS -ggdb"
fi

dnl Various dirs
dnl ============

if test "x${datadir}" = 'x${prefix}/share'; then
  if test "x${prefix}" = "xNONE"; then
    ggzdatadir="${ac_default_prefix}/share/pixmaps/ggz"
  else
    ggzdatadir="${prefix}/share/pixmaps/ggz"
  fi
else
  ggzdatadir=${datadir}
fi


AC_DEFINE_UNQUOTED(GGZDATADIR, "${ggzdatadir}", [Location of GGZ module data])

SUBDIRS=$allsubdirs
AC_SUBST(LDADD)
AC_SUBST(SUBDIRS)
AC_SUBST(ggzdatadir)

AC_OUTPUT([Makefile 
	   ggz-gtk/Makefile
	   po/Makefile
	   man/Makefile
	   ])


dnl Status Output
dnl =============
echo ""
echo "Options"
echo "    General Debugging..$enable_debug"
echo "    GDB Debugging......$enable_debug_gdb"
echo "    Gettext support....$enable_gettext"
echo "    GTK library........$enable_gtk"
echo ""
echo "At the prompt type \"make\" to compile the GGZ gtk client"
echo ""
echo "When complete, su to root and type \"make install\" to"
echo "install the software."
echo ""

