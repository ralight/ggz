EXTRA_DIST = default/*.xpm

SUBDIRS = .

data = default

install-data-local:
	for p in $(data); do \
		$(mkinstalldirs) $(DESTDIR)$(ggzexecmoddir)/combat/pixmaps/$$p ;\
		cp -r $$p/*.xpm $(DESTDIR)$(ggzexecmoddir)/combat/pixmaps/$$p ;\
	done

uninstall-local:
	rm -rv $(ggzexecmoddir)/combat/pixmaps
