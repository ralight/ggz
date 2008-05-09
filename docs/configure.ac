dnl Configuration for the GGZ Documentation
dnl =======================================

AC_INIT([GGZ documentation], [snapshot-0.99.3], [ggz-dev@mail.ggzgamingzone.org], [ggz-docs])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

dnl Options for building GGZ-Docs
dnl =============================

AC_ARG_ENABLE(guides,  [  --enable-guides    build the GGZ guides], enable_guides=yes, enable_guides=no)
AC_ARG_ENABLE(spec,    [  --enable-spec      build the GGZ specs ], enable_spec=yes, enable_spec=no)

dnl Use GGZDOCDIR as the top-level installation directory
dnl =====================================================

GGZDOCDIR="$datadir/doc/ggz-docs"
AC_SUBST(GGZDOCDIR)

ggzdocdir=`eval "echo $GGZDOCDIR"`

dnl Apply options
dnl =============

if test "$enable_guides" = yes; then
	AC_PATH_PROG([DVIPS], [dvips])
	AC_PATH_PROG([TEXI2HTML], [texi2html])
	if test "x$DVIPS" = "x" || test "x$TEXI2HTML" = "x"; then
		AC_MSG_ERROR([The tools necessary to build the guides have not been found.])
	fi
fi

if test "$enable_spec" = yes; then
	AC_PATH_PROG([SGML2HTML], [docbook-2-html])
	if test "x$SGML2HTML" = "x"; then
		AC_PATH_PROG([SGML2HTML], [docbook2html])
	fi
	AC_PATH_PROG([SGML2PDF], [docbook-2-pdf])
	if test "x$SGML2PDF" = "x"; then
		AC_PATH_PROG([SGML2PDF], [docbook2pdf])
	fi
	AC_PATH_PROG([JADE], [jade])
	AC_PATH_PROG([LYNX], [lynx])
	AC_PATH_PROG([FIG2DEV], [fig2dev])
	AC_PATH_PROG([GS], [gs])
	if test "x$SGML2HTML" = "x" || test "x$JADE" = "x" || test "x$LYNX" = "x" \
	|| test "x$GS" = "x" || test "x$SGML2PDF" = "x" || test "x$FIG2DEV" = "x"; then
		AC_MSG_ERROR([The tools necessary to build the specification have not been found.])
		AC_MSG_ERROR([The tools necessary to build the specification have not been found.])
	fi

	EXTRA_SUBDIRS="$EXTRA_SUBDIRS spec"
fi

AM_CONDITIONAL([COND_GUIDES], [test "$enable_guides" = yes])
AM_CONDITIONAL([COND_SPEC], [test "$enable_spec" = yes])

dnl Create the Makefiles
dnl ====================

AC_CONFIG_FILES([Makefile
	intro/Makefile
	games/Makefile
	games/external-games/Makefile
	games/ggz-games/Makefile
	games/ggz-games/tictactoe/Makefile
	games/ggz-games/hastings/Makefile
	games/ggz-games/ggzcards/Makefile
	games/game-development/Makefile
	games/modules/Makefile
	ggz-project/Makefile
	coreclients/Makefile
	grubby/Makefile
	spec/Makefile
	spec/design/Makefile
	spec/mainspec/Makefile
	spec/serverspec/Makefile
	spec/clientspec/Makefile
	overview/Makefile
	overview/architecture/Makefile
	server/Makefile
	server/database/Makefile
	server/ggzd/Makefile
	server/hosting/Makefile
	server/metaserver/Makefile
	server/metaserver/pics/Makefile
	articles/Makefile])
AC_OUTPUT

dnl Report success
dnl ==============

echo ""
echo "This package is now configured. Please type 'make' in order to compile."
echo "The following installation layout is used:"
echo ""
echo "Installation prefix: $prefix"
echo "Documentation goes to: $ggzdocdir"
echo "Build guides: $enable_guides"
echo "Build specification: $enable_spec"
echo ""

