## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 8833 2007-01-05 01:06:13Z jdorje $

SUBDIRS = ggz-kde ggzap shadowbridge kio_ggz kio_ggzmeta kcm_ggz compcal keepalivecontrol kgrubby po man

EXTRA_DIST = README.GGZ QuickStart.GGZ

dist-hook:
	-perl am_edit --foreign-libtool --no-autodist

ACLOCAL_AMFLAGS = -I m4 -I m4/ggz
