dnl Configuration for the GGZ Documentation
dnl =======================================

AC_INIT(configure.ac)
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE(ggz-docs, 0.0.8pre)
AM_CONFIG_HEADER(config.h)

dnl Options for building GGZ-Docs
dnl =============================

AC_ARG_ENABLE(guides,  [  --enable-guides    build the GGZ guides], enable_guides=yes, enable_guides=no)
AC_ARG_ENABLE(spec,    [  --enable-spec      build the GGZ specs ], enable_spec=yes, enable_spec=no)

dnl Use GGZDOCDIR as the top-level installation directory
dnl =====================================================

GGZDOCDIR="$datadir/doc/ggz-docs"
AC_SUBST(GGZDOCDIR)

dnl Apply options
dnl =============

ggzgamedevelopmentguide="ggz-game-development-guide.txt"
ggzhostingguide="ggz-hosting-guide.txt"

if test "$enable_guides" = yes; then
	ggzgamedevelopmentguide="ggz-game-development-guide.ps ggz-game-development-guide_toc.html"
	ggzhostingguide="ggz-hosting-guide.ps ggz-hosting-guide_toc.html"
fi
AC_SUBST(ggzgamedevelopmentguide)
AC_SUBST(ggzhostingguide)

if test "$enable_spec" = yes; then
  AC_PATH_PROG([SGML2HTML], ["sgml2html tool"], [docbook-2-html])
  AC_SUBST(SGML2HTML)

  AC_PATH_PROG([JADE], ["jade tool"], [jade])
  AC_SUBST(JADE)

  AC_PATH_PROG([LYNX], ["lynx web browser"], [lynx])
  AC_SUBST(LYNX)

  EXTRA_SUBDIRS="$EXTRA_SUBDIRS spec"
fi

AC_SUBST(EXTRA_SUBDIRS)

dnl Create the Makefiles
dnl ====================

AC_OUTPUT(Makefile
	games/Makefile
	games/hastings/Makefile
	games/external-games/Makefile
	games/ggzcards/Makefile
	games/game-development/Makefile
	ggz-project/Makefile
	coreclients/Makefile
	grubby/Makefile
	spec/Makefile
	overview/Makefile
	server/Makefile
	server/hosting/Makefile
	articles/Makefile)

dnl Report success
dnl ==============

echo ""
echo "This package is now configured. Please type 'make' in order to compile."
echo "The following installation layout is used:"
echo ""
echo "Installation prefix: $prefix"
echo "Documentation goes to: $GGZDOCDIR"
echo "Build guides: $enable_guides"
echo "Build specification: $enable_spec"
echo ""

