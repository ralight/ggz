dnl ======================================
dnl RSVG Configuration
dnl ======================================
dnl

AC_DEFUN([AC_RSVG],
[
AC_MSG_CHECKING([for GNOME library: rsvg])

ac_rsvg_includes=NO ac_rsvg_libraries=NO
rsvg_libraries=""
rsvg_includes=""

AC_CACHE_VAL(ac_cv_have_rsvg,
[
ac_rsvg_includes=`pkg-config --cflags librsvg-2.0 2>/dev/null`
ac_rsvg_libraries=`pkg-config --libs librsvg-2.0 2>/dev/null`

if test "$ac_rsvg_includes" = "" || test "$ac_rsvg_libraries" = ""; then
  ac_cv_have_rsvg="have_rsvg=no"
  ac_rsvg_notfound=""
else
  have_rsvg="yes"
fi
])

eval "$ac_cv_have_rsvg"

if test "$have_rsvg" != yes; then
  AC_MSG_RESULT([$have_rsvg])
else
  AC_MSG_RESULT([$have_rsvg])

  AC_DEFINE_UNQUOTED(HAVE_RSVG, 1, [Add RSVG functionality.])

  RSVG_INCLUDES=$ac_rsvg_includes
  RSVG_LDFLAGS=$ac_rsvg_libraries
  AC_SUBST(RSVG_INCLUDES)
  AC_SUBST(RSVG_LDFLAGS)
fi

])

