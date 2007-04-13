dnl =======================================
dnl GGZ Gaming Zone - Password Check Macros
dnl =======================================
dnl
dnl Looks for cracklib library implementations and
dnl sets up the corresponding variables, that is,
dnl $passwordcheck for display purposes and LIB_PASSWORDCHECK/
dnl PASSWORDCHECK_INCLUDES depending on the choice.
dnl
dnl ------------------------------------------------------------------------
dnl Content of this file:
dnl ------------------------------------------------------------------------
dnl AC_GGZ_PASSWORDCHECK - find a suitable password check implementation
dnl
dnl ------------------------------------------------------------------------
dnl Internal functions:
dnl ------------------------------------------------------------------------
dnl AC_GGZ_PASSWORDCHECK_CRACKLIB - cracklib2 library
dnl AC_GGZ_PASSWORDCHECK_OMNICRACKLIB - omnicrack library
dnl

dnl Password check through (omni)cracklib
dnl =====================================
AC_DEFUN([AC_GGZ_PASSWORDCHECK],
[
	if test "$passwordcheck" = yes || test "$passwordcheck" = omnicracklib; then
		AC_GGZ_PASSWORDCHECK_OMNICRACKLIB
	fi
	if test "$passwordcheck" = yes || test "$passwordcheck" = cracklib; then
		AC_GGZ_PASSWORDCHECK_CRACKLIB
	fi

	if test "$passwordcheck" = "cracklib"; then
		AC_DEFINE(WITH_CRACKLIB, 1, [Use cracklib for password checks])
		CRACKLIB_INCLUDES=""
		LIB_CRACKLIB="-lcrack"
		AC_SUBST(CRACKLIB_INCLUDES)
		AC_SUBST(LIB_CRACKLIB)
	elif test "$passwordcheck" = "omnicracklib"; then
		AC_DEFINE(WITH_OMNICRACKLIB, 1, [Use omnicracklib for password checks])
		OMNICRACKLIB_INCLUDES=""
		LIB_OMNICRACKLIB="-lomnicrack"
		AC_SUBST(OMNICRACKLIB_INCLUDES)
		AC_SUBST(LIB_OMNICRACKLIB)
	else
		AC_MSG_WARN([Password checking disabled, (omni)cracklib not found.])
		passwordcheck="(none)"
	fi
])

dnl Check for password checker implementation cracklib
dnl ==================================================
AC_DEFUN([AC_GGZ_PASSWORDCHECK_CRACKLIB],
[
	AC_CHECK_LIB(crack, FascistCheck,
	[
		AC_CHECK_HEADERS([crack.h],
		[
			passwordcheck=cracklib
		],
		[
			if test "$passwordcheck" = "cracklib"; then
				AC_MSG_ERROR([cannot configure cracklib (headers needed)])
			fi
		])
	],
	[
		if test "$passwordcheck" = "cracklib"; then
			AC_MSG_ERROR([cannot configure cracklib (library needed)])
		fi
	],
	[])
])

dnl Check for password checker implementation omnicracklib
dnl ======================================================
AC_DEFUN([AC_GGZ_PASSWORDCHECK_OMNICRACKLIB],
[
	AC_CHECK_LIB(omnicrack, omnicrack_checkstrength,
	[
		AC_CHECK_HEADERS([omnicrack.h],
		[
			passwordcheck=omnicracklib
		],
		[
			if test "$passwordcheck" = "omnicracklib"; then
				AC_MSG_ERROR([cannot configure omnicracklib (headers needed)])
			fi
		])
	],
	[
		if test "$passwordcheck" = "omnicracklib"; then
			AC_MSG_ERROR([cannot configure omnicracklib (library needed)])
		fi
	],
	[])
])

