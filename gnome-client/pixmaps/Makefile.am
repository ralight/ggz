XTRA_DIST = Default/*.xpm

SUBDIRS = .

data = Default

install-data-local:
	for p in $(data); do \
		$(mkinstalldirs) $(DESTDIR)$(ggzdatadir)/ggz-gnome/$$p ;\
		$(INSTALL) -m 644 $$p/*.xpm $(DESTDIR)$(ggzdatadir)/ggz-gnome/$$p ;\
	done
	
uninstall-local:
	-rm -rv $(ggzdatadir)/ggz-gnome	
