SUBDIRS = krosswater KReversi kdots ktictactux koenig muehle keepalive fyrdman man po

EXTRA_DIST = README.GGZ QuickStart.GGZ

dist-hook:
	-perl am_edit --foreign-libtool

install:
	mkdir -pv $(DESTDIR)/$(ggzmoduleconfdir)
	touch $(DESTDIR)/$(ggzmoduleconfdir)/ggz.modules
	make install-recursive # FIXME - kludge
