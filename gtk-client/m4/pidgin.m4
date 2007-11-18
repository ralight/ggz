dnl ======================================
dnl Pidgin Configuration (formerly Gaim)
dnl ======================================
dnl
dnl Call: AC_PIDGIN(major, minor, micro)
dnl Returns: $have_pidgin (= yes/no), $have_purple (= yes/no)
dnl
dnl Copyright (C) 2004 - 2007 Josef Spillner <josef@ggzgamingzone.org>
dnl

AC_DEFUN([AC_PIDGIN],
[
AC_ARG_ENABLE([pidgin_plugin],
  AS_HELP_STRING([--disable-pidgin-plugin], [Do not compile pidgin plugin]),
  [enable_pidgin_plugin=no],
  [enable_pidgin_plugin=yes])

if test "$enable_pidgin_plugin" = "yes"; then
  AC_PURPLE_CHECK([$1], [$2], [$3])
  if test "$have_purple" = yes; then
    AC_PIDGIN_CHECK
  fi
else
  have_pidgin=no
fi
])

AC_DEFUN([AC_PURPLE_CHECK],
[
AC_MSG_CHECKING([for Pidgin library: libpurple])

ac_purple_includes=NO
purple_includes=""

AC_CACHE_VAL(ac_cv_have_purple,
[
AC_GGZ_FIND_FILE(libpurple/plugin.h, "/usr/include", purple_incdir)
ac_purple_includes="$purple_incdir"

if test "$ac_purple_includes" = NO; then
  ac_cv_have_purple="have_purple=no"
else
  have_purple="yes"
fi
])

eval "$ac_cv_have_purple"

if test "$have_purple" != yes; then
  AC_MSG_RESULT([$have_purple])
else
  AC_MSG_RESULT([$have_purple (headers $ac_purple_includes)])

  major=$1
  minor=$2
  micro=$3

  testprologue=""
  testprologue="$testprologue #include <glib/gtypes.h>"
  testprologue="$testprologue #include <libpurple/version.h>"
  testbody=""
  testbody="$testbody if(!purple_version_check($major, $minor, $micro)) return 0;"
  testbody="$testbody else return -1;"

  save_cppflags=$CPPFLAGS
  save_libs=$LIBS
  glibflags=`pkg-config --cflags glib-2.0 2>/dev/null`
  CPPFLAGS="$CPPFLAGS $glibflags"
  LIBS="$LIBS -lpurple"
  AC_RUN_IFELSE(
    [AC_LANG_PROGRAM([[$testprologue]], [[$testbody]])],
    [],
    [
      AC_MSG_WARN([The Pidgin/purple version is too old. Version $major.$minor.$micro is required.])
	  have_purple=no
    ]
  )
  CPPFLAGS=$save_cppflags
  LIBS=$save_LIBS
fi
])

AC_DEFUN([AC_PIDGIN_CHECK],
[
AC_MSG_CHECKING([for Pidgin library: libpidgin])

ac_pidgin_includes=NO
pidgin_includes=""

AC_CACHE_VAL(ac_cv_have_pidgin,
[
AC_GGZ_FIND_FILE(pidgin/gtkplugin.h, "/usr/include", pidgin_incdir)
ac_pidgin_includes="$pidgin_incdir"

if test "$ac_pidgin_includes" = NO; then
  ac_cv_have_pidgin="have_pidgin=no"
else
  have_pidgin="yes"
fi
])

eval "$ac_cv_have_pidgin"

if test "$have_pidgin" != yes; then
  AC_MSG_RESULT([$have_pidgin])
else
  AC_MSG_RESULT([$have_pidgin (headers $ac_pidgin_includes)])
fi
])

