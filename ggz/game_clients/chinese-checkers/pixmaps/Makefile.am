SUBDIRS = .

data = default templates

install-data-local:
	for p in $(data); do \
		$(mkinstalldirs) $(ggzexecmoddir)/ccheckers/pixmaps/$$p ;\
		cp -r $$p/*.xpm $(ggzexecmoddir)/ccheckers/pixmaps/$$p ;\
	done

uninstall-local:
	-rm -rv $(ggzexecmoddir)/ccheckers
