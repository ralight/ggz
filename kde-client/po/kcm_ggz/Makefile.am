SUBDIRS = .

PACKAGE = kcmggz
POTFILE = kcm_ggz.pot
POFILES = de.po
LOCATIONS = $(top_srcdir)/kcm_ggz/*.cpp $(top_srcdir)/kcm_ggz/modules/*.cpp

# don't do this! maintainer only (will mess up non-latin1 messages)
messages:
	touch $(POTFILE)
	$(XGETTEXT) -s -j -C -kI18N_NOOP -ki18n $(LOCATIONS) -o $(POTFILE)
	@for i in $(POFILES); do \
		echo "process: $$i"; \
		msgmerge $$i $(POTFILE) > .$$i 2>/dev/null; \
		mv .$$i $$i; \
	done

