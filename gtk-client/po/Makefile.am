SUBDIRS = .

XGETTEXT = xgettext
MSGMERGE = msgmerge
MSGFMT = msgfmt

EXTRA_DIST = ggz-gtk.pot de.po es_AR.po pt_BR.po

CATALOG = ggz-gtk

messages:
	$(XGETTEXT) -k_ -kN_ ../ggz-gtk/*.c -o $(CATALOG).pot
	@for j in `ls -1 *.po`; do \
		echo "process $$j"; \
		$(MSGMERGE) $$j $(CATALOG).pot > .$$j 2>/dev/null; \
		mv .$$j $$j; \
	done

install-data-local:
	for j in $(top_srcdir)/po/*.po; do \
		k=$$(basename $$j); \
		country=`echo $$k | cut -d "." -f 1`; \
		dir=$(prefix)/share/locale/$$country/LC_MESSAGES; \
		echo "installing $$dir/$(CATALOG).mo ($$k)"; \
		install -d $$dir; \
		$(MSGFMT) -o $$dir/$(CATALOG).mo $$j; \
	done

uninstall-local:
	for j in $(top_srcdir)/po/*.po; do \
		k=$$(basename $$j); \
		country=`echo $$k | cut -d "." -f 1`; \
		dir=$(prefix)/share/locale/$$country/LC_MESSAGES; \
		rm -f $$dir/$(CATALOG).mo; \
	done

