dnl ========================================
dnl GGZ Gaming Zone - Reconfiguration Macros
dnl ========================================
dnl
dnl Reconfiguration support in ggzd comes through file/dir monitoring
dnl (watchers), which either are provided by the kernel or a daemon.
dnl The check looks for a suitable implementation
dnl and sets $reconfiguration as well as LIB_FAM, if appropriate.
dnl
dnl ------------------------------------------------------------------------
dnl Content of this file:
dnl ------------------------------------------------------------------------
dnl AC_GGZ_RECONFIGURATION - find a suitable file change sensor library
dnl
dnl ------------------------------------------------------------------------
dnl Internal functions:
dnl ------------------------------------------------------------------------
dnl AC_GGZ_RECONFIGURATION_FAM - File Alteration Monitor (FAM) daemon
dnl AC_GGZ_RECONFIGURATION_INOTIFY - Linux kernel-based notification
dnl

dnl Reconfiguration checks for FAM and inotify
dnl ==========================================
AC_DEFUN([AC_GGZ_RECONFIGURATION],
[
	if test "$reconfiguration" = yes || test "$reconfiguration" = inotify; then
		AC_GGZ_RECONFIGURATION_INOTIFY
	fi
	if test "$reconfiguration" = yes || test "$reconfiguration" = fam; then
		AC_GGZ_RECONFIGURATION_FAM
	fi

	if test "$reconfiguration" = "inotify"; then
		AC_DEFINE_UNQUOTED(HAVE_INOTIFY, 1,
			[Use inotify to monitor configuration changes])
		reconfiguration="inotify"
	elif test "$reconfiguration" = "fam"; then
		AC_DEFINE_UNQUOTED(WITH_FAM, 1,
			[Use fam to monitor configuration changes])
		LIB_FAM="-lfam"
		AC_SUBST(LIB_FAM)
		reconfiguration="fam"
	else
		AC_MSG_WARN([Reconfiguration disabled, neither inotify nor fam found.])
		reconfiguration="(none)"
	fi
])

dnl Reconfiguration support via FAM (x-platform, but slower)
dnl ========================================================
AC_DEFUN([AC_GGZ_RECONFIGURATION_FAM],
[
	AC_CHECK_LIB(fam, FAMMonitorDirectory,
	[
		AC_CHECK_HEADERS([fam.h],
		[
			reconfiguration=fam
		],
		[
			if test "$reconfiguration" = "fam"; then
				AC_MSG_ERROR([cannot configure fam (headers needed)])
			fi
		])
	],
	[
		if test "$reconfiguration" = "fam"; then
			AC_MSG_ERROR([cannot configure fam (library needed)])
		fi
	],
	[-lfam])
])

dnl Reconfiguration support via inotify (Linux only)
dnl ================================================
AC_DEFUN([AC_GGZ_RECONFIGURATION_INOTIFY],
[
	AC_MSG_CHECKING([for inotify])
	#AC_EGREP_HEADER([__NR_inotify_init], [/usr/include/bits/syscall.h],
	AC_EGREP_HEADER([inotify_event], [/usr/include/linux/inotify.h],
		[
			AC_MSG_RESULT(yes)
			reconfiguration=inotify
		],
		[
			AC_MSG_RESULT(no)
			if test "$reconfiguration" = "inotify"; then
				AC_MSG_ERROR([cannot configure inotify (headers needed)])
			fi
		])
])

