## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 5682 2003-11-23 11:14:56Z dr_maux $

SUBDIRS = krosswater KReversi kdots ktictactux koenig muehle keepalive fyrdman kcc man po
DIST_SUBDIRS = $(SUBDIRS) lib

EXTRA_DIST = README.GGZ QuickStart.GGZ \
		m4/ggz.m4 m4/kde.m4

dist-hook:
	-perl am_edit --foreign-libtool

