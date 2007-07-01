dnl =================================
dnl GGZ Gaming Zone - Zeroconf Macros
dnl =================================
dnl
dnl Looks for zeroconf library implementations and
dnl sets up the corresponding variables, that is,
dnl $zeroconf for display purposes and LIB_AVAHI/
dnl AVAHI_INCLUDES.
dnl
dnl ------------------------------------------------------------------------
dnl Content of this file:
dnl ------------------------------------------------------------------------
dnl AC_GGZ_ZEROCONF - find a suitable zeroconf implementation
dnl
dnl ------------------------------------------------------------------------
dnl Internal functions:
dnl ------------------------------------------------------------------------
dnl AC_GGZ_ZEROCONF_HOWL - Howl library
dnl AC_GGZ_ZEROCONF_AVAHI - Avahi library
dnl

dnl Zeroconf checks for Howl and Avahi
dnl ==================================
AC_DEFUN([AC_GGZ_ZEROCONF],
[
	if test "$zeroconf" = yes || test "$zeroconf" = avahi; then
		AC_GGZ_ZEROCONF_AVAHI
	fi

	if test "$zeroconf" = "avahi"; then
		AC_DEFINE(WITH_AVAHI, 1, [Use avahi for zeroconf mechanism])
		AVAHI_INCLUDES="-I/usr/include"
		LIB_AVAHI="-lavahi-client"
		AC_SUBST(AVAHI_INCLUDES)
		AC_SUBST(LIB_AVAHI)
	else
		AC_MSG_WARN([Zeroconf disabled, avahi not found.])
		zeroconf="(none)"
	fi
])

dnl Check for zeroconf implementation Avahi
dnl =======================================
AC_DEFUN([AC_GGZ_ZEROCONF_AVAHI],
[
	AC_CHECK_LIB(avahi-client, avahi_entry_group_add_service,
	[
		AC_CHECK_HEADERS([avahi-client/publish.h],
		[
			zeroconf=avahi
		],
		[
			if test "$zeroconf" = "avahi"; then
				AC_MSG_ERROR([cannot configure avahi (headers needed)])
			fi
		])
	],
	[
		if test "$zeroconf" = "avahi"; then
			AC_MSG_ERROR([cannot configure avahi (library needed)])
		fi
	],
	[-lpthread])
])

