# Configuration for the GGZ client libraries
# ==========================================

AC_INIT([GGZ client libraries], [0.0.13], [ggz-dev@mail.ggzgamingzone.org], [ggz-client-libs])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE


# Check for standard build environment
# ====================================
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_CPP

AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL
AC_PROG_MAKE_SET
AC_PROG_RANLIB

# Special case for compiling on windows
case $host_os in
  *mingw32* ) MINGW32=yes;;
          * ) MINGW32=no;;
esac

# Check for GGZ Requirements
# ==========================
AC_GGZ_INIT(defaults, export)
AC_GGZ_LIBGGZ
AC_GGZ_INTL


# Check for header files
# ======================
AC_CHECK_HEADERS([fcntl.h limits.h sys/time.h time.h \
                  unistd.h], [],
                 [AC_MSG_ERROR([cannot find required header file])])
AC_CHECK_HEADERS([netdb.h sys/socket.h winsock2.h])
AC_HEADER_SYS_WAIT
AC_HEADER_TIME
# We used to check for the presence of WAIT_ANY, and define it as
# (pid_t)(-1) if not present.  Now we just leave it up to AC_HEADER_SYS_WAIT.


# Check for typedefs, structures, and compiler characteristics
# ============================================================
AC_C_CONST
AC_TYPE_SIZE_T
AC_TYPE_SIGNAL


# Check for PF_LOCAL/PF_UNIX
# ==========================
# There should be an autoconf macro to check this???
#
# Not having this is not necessarily a fatal error.  It should mean that
# the WinAPI interface is used instead.
AC_MSG_CHECKING([for PF_LOCAL])
AC_EGREP_CPP(PF_LOCAL,
             [#include <sys/socket.h>
              PF_LOCAL ],
             [AC_MSG_RESULT(no)
              AC_MSG_CHECKING([for PF_UNIX])
              AC_EGREP_CPP(PF_UNIX, [ #include <sys/socket.h>
				      PF_UNIX ],
                           [AC_MSG_RESULT(no)],
                           [AC_MSG_RESULT(yes)
                            AC_DEFINE([PF_LOCAL], PF_UNIX,
				      [PF_UNIX is available])
			    AC_DEFINE([AF_LOCAL], AF_UNIX,
				      [AF_UNIX is available])])], 
             AC_MSG_RESULT(yes))

# Add DLL build support for WIN32
# ===============================

if test "$MINGW32" = "yes"; then
	LDFLAGS="$LDFLAGS -no-undefined -lws2_32"
fi

# Check for library functions
# ===========================
AC_CHECK_FUNCS([dup2 strcasecmp strchr strerror \
                malloc vprintf], [],
               [AC_MSG_ERROR([cannot find required function])])
AC_CHECK_FUNCS([fork select]) # These fail; dunno why
AC_CHECK_FUNCS([close hstrerror kill socketpair alarm getenv setenv])
AC_FUNC_FORK
#AC_FUNC_MALLOC <- this doesn't do what you think it does
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_VPRINTF
FUNC_MKDIR_TAKES_ONE_ARG
if test "$MINGW32" = "yes"; then
	LIBS="$LIBS -lws2_32"
fi


# Check for expat
# ===============
AC_CHECK_LIB(expat, XML_ParserCreate, [LIBEXPAT="-lexpat"],
             [AC_MSG_ERROR(cannot find expat library)])
AC_CHECK_HEADER(expat.h, [],
                [AC_MSG_ERROR(cannot find expat.h header)])
AC_SUBST(LIBEXPAT)


# Debug modes
# ===========
AC_GGZ_DEBUG


# Various dirs
# ============
if test "x${sysconfdir}" = 'x${prefix}/etc'; then
  ggzconfdir="${prefix}/etc"
else
  ggzconfdir="${sysconfdir}"
fi
AC_DEFINE_UNQUOTED([GGZCONFDIR], "${ggzconfdir}",
		   [Directory containing GGZ configuration files])


if test "x${libdir}" = 'x${exec_prefix}/lib'; then
  if test "x${exec_prefix}" = "x\${prefix}"; then
    ggzexecmoddir="${prefix}/lib/ggz"
  else
    ggzexecmoddir="${exec_prefix}/lib/ggz"
  fi
else
  ggzexecmoddir="${libdir}/ggz"
fi
AC_DEFINE_UNQUOTED([GAMEDIR], "${ggzexecmoddir}",
		   [Directory containing GGZ client module executables])


if test "x${datadir}" = 'x${prefix}/share'; then
  ggzdatadir="${prefix}/share/ggz"
else
  ggzdatadir="${datadir}/ggz"
fi
AC_DEFINE_UNQUOTED([GGZDATADIR], "${ggzdatadir}",
		   [Directory containing GGZ shared data])

AC_SUBST(ggzconfdir)
AC_SUBST(ggzexecmoddir)
AC_SUBST(ggzdatadir)
AC_SUBST(packagesrcdir)

AC_CONFIG_FILES([Makefile 
	   ggzcore/Makefile
	   ggzmod/Makefile
	   ggzmod-ggz/Makefile
	   ggz-config/Makefile
	   ggz-wrapper/Makefile
	   ggzwrap/Makefile
	   man/Makefile
	   man/ggz-config.6
	   man/ggz.modules.5
	   man/ggzmod_h.3
	   man/ggzcore_h.3
	   tests/Makefile
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
echo "    Dmalloc Debugging..$enable_debug_mem"
echo ""
echo "-------------------------"
echo "At the prompt type \"make\" to"
echo "compile ggzcore and ggzmod."
echo ""
echo "When complete, su to root"
echo "and type \"make install\" to"
echo "install the client software."
