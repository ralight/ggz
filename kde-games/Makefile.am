## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 5329 2003-01-15 18:40:18Z dr_maux $

SUBDIRS = krosswater KReversi kdots ktictactux koenig muehle keepalive fyrdman man po

EXTRA_DIST = README.GGZ QuickStart.GGZ \
		m4/ggz.m4 m4/kde.m4

dist-hook:
	-perl am_edit --foreign-libtool

install:
	mkdir -p --verbose $(DESTDIR)/$(ggzmoduleconfdir)
	touch $(DESTDIR)/$(ggzmoduleconfdir)/ggz.modules
	make install-recursive # FIXME - kludge
