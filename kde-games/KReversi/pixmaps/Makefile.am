EXTRA_DIST = default/*.png blend/*.png

SUBDIRS = .

data = default blend

install-data-local:
	for p in $(data); do \
		$(mkinstalldirs) $(DESTDIR)$(ggzdatadir)/kreversi/pixmaps/$$p ;\
		cp -r $$p/*.png $(DESTDIR)$(ggzdatadir)/kreversi/pixmaps/$$p ;\
	done

uninstall-local:
	-rm -rv $(ggzdatadir)/kreversi
