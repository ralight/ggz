## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6958 2005-02-28 20:24:58Z josef $

SUBDIRS = ggz-kde ggzap shadowbridge kio_ggz kio_ggzmeta kcm_ggz compcal keepalivecontrol kgrubby po

EXTRA_DIST = README.GGZ QuickStart.GGZ \
		m4/ggz.m4 m4/kde.m4 m4/iconv.m4

dist-hook:
	-perl am_edit --foreign-libtool

