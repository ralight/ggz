## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 5654 2003-09-27 14:23:57Z dr_maux $

SUBDIRS = krosswater KReversi kdots ktictactux koenig muehle keepalive fyrdman kcc man po
DIST_SUBDIRS = $(SUBDIRS) lib

EXTRA_DIST = README.GGZ QuickStart.GGZ \
		m4/ggz.m4 m4/kde.m4

dist-hook:
	-perl am_edit --foreign-libtool

install:
	mkdir -p --verbose $(DESTDIR)/$(ggzmoduleconfdir)
	touch $(DESTDIR)/$(ggzmoduleconfdir)/ggz.modules
	make install-recursive # FIXME - kludge

uninstall-local:
	rm -f $(DESTDIR)/`$(GGZ_CONFIG) -c`/ggz.modules

