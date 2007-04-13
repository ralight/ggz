dnl =================================
dnl GGZ Gaming Zone - Unicode Macros
dnl =================================
dnl
dnl Looks for unicode library implementations and
dnl sets up the corresponding variables, that is,
dnl $unicode for display purposes and LIB_ICU/
dnl ICU_INCLUDES depending on the choice.
dnl
dnl ------------------------------------------------------------------------
dnl Content of this file:
dnl ------------------------------------------------------------------------
dnl AC_GGZ_UNICODE - find a suitable unicode implementation
dnl
dnl ------------------------------------------------------------------------
dnl Internal functions:
dnl ------------------------------------------------------------------------
dnl AC_GGZ_UNICODE_ICU - ICU library
dnl

dnl Unicode checks for ICU
dnl ======================
AC_DEFUN([AC_GGZ_UNICODE],
[
	if test "$unicode" = yes || test "$unicode" = icu; then
		AC_GGZ_UNICODE_ICU
	fi
	if test "$unicode" = "icu"; then
		AC_DEFINE(WITH_ICU, 1, [Use ICU for unicode mechanism])
		ICU_INCLUDES=""
		LIB_ICU="-licui18n"
		AC_SUBST(ICU_INCLUDES)
		AC_SUBST(LIB_ICU)
	else
		AC_MSG_WARN([Unicode disabled, ICU not found.])
		unicode="(none)"
	fi
])

dnl Check for unicode implementation ICU
dnl ====================================
AC_DEFUN([AC_GGZ_UNICODE_ICU],
[
	AC_CHECK_LIB(icui18n, u_strFromUTF8_3_6,
	[
		AC_CHECK_HEADERS([unicode/ustring.h],
		[
			unicode=icu
		],
		[
			if test "$unicode" = "icu"; then
				AC_MSG_ERROR([cannot configure ICU (headers needed)])
			fi
		])
	],
	[
		if test "$unicode" = "icu"; then
			AC_MSG_ERROR([cannot configure ICU (library needed)])
		fi
	],
	[])
])

