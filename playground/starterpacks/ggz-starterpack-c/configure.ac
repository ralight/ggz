dnl Configuration for the GGZ Starterpack for C
dnl ===========================================

AC_INIT([GGZ Starterpack for C], [1.0], [ggz-dev@mail.ggzgamingzone.org], [ggz-starterpack-c])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

dnl Check for standard build environment
dnl ====================================

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
#AC_PROG_RANLIB
AM_PROG_CC_C_O

dnl Check for GGZ Requirements
dnl ==========================
AC_GGZ_INIT(defaults, export)
AC_GGZ_LIBGGZ
AC_GGZ_VERSION(0, 99, 4)
AC_GGZ_GGZMOD
AC_GGZ_GGZDMOD
AC_GGZ_GGZCORE
AC_GGZ_GTK
AC_GGZ_CONFIG("true", ignore, "ggz-starterpack-c")
AC_GGZ_INTL

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
AC_TYPE_SIGNAL

dnl Check for libraries
dnl ===========================

AM_PATH_GTK_2_0([2.0.0], [],
		AC_MSG_ERROR([*** Can't build GTK+-2.0 frontend: GTK+-2.0 not found]))

dnl Server stuff
dnl ============

ggzdconfdir="$sysconfdir/ggzd"
ggzdexecmoddir="$libdir/ggzd"
ggzdroomdir="$ggzdconfdir/rooms"
ggzdgamedir="$ggzdconfdir/games"

AC_SUBST(ggzdconfdir)
AC_SUBST(ggzdexecmoddir)
AC_SUBST(ggzdroomdir)
AC_SUBST(ggzdgamedir)

# Library settings
# ================
AC_GGZ_VISIBILITY

AC_CONFIG_FILES([Makefile
	ggz-sample-server/Makefile
	ggz-sample-client/Makefile
])
AC_OUTPUT

dnl Status Output
dnl =============
echo ""
echo "At the prompt type \"make\" to compile the GGZ starterpack."
echo ""
echo "When complete, type \"make install\" to install the software."
echo "You may need root access (su/sudo) for this step."
echo ""
if test "x$GGZ_CONFIG" = "xtrue"; then
	echo "Warning: game information files will not be installed."
	echo "This should only happen during binary package creation."
	echo "Packages must install the .dsc files upon installation."
	echo ""
fi

