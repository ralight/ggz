SUBDIRS = ttt3d geekgame

EXTRA_DIST = README.GGZ QuickStart.GGZ \
	m4/ggz.m4

# this hack is required so we can call 'DESTDIR=/tmp make distcheck'
install:
	mkdir -pv $(DESTDIR)/$(ggzmoduleconfdir)
	touch $(DESTDIR)/$(ggzmoduleconfdir)/ggz.modules
	make install-recursive # FIXME - kludge

