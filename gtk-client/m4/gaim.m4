dnl ======================================
dnl Gaim Configuration
dnl ======================================
dnl
dnl Call: AC_GAIM(major, minor, micro)
dnl Returns: $have_gaim (= yes/no)
dnl
dnl Copyright (C) 2004 - 2006 Josef Spillner <josef@ggzgamingzone.org>
dnl

AC_DEFUN([AC_GAIM],
[
AC_ARG_ENABLE([gaim_plugin],
  AS_HELP_STRING([--disable-gaim-plugin], [Do not compile gaim plugin]),
  [enable_gaim_plugin=no],
  [enable_gaim_plugin=yes])

if test "$enable_gaim_plugin" = "yes"; then
  AC_GAIM_CHECK([$1], [$2], [$3])
else
  have_gaim=no
fi
])

AC_DEFUN([AC_GAIM_CHECK],
[
AC_MSG_CHECKING([for Gtk+ library: libgaim])

ac_gaim_includes=NO
gaim_includes=""

AC_CACHE_VAL(ac_cv_have_gaim,
[
AC_GGZ_FIND_FILE(gaim/gaim.h, "/usr/include", gaim_incdir)
ac_gaim_includes="$gaim_incdir"

if test "$ac_gaim_includes" = NO; then
  ac_cv_have_gaim="have_gaim=no"
  ac_gaim_notfound=""
else
  have_gaim="yes"
fi
])

eval "$ac_cv_have_gaim"

if test "$have_gaim" != yes; then
  AC_MSG_RESULT([$have_gaim])
else
  AC_MSG_RESULT([$have_gaim (headers $ac_gaim_includes)])

  major=$1
  minor=$2
  micro=$3

  testprologue="#include <gaim/version.h>"
  testbody=""
  testbody="$testbody if(GAIM_MAJOR_VERSION > $major) return 0;"
  testbody="$testbody if(GAIM_MAJOR_VERSION < $major) return -1;"
  testbody="$testbody if(GAIM_MINOR_VERSION > $minor) return 0;"
  testbody="$testbody if(GAIM_MINOR_VERSION < $minor) return -1;"
  testbody="$testbody if(GAIM_MICRO_VERSION > $micro) return 0;"
  testbody="$testbody if(GAIM_MICRO_VERSION < $micro) return -1;"
  testbody="$testbody return 0;"

  AC_RUN_IFELSE(
    [AC_LANG_PROGRAM([[$testprologue]], [[$testbody]])],
    [],
    [
      AC_MSG_WARN([The Gaim version is too old. Version $major.$minor.$micro is required.])
	  have_gaim=no
    ]
  )
fi

])

