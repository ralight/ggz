SUBDIRS = .

data = default blend gtk-client

install-data-local:
	for p in $(data); do \
		$(mkinstalldirs) $(DESTDIR)$(ggzdatadir)/kreversi/pixmaps/$$p ;\
		$(INSTALL) $$p/*.png $(DESTDIR)$(ggzdatadir)/kreversi/pixmaps/$$p ;\
		if [ -f $$p/themerc ];  \
			then \
			$(INSTALL) $$p/themerc $(DESTDIR)$(ggzdatadir)/kreversi/pixmaps/$$p ; \
		fi \
	done

uninstall-local:
	-rm -rv $(ggzdatadir)/kreversi
