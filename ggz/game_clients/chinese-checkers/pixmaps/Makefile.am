EXTRA_DIST = default/*.xpm templates/*.xpm templates/README Dragon/*.xpm Electric/*.xpm

SUBDIRS = .

data = default templates Dragon Electric

install-data-local:
	for p in $(data); do \
		$(mkinstalldirs) $(DESTDIR)$(ggzdatadir)/ccheckers/pixmaps/$$p ;\
		$(INSTALL) -m 644 $$p/*.xpm $(DESTDIR)$(ggzdatadir)/ccheckers/pixmaps/$$p ;\
	done
	cp templates/README $(DESTDIR)$(ggzdatadir)/ccheckers/pixmaps/templates

uninstall-local:
	-rm -rv $(ggzdatadir)/ccheckers
