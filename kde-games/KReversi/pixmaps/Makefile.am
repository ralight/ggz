SUBDIRS = .

data = default blend gtk-client

install-data-local:
	for p in $(data); do \
		$(mkinstalldirs) $(DESTDIR)$(ggzdatadir)/kreversi/pixmaps/$$p ;\
		cp -r $$p/*.png $(DESTDIR)$(ggzdatadir)/kreversi/pixmaps/$$p ;\
		if [ -f $$p/themerc ];  \
			then \
			cp -r $$p/themerc $(DESTDIR)$(ggzdatadir)/kreversi/pixmaps/$$p ; \
		fi \
	done

uninstall-local:
	-rm -rv $(ggzdatadir)/kreversi
