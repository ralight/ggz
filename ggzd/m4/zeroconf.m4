dnl =================================
dnl GGZ Gaming Zone - Zeroconf Macros
dnl =================================
dnl
dnl Looks for zeroconf library implementations and
dnl sets up the corresponding variables, that is,
dnl $zeroconf for display purposes and LIB_AVAHI/LIB_HOWL/
dnl AVAHI_INCLUDES/HOWL_INCLUDES depending on the choice.
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
	if test "$zeroconf" = yes || test "$zeroconf" = howl; then
		AC_GGZ_ZEROCONF_HOWL
	fi

	if test "$zeroconf" = "howl"; then
		AC_DEFINE(WITH_HOWL, 1, [Use howl for zeroconf mechanism])
		HOWL_INCLUDES="-I/usr/include/howl"
		LIB_HOWL="-lhowl -lpthread"
		AC_SUBST(HOWL_INCLUDES)
		AC_SUBST(LIB_HOWL)
		AC_C_BIGENDIAN
	elif test "$zeroconf" = "avahi"; then
		AC_DEFINE(WITH_AVAHI, 1, [Use avahi for zeroconf mechanism])
		AVAHI_INCLUDES="-I/usr/include"
		LIB_AVAHI="-lavahi-client"
		AC_SUBST(AVAHI_INCLUDES)
		AC_SUBST(LIB_AVAHI)
	else
		AC_MSG_WARN([Zeroconf disabled, neither howl nor avahi found.])
		zeroconf="(none)"
	fi
])

dnl Check for zeroconf implementation Howl
dnl ======================================
AC_DEFUN([AC_GGZ_ZEROCONF_HOWL],
[
	AC_CHECK_LIB(howl, sw_discovery_init,
	[
		AC_CHECK_HEADERS([howl/howl_config.h],
		[
			zeroconf=howl
		],
		[
			if test "$zeroconf" = "howl"; then
				AC_MSG_ERROR([cannot configure howl (headers needed)])
			fi
		])
	],
	[
		if test "$zeroconf" = "howl"; then
			AC_MSG_ERROR([cannot configure howl (library needed)])
		fi
	],
	[-lpthread])
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

