dnl Process this file with autoconf to produce a configure script.
dnl $Id: configure.ac 5739 2004-01-17 18:42:36Z dr_maux $

AC_INIT([GGZ Gtk+ client], [0.0.8pre], [ggz-dev@lists.sourceforge.net], [ggz-gtk-client])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE
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
AC_CHECK_HEADERS([arpa/inet.h libintl.h locale.h netinet/in.h sys/socket.h \
                  sys/time.h sys/wait.h], [],
                 [AC_MSG_ERROR([cannot find required header file])])
AC_HEADER_SYS_WAIT
AC_HEADER_TIME


# Check for library functions
# ===========================
AC_CHECK_FUNCS([memset setlocale strcasecmp strchr strncasecmp strrchr \
                fork malloc realloc stat], [],
               [AC_MSG_ERROR([cannot find required function])])
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STAT


dnl Check for NLS
dnl ============================================================
AM_WITH_NLS
if test "${prefix}" = "NONE"; then
	prefix=${ac_default_prefix}
fi
AC_DEFINE_UNQUOTED([PACKAGE_LOCALE_DIR], ["${prefix}/share/locale"],
                   [The location to put message catalogs])

dnl Check for typedefs, structures, and compiler characteristics
dnl ============================================================
AC_C_CONST
AC_C_INLINE
AC_PATH_X
AC_TYPE_SIZE_T


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


dnl Debug modes
dnl ===========
AC_GGZ_DEBUG


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

AC_SUBST(LDADD)
AC_SUBST(ggzdatadir)

AC_CONFIG_FILES([Makefile 
	   ggz-gtk/Makefile
	   po/Makefile
	   man/Makefile
	   ])
AC_OUTPUT


dnl Status Output
dnl =============
echo ""
echo "Options"
echo "    General Debugging..$enable_debug"
echo "    GDB Debugging......$enable_debug_gdb"
echo "    Dmalloc Debugging..$enable_debug_mem"
echo "    GTK library........$enable_gtk"
echo ""
echo "At the prompt type \"make\" to compile the GGZ gtk client"
echo ""
echo "When complete, su to root and type \"make install\" to"
echo "install the software."
echo ""

