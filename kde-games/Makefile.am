## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 5305 2002-12-20 04:59:26Z jdorje $

SUBDIRS = krosswater KReversi kdots ktictactux koenig muehle keepalive fyrdman man po

EXTRA_DIST = README.GGZ QuickStart.GGZ \
		m4/ggz.m4 m4/kde.m4

dist-hook:
	-perl am_edit --foreign-libtool

install:
	mkdir -pv $(DESTDIR)/$(ggzmoduleconfdir)
	touch $(DESTDIR)/$(ggzmoduleconfdir)/ggz.modules
	make install-recursive # FIXME - kludge
