dnl ======================================
dnl GGZ Gaming Zone - Configuration Macros
dnl ======================================
dnl
dnl Copyright (C) 2006 Josef Spillner, josef@ggzgamingzone.org
dnl It is published under the conditions of the GNU General Public License.
dnl
dnl ======================================
dnl
dnl This file contains system library dependent macros such as asynchronous
dnl hostname lookups.
dnl As opposed to platform.m4, all optional features go here.
dnl
dnl ======================================
dnl

dnl ------------------------------------------------------------------------
dnl Content of this file:
dnl ------------------------------------------------------------------------
dnl AC_GGZ_ASYNC - find out if libanl is available which makes resolving
dnl                non-blocking
dnl

dnl ------------------------------------------------------------------------
dnl Check if asynchronous hostname lookups are possible
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_GGZ_ASYNC],
[
  AC_ARG_ENABLE([anl],
    AC_HELP_STRING([--enable-anl], [Enable asynchronous hostname lookups]),
    [enable_anl=$enableval],
    [enable_anl=no])
  if test $enable_anl != "no"; then
    AC_CHECK_LIB(anl, getaddrinfo_a,
      [AC_DEFINE([GAI_A], 1, [Support for asynchronous hostname lookups])
       enable_anl=yes
       LIB_ASYNC="-lanl"],
      [enable_anl=no])
  fi

  AC_SUBST(LIB_ASYNC)
])

