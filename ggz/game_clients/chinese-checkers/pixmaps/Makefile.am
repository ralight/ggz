EXTRA_DIST = default/*.xpm templates/*.xpm templates/README

SUBDIRS = .

data = default templates

install-data-local:
	for p in $(data); do \
		$(mkinstalldirs) $(DESTDIR)$(ggzexecmoddir)/ccheckers/pixmaps/$$p ;\
		cp -r $$p/*.xpm $(DESTDIR)$(ggzexecmoddir)/ccheckers/pixmaps/$$p ;\
	done
	cp templates/README $(DESTDIR)$(ggzexecmoddir)/ccheckers/pixmaps/templates

uninstall-local:
	-rm -rv $(ggzexecmoddir)/ccheckers
