## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 5593 2003-05-17 10:30:22Z dr_maux $

SUBDIRS = @SUBDIRS@
EXTRA_DIST = package README.GGZ QuickStart.GGZ \
		m4/ggz.m4	\
		m4/gtk-2.0.m4	\
		m4/gtk.m4

install:
	mkdir -p --verbose $(DESTDIR)/`$(GGZ_CONFIG) -c`
	touch $(DESTDIR)/`$(GGZ_CONFIG) -c`/ggz.modules
	make install-recursive # FIXME - kludge

uninstall-local:
	rm -f $(DESTDIR)/`$(GGZ_CONFIG) -c`/ggz.modules

