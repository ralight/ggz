EXTRA_DIST = blend/*.png blend/themerc default/*.png default/kreversi.blend gtk-client/*.png gtk-client/themerc

SUBDIRS = .

data = default blend gtk-client

install-data-local:
	for p in $(data); do \
		$(mkinstalldirs) $(DESTDIR)$(ggzdatadir)/kreversi/pixmaps/$$p ;\
		$(INSTALL) -m 644 $$p/*.png $(DESTDIR)$(ggzdatadir)/kreversi/pixmaps/$$p ;\
		if [ -f $$p/themerc ];  \
			then \
			$(INSTALL) -m 644 $$p/themerc $(DESTDIR)$(ggzdatadir)/kreversi/pixmaps/$$p ; \
		fi \
	done

uninstall-local:
	-rm -rv $(ggzdatadir)/kreversi
