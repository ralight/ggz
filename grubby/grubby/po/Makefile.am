SUBDIRS = .

EXTRA_DIST = grubby.pot de.po

messages:
	$(XGETTEXT) -k_ ../src/*.c -o grubby.pot; \
	for j in *.po; do \
		echo "process $$j"; \
		$(MSGMERGE) $$j grubby.pot > .$$j 2>/dev/null; \
		mv .$$j $$j; \
	done;

install-data-local:
	for j in $(top_srcdir)/grubby/po/*.po; do \
		k=$$(basename $$j); \
		country=`echo $$k | cut -d "." -f 1`; \
		dir=$(DESTDIR)$(prefix)/share/locale/$$country/LC_MESSAGES; \
		echo "installing $$dir/grubby.mo ($$k)"; \
		install -d $$dir; \
		$(MSGFMT) -o $$dir/grubby.mo $$j; \
	done;

