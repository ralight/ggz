dnl ======================================
dnl KNewStuff Configuration
dnl ======================================
dnl
dnl Copyright (C) 2003 Josef Spillner, dr_maux@users.sourceforge.net
dnl This file is to be used within KDE's build system.
dnl

AC_DEFUN(AC_KNEWSTUFF,
[
AC_MSG_CHECKING([for KDE library: knewstuff])

ac_knewstuff_includes=NO ac_knewstuff_libraries=NO
knewstuff_libraries=""
knewstuff_includes=""

AC_CACHE_VAL(ac_cv_have_knewstuff,
[
AC_FIND_FILE(knewstuff/downloaddialog.h, $kde_incdirs, knewstuff_incdir)
ac_knewstuff_includes="$knewstuff_incdir"

AC_FIND_FILE(libknewstuff.so, $kde_libdirs, knewstuff_libdir)
ac_knewstuff_libraries="$knewstuff_libdir"

if test "$ac_knewstuff_includes" = NO || test "$ac_knewstuff_libraries" = NO; then
  ac_cv_have_knewstuff="have_knewstuff=no"
  ac_knewstuff_notfound=""
else
  have_knewstuff="yes"
fi
])

eval "$ac_cv_have_knewstuff"

if test "$have_knewstuff" != yes; then
  AC_MSG_RESULT([$have_knewstuff])
else
  AC_MSG_RESULT([$have_knewstuff (libraries $ac_knewstuff_libraries, headers $ac_knewstuff_includes)])

  AC_DEFINE_UNQUOTED(HAVE_KNEWSTUFF, 1, [Add KNewStuff functionality.])

  KNEWSTUFF='-lknewstuff'
  AC_SUBST(KNEWSTUFF)
fi

])

