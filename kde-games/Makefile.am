## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6135 2004-07-17 03:59:30Z josef $

SUBDIRS = krosswater KReversi kdots ktictactux koenig muehle keepalive fyrdman kcc man po
DIST_SUBDIRS = $(SUBDIRS) lib

EXTRA_DIST = README.GGZ QuickStart.GGZ \
		m4/ggz.m4 m4/kde.m4

dist-hook:
	-perl am_edit --foreign-libtool

