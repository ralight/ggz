## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 5279 2002-12-09 13:10:45Z dr_maux $

SUBDIRS = @SUBDIRS@
EXTRA_DIST = package README.GGZ QuickStart.GGZ

install:
	mkdir -p --verbose $(DESTDIR)/`$(GGZ_CONFIG) -c`
	touch $(DESTDIR)/`$(GGZ_CONFIG) -c`/ggz.modules
	make install-recursive # FIXME - kludge
