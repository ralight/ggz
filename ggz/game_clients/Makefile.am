## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 4666 2002-09-23 12:33:57Z jdorje $

SUBDIRS = @SUBDIRS@
EXTRA_DIST = package README.GGZ QuickStart.GGZ

install:
	mkdir -pv $(DESTDIR)/`$(GGZ_CONFIG) -c`
	touch $(DESTDIR)/`$(GGZ_CONFIG) -c`/ggz.modules
	make install-recursive # FIXME - kludge
