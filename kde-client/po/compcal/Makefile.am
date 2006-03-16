PACKAGE = compcal
POTFILE = compcal.pot
POFILES = de.po fr.po
LOCATIONS = $(top_srcdir)/compcal/src/*.cpp

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

