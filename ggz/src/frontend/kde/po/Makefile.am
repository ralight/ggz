SUBDIRS = .

# fake package name
PACKAGE = kggz

POFILES = de.po pt_BR.po es.po

# don't do this! maintainer only (will mess up non-latin1 messages)
messages:
	touch kggz.pot
	$(XGETTEXT) -s -j -C -kI18N_NOOP -ki18n $(top_srcdir)/ggz-kde/src/*.cpp $(top_srcdir)/ggz-kde/src/widgets/*.cpp $(top_srcdir)/ggz-kde/src/widgets/more/*.cpp -o kggz.pot
	@for i in $(POFILES); do \
		echo "process: $$i"; \
		msgmerge $$i kggz.pot > .$$i 2>/dev/null; \
		mv .$$i $$i; \
	done

