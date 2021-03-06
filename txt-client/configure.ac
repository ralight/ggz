dnl Configuration for the text client
dnl =================================

AC_INIT([GGZ text client], [snapshot-0.99.5], [ggz-dev@mail.ggzgamingzone.org], [ggz-txt-client])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

dnl Check for standard build environment
dnl ====================================
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET


dnl Compiling Options
dnl =================
AC_ARG_ENABLE(debug,           [  --disable-debug          Disable debugging ],, enable_debug=yes)
AC_ARG_ENABLE(debug-gdb,       [  --enable-debug-gdb      enable GDB debugging ],,enable_debug_gdb=no)


dnl Check for GGZ stuff
dnl ===================
AC_GGZ_INIT(defaults, export)
AC_GGZ_INTL
AC_GGZ_LIBGGZ
AC_GGZ_GGZMOD
AC_GGZ_GGZCORE
AC_GGZ_CONFIG("true", ignore, "ggz-txt-client")

dnl Check for header files
dnl ======================
AC_HEADER_STDC
AC_HEADER_TIME
AC_CHECK_HEADERS([libintl.h winsock.h sys/ioctl.h])

dnl Check for NLS
dnl ============================================================
dnl FIXME: TODO

dnl Check for typedefs, structures, and compiler characteristics
dnl ============================================================
AC_C_CONST
AC_TYPE_SIZE_T
AC_TYPE_SIGNAL


dnl Check for library functions
dnl ===========================
AC_CHECK_FUNCS([strsep])

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

dnl Readline support
dnl ================

LIB_READLINE=""
AC_CHECK_HEADERS(readline/readline.h, [LIB_READLINE="-lreadline"], [], [])
AC_CHECK_HEADERS(readline/history.h, [], [], [])

AC_SUBST(LIB_READLINE)

if test "x$LIB_READLINE" = "x"; then
	enable_readline="no"
else
	enable_readline="yes"
fi

dnl Various dirs
dnl ============

AC_CONFIG_FILES([Makefile
		ggz-txt/Makefile
		tttxt/Makefile
		tttxt/module.dsc
		man/Makefile
		po/Makefile
	   ])
AC_OUTPUT


dnl Status Output
dnl =============
echo ""
echo "Options"
echo "    General Debugging..$enable_debug"
echo "    GDB Debugging......$enable_debug_gdb"
echo "    Readline support...$enable_readline"
echo ""
echo "At the prompt type \"make\" to compile the GGZ text client"
echo ""
echo "When complete, su to root and type \"make install\" to"
echo "install the software."
echo ""
