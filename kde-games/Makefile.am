## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 5535 2003-05-10 12:21:27Z dr_maux $

SUBDIRS = krosswater KReversi kdots ktictactux koenig muehle keepalive fyrdman man po
DIST_SUBDIRS = $(SUBDIRS) lib

EXTRA_DIST = README.GGZ QuickStart.GGZ \
		m4/ggz.m4 m4/kde.m4

dist-hook:
	-perl am_edit --foreign-libtool

install:
	mkdir -p --verbose $(DESTDIR)/$(ggzmoduleconfdir)
	touch $(DESTDIR)/$(ggzmoduleconfdir)/ggz.modules
	make install-recursive # FIXME - kludge
