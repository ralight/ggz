## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 7440 2005-08-15 20:05:03Z josef $

SUBDIRS = lib krosswater KReversi kdots ktictactux koenig muehle keepalive fyrdman kcc man po

EXTRA_DIST = README.GGZ QuickStart.GGZ \
		m4/ggz.m4 m4/kde.m4 m4/iconv.m4

dist-hook:
	-perl am_edit --foreign-libtool

