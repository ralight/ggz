SUBDIRS = .

data = 8x8.* Bridge.* Checkers.* Corridors.* Default.* In\ The\ Wild.* Island\ I.* Island\ II.* Isles.* Pipes.* Small.* Plenty\ of\ options.* Checkers+Options.*

EXTRA_DIST = $(data)

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(ggzexecmoddir)/combat/maps ;\
	for p in $(data); do \
    $(INSTALL_DATA) "$$p" $(DESTDIR)$(ggzexecmoddir)/combat/maps ;\
	done

uninstall-local:
	rm -rv $(ggzexecmoddir)/combat/maps
