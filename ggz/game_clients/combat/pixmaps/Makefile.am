EXTRA_DIST = default/*.xpm

SUBDIRS = .

data = default

install-data-local:
	for p in $(data); do \
		$(mkinstalldirs) $(DESTDIR)$(ggzdatadir)/combat/pixmaps/$$p ;\
		cp -r $$p/*.xpm $(DESTDIR)$(ggzdatadir)/combat/pixmaps/$$p ;\
	done

uninstall-local:
	rm -rv $(ggzdatadir)/combat/pixmaps
