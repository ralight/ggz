SUBDIRS = .

data = default templates

install-data-local:
	for p in $(data); do \
		$(mkinstalldirs) $(DESTDIR)$(ggzexecmoddir)/ccheckers/pixmaps/$$p ;\
		cp -r $$p/*.xpm $(DESTDIR)$(ggzexecmoddir)/ccheckers/pixmaps/$$p ;\
	done

uninstall-local:
	-rm -rv $(ggzexecmoddir)/ccheckers
