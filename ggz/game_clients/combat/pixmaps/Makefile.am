SUBDIRS = .

data = default

install-data-local:
	for p in $(data); do \
		$(mkinstalldirs) $(ggzexecmoddir)/combat/pixmaps/$$p ;\
		cp -r $$p/*.xpm $(ggzexecmoddir)/combat/pixmaps/$$p ;\
	done

uninstall-local:
	rm -rv $(ggzexecmoddir)/combat/pixmaps
