SUBDIRS = ttt3d geekgame

EXTRA_DIST = README.GGZ QuickStart.GGZ

install:
	mkdir -pv $(DESTDIR)/$(ggzmoduleconfdir)
	touch $(DESTDIR)/$(ggzmoduleconfdir)/ggz.modules
	make install-recursive # FIXME - kludge
