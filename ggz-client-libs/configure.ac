dnl Process this file with autoconf to produce a configure script.
dnl $Id: configure.ac 5358 2003-02-04 01:55:26Z jdorje $

AC_INIT(configure.ac)
AM_INIT_AUTOMAKE(ggz-client-libs, 0.0.7pre)
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

dnl Check for standard build environment
dnl ====================================

AC_PROG_CC
AC_ISC_POSIX
AC_PROG_INSTALL
AC_PROG_CPP
AM_PROG_CC_STDC
AM_PROG_LIBTOOL
AC_PROG_MAKE_SET

dnl Compiling Options
dnl =================
AC_ARG_ENABLE(debug,           [  --enable-debug          Enable debugging ],, enable_debug=no)
AC_ARG_ENABLE(debug-gdb,       [  --enable-debug-gdb      enable GDB debugging ],,enable_debug_gdb=no)

dnl Check for GGZ Requirements
dnl ==========================
AC_GGZ_INIT(defaults, export)
AC_GGZ_LIBGGZ

dnl Check for header files
dnl ======================
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS(sys/time.h unistd.h)

dnl Check for typedefs, structures, and compiler characteristics
dnl ============================================================
AC_C_CONST
AC_TYPE_SIZE_T
AC_TYPE_SIGNAL
AC_HEADER_TIME
AC_EGREP_CPP(WAIT_ANY, [ #include <sys/wait.h>
		 WAIT_ANY ], AC_MSG_WARN(*** Cannot find WAIT_ANY, defining our own )
	         AC_DEFINE(WAIT_ANY, (pid_t)-1))

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


dnl Check for library functions
dnl ===========================
AC_CHECK_FUNC(poll)


dnl Check for popt
dnl ==============
AC_CHECK_LIB(popt, poptGetNextOpt,
    [AC_CHECK_HEADER(popt.h, LDADD="$LDADD -lpopt",
        AC_MSG_ERROR(*** Cannot find popt.h header))],
    AC_MSG_ERROR(*** Cannot find popt library ))


dnl Check for expat
dnl ==============
AC_CHECK_LIB(expat, XML_ParserCreate,
    [AC_CHECK_HEADER(expat.h, LDADD="$LDADD -lexpat",
        AC_MSG_ERROR(*** Cannot find expat.h header); exit)],
    AC_MSG_ERROR(*** Cannot find expat library ); exit)


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


dnl Various dirs
dnl ============
if test "x${sysconfdir}" = 'x${prefix}/etc'; then
  if test "x${prefix}" = "xNONE"; then
    ggzconfdir="${ac_default_prefix}/etc"  
  else
    ggzconfdir="${prefix}/etc"
  fi
else
  ggzconfdir="${sysconfdir}"
fi
AC_DEFINE_UNQUOTED(GGZCONFDIR, "${ggzconfdir}")


if test "x${libdir}" = 'x${exec_prefix}/lib'; then
  if test "x${exec_prefix}" = "xNONE"; then
    if test "x${prefix}" = "xNONE"; then
      ggzexecmoddir="${ac_default_prefix}/lib/ggz"
    else
      ggzexecmoddir="${prefix}/lib/ggz"
    fi
  else
    ggzexecmoddir="${exec_prefix}/lib/ggz"
  fi
else
  ggzexecmoddir="${libdir}/ggz"
fi
AC_DEFINE_UNQUOTED(GAMEDIR, "${ggzexecmoddir}")


if test "x${datadir}" = 'x${prefix}/share'; then
  if test "x${prefix}" = "xNONE"; then
    ggzdatadir="${ac_default_prefix}/share/ggz"
  else
    ggzdatadir="${prefix}/share/ggz"
  fi
else
  ggzdatadir="${datadir}/ggz"
fi
AC_DEFINE_UNQUOTED(GGZDATADIR, "${ggzdatadir}")

packagesrcdir=`cd $srcdir && pwd`
AC_DEFINE_UNQUOTED(PACKAGE_SOURCE_DIR, "${packagesrcdir}")

AC_SUBST(LDADD)
AC_SUBST(ggzconfdir)
AC_SUBST(ggzexecmoddir)
AC_SUBST(ggzdatadir)
AC_SUBST(packagesrcdir)

AC_OUTPUT([Makefile 
	   ggzcore/Makefile
	   ggzmod/Makefile
	   man/Makefile
	   man/ggz-config.6
	   man/ggz.modules.5
	   man/ggzmod_h.3
	   man/ggzcore_h.3
	   ])


dnl Status Output
dnl =============
echo ""
echo "Options"
echo "    General Debugging..$enable_debug"
echo "    GDB Debugging......$enable_debug_gdb"
echo ""
echo "-------------------------"
echo "At the prompt type \"make\" to"
echo "compile ggzcore"
echo ""
echo "When complete, su to root"
echo "and type \"make install\" to"
echo "install the client software."
