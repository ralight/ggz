## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 5663 2003-10-09 08:27:50Z jdorje $

SUBDIRS = @SUBDIRS@
EXTRA_DIST = package README.GGZ QuickStart.GGZ \
		m4/gettext.m4	\
		m4/ggz.m4	\
		m4/glibc21.m4	\
		m4/gtk-2.0.m4	\
		m4/gtk.m4	\
		m4/iconv.m4	\
		m4/lcmessage.m4	\
		m4/locale.m4	\
		m4/progtest.m4

install:
	mkdir -p --verbose $(DESTDIR)/`$(GGZ_CONFIG) -c`
	touch $(DESTDIR)/`$(GGZ_CONFIG) -c`/ggz.modules
	make install-recursive # FIXME - kludge

uninstall-local:
	rm -f $(DESTDIR)/`$(GGZ_CONFIG) -c`/ggz.modules

