## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 5305 2002-12-20 04:59:26Z jdorje $

SUBDIRS = ggz-kde modsniff ggzap shadowbridge kio_ggz kio_ggzmeta kcm_ggz compcal keepalivecontrol po

EXTRA_DIST = README.GGZ QuickStart.GGZ \
		m4/ggz.m4 m4/kde.m4

dist-hook:
	-perl am_edit --foreign-libtool

