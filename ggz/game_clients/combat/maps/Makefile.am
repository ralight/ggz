SUBDIRS = .

data = 8x8.* Bridge.* Checkers.* Corridors.* Default.* In\ The\ Wild.* Island\ I.* Island\ II.* Isles.* Pipes.* Small.*

install-data-local:
	$(mkinstalldirs) $(ggzexecmoddir)/combat/maps ;\
	for p in $(data); do \
    $(INSTALL_DATA) "$$p" $(ggzexecmoddir)/combat/maps ;\
	done

uninstall-local:
	rm -rv $(ggzexecmoddir)/combat/maps
