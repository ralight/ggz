## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 7624 2005-11-15 16:27:13Z josef $

SUBDIRS = ggz-kde ggzap shadowbridge kio_ggz kio_ggzmeta kcm_ggz compcal keepalivecontrol kgrubby po man

EXTRA_DIST = README.GGZ QuickStart.GGZ \
		m4/ggz.m4 m4/kde.m4 m4/iconv.m4

dist-hook:
	-perl am_edit --foreign-libtool --no-autodist

