EXTRA_DIST = default/*.xpm templates/*.xpm templates/README Dragon/*.xpm Electric/*.xpm

SUBDIRS = .

data = default templates Dragon Electric

install-data-local:
	for p in $(data); do \
		$(mkinstalldirs) $(DESTDIR)$(ggzexecmoddir)/ccheckers/pixmaps/$$p ;\
		cp -r $$p/*.xpm $(DESTDIR)$(ggzexecmoddir)/ccheckers/pixmaps/$$p ;\
	done
	cp templates/README $(DESTDIR)$(ggzexecmoddir)/ccheckers/pixmaps/templates

uninstall-local:
	-rm -rv $(ggzexecmoddir)/ccheckers
