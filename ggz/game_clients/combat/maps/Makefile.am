SUBDIRS = .

data = 8x8.* Bridge.* Checkers.* Default.* Island\ I.*

install-data-local:
	$(mkinstalldirs) $(ggzexecmoddir)/combat/maps ;\
	for p in $(data); do \
    $(INSTALL_DATA) "$$p" $(ggzexecmoddir)/combat/maps ;\
	done

uninstall-local:
	rm -rv $(ggzexecmoddir)/combat/maps
