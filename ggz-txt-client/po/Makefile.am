SUBDIRS = .

EXTRA_DIST = ggz-txt.pot de.po

messages:
	$(XGETTEXT) -k_ ../ggz-txt/*.c -o ggz-txt.pot; \
	for j in *.po; do \
		echo "process $$j"; \
		$(MSGMERGE) $$j ggz-txt.pot > .$$j 2>/dev/null; \
		mv .$$j $$j; \
	done;

install-data-local:
	for j in *.po; do \
		country=`echo $$j | cut -d "." -f 1`; \
		dir=$(DESTDIR)$(prefix)/share/locale/$$country/LC_MESSAGES; \
		echo "installing $$dir/ggz-txt.mo ($$j)"; \
		install -d $$dir; \
		$(MSGFMT) -o $$dir/ggz-txt.mo $$j; \
	done;

