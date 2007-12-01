dnl Configuration for the Gtk+ core client
# $Id: configure.ac 9394 2007-12-01 00:30:32Z jdorje $
dnl ======================================

AC_INIT([GGZ Gtk+ client], [0.0.14], [ggz-dev@mail.ggzgamingzone.org], [ggz-gtk-client])
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
AC_PROG_RANLIB


dnl Check for GGZ Requirements
dnl ==========================
AC_GGZ_INIT(defaults, export)
AC_GGZ_LIBGGZ
AC_GGZ_VERSION(0, 0, 14)
AC_GGZ_GGZCORE
AC_GGZ_INTL


dnl Pidgin plugin check
dnl =================
AC_PIDGIN(2, 2, 0)

dnl Temporary: Check for windows resource compiler
#AC_PATH_PROG([WINDRES], [i586-mingw32msvc-windres], [])
#AM_CONDITIONAL(WIN32, test "x$WINDRES" != "x")

dnl Check for header files
dnl ======================
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_HEADER_TIME
AC_CHECK_HEADERS([arpa/inet.h netinet/in.h sys/time.h sys/socket.h winsock.h])
AC_CHECK_HEADERS([sys/stat.h unistd.h assert.h], [],
                 [AC_MSG_ERROR([cannot find required header file])])


# Check for library functions
# ===========================
# We don't use AC_FUNC_MALLOC or AC_FUNC_REALLOC.  See the manual.
AC_CHECK_FUNCS([memset setlocale strcasecmp strchr strncasecmp strrchr \
                malloc realloc stat strdup strstr rand srand], [],
               [AC_MSG_ERROR([cannot find required function])])
AC_CHECK_FUNCS([execvp])
AC_FUNC_FORK
AC_FUNC_STAT


dnl Check for NLS
dnl ============================================================
AM_WITH_NLS
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

AM_PATH_GTK_2_0([2.0.0], [],
		AC_MSG_ERROR([*** Can't build GTK+-2.0 frontend: GTK+-2.0 not found]))


dnl Debug modes
dnl ===========
AC_GGZ_DEBUG
if test "$enable_debug" = yes; then
  AC_DEFINE([G_DISABLE_DEPRECATED], 1, [Disable deprecated glib functions])
  AC_DEFINE([GDK_DISABLE_DEPRACATED], 1, [Disable deprecated GDK functions])
  AC_DEFINE([GTK_DISABLE_DEPRECATED], 1, [Disable deprecated GTK functions])
fi

# Library settings
# ================
AM_CONDITIONAL(INSTALL_GGZ_GTK, true)
AM_CONDITIONAL([COND_PIDGIN], [test "$have_pidgin" = yes])

dnl Various dirs
dnl ============
AC_SUBST(LDADD)

AC_CONFIG_FILES([Makefile 
	   ggz-gtk/Makefile
	   src/Makefile
	   pidgin-plugin/Makefile
	   po/Makefile
	   doc/Makefile
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
echo ""
echo "    Pidgin plugin......$have_pidgin"
echo ""
echo "At the prompt type \"make\" to compile the GGZ gtk client"
echo ""
echo "When complete, su to root and type \"make install\" to"
echo "install the software."
echo ""

