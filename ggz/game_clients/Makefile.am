## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 5282 2002-12-15 08:23:37Z jdorje $

SUBDIRS = @SUBDIRS@
EXTRA_DIST = package README.GGZ QuickStart.GGZ \
		m4/ggz.m4	\
		m4/gtk-2.0.m4	\
		m4/gtk.m4

install:
	mkdir -p --verbose $(DESTDIR)/`$(GGZ_CONFIG) -c`
	touch $(DESTDIR)/`$(GGZ_CONFIG) -c`/ggz.modules
	make install-recursive # FIXME - kludge
