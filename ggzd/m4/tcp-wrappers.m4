dnl =====================================
dnl GGZ Gaming Zone - tcp-wrappers Macros
dnl =====================================
dnl
dnl This is taken directly from zeroconf.m4
dnl in GGZ and modified for tcp-wrappers.
dnl
dnl Looks for tcp-wrappers library (libwrap) 
dnl implementations and sets up the 
dnl corresponding variables, that is, $libwrap
dnl for display purposes and LIB_LIBWRAP/
dnl LIBWRAP_INCLUDES.
dnl
dnl ------------------------------------------------------------------------
dnl Content of this file:
dnl ------------------------------------------------------------------------
dnl AC_GGZ_LIBWRAP

AC_DEFUN([AC_GGZ_LIBWRAP],
[
	if test "$libwrap" = "yes"; then
		AC_CHECK_HEADERS([tcpd.h],
		[
			AC_TRY_COMPILE([
#include <syslog.h>
#include <tcpd.h>
int allow_severity = LOG_INFO;
int deny_severity = LOG_WARNING;
				], [
struct request_info wrap_request;
request_init(&wrap_request, RQ_CLIENT_ADDR, "127.0.0.1", RQ_DAEMON, "ggzd", 0);
				], libwrap="yes", libwrap="no")

			if test "$libwrap" = "yes"; then
				AC_DEFINE(WITH_LIBWRAP, 1, [Use tcp_wrappers])
				LIBWRAP_INCLUDES="-I/usr/include"
				LIB_WRAP="-lwrap"
				AC_SUBST(LIBWRAP_INCLUDES)
				AC_SUBST(LIB_WRAP)
				libwrap="yes"
			else
				AC_MSG_ERROR([cannot configure tcp_wrappers (library needed)])
				libwrap="no"
			fi
		],
		[
			AC_MSG_ERROR([cannot configure tcp_wrappers (headers needed)])
		])
	fi
])

