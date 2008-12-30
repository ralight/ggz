dnl ======================================
dnl GGZ Gaming Zone - Configuration Macros
dnl ======================================
dnl
dnl Copyright (C) 2008 Josef Spillner <josef@ggzgamingzone.org>
dnl It is published under the conditions of the GNU General Public License.
dnl
dnl ======================================
dnl
dnl This file contains all autoconf macros needed to tweak libggz's
dnl memory handling for garbage collection or leak detection.

dnl ------------------------------------------------------------------------
dnl Content of this file:
dnl ------------------------------------------------------------------------
dnl AC_GGZ_GC - find the Boehm-Demers-Weiser conservative garbage collector
dnl

dnl ------------------------------------------------------------------------
dnl Try to find the Boehm-GC headers and libraries.
dnl Exported are $(GC_INCLUDES) and $(LIB_GC).
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_GGZ_GC],
[
AC_ARG_WITH([gc],
	AS_HELP_STRING([--with-gc@<:@=TYPE@:>@],
	[type of garbage collector: boehm - auto if no TYPE]),
	[enable_gc=$withval], [enable_gc=no])

usegc=0

if test "$enable_gc" = yes || test "$enable_gc" = boehm; then
	AC_CHECK_LIB(gc, GC_malloc,
	[
		AC_CHECK_HEADERS([gc/gc.h],
		[
			gc=boehm
		],
		[
			if test "$enable_gc" = boehm; then
				AC_MSG_ERROR([cannot configure boehm (headers needed)])
			fi
		])
	],
	[
		if test "$enable_gc" = boehm; then
			AC_MSG_ERROR([cannot configure boehm (library needed)])
		fi
	],
	[-lgc])

	if test "$enable_gc" = boehm; then
		AC_DEFINE(WITH_GC, 1, [Use boehm for garbage collection])
		GC_INCLUDES="-I/usr/include/gc"
		LIB_GC="-lgc"
		AC_SUBST(GC_INCLUDES)
		AC_SUBST(LIB_GC)
		usegc=1
	fi
fi

AC_SUBST(usegc)
])

