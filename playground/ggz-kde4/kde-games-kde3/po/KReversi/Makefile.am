XGETTEXT = xgettext
MSGMERGE = msgmerge
GMSGFMT = msgfmt

PACKAGE = ggz.kreversi
POTFILE = kreversi.pot
POFILES = de.po es.po
LOCATIONS = $(top_srcdir)/KReversi/*.cpp

EXTRA_DIST = $(POTFILE) $(POFILES)

# don't do this! maintainer only (will mess up non-latin1 messages)
messages:
	touch $(POTFILE)
	$(XGETTEXT) -s -j -C -kI18N_NOOP -ki18n $(LOCATIONS) -o $(POTFILE)
	@for i in $(POFILES); do \
		echo "process: $$i"; \
		msgmerge $$i $(POTFILE) > .$$i 2>/dev/null; \
		mv .$$i $$i; \
	done

