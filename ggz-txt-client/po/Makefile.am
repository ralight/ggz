SUBDIRS = .

EXTRA_DIST = ggz-txt.pot de.po

CATALOG = ggz-txt

messages:
	$(XGETTEXT) -k_ ../ggz-txt/*.c -o $(CATALOG).pot; \
	for j in *.po; do \
		echo "process $$j"; \
		$(MSGMERGE) $$j $(CATALOG).pot > .$$j 2>/dev/null; \
		mv .$$j $$j; \
	done;

all-local: translation-stamp

translation-stamp:
	@for j in $(top_srcdir)/po/*.po; do \
		echo "convert $$j"; \
		k=$$(basename $$j); \
		country=`echo $$k | cut -d "." -f 1`; \
		$(MSGFMT) -o $$country.mo $$j; \
	done
	touch translation-stamp

clean-local:
	@for j in $(top_builddir)/po/*.mo; do \
		rm -f $$j; \
	done
	rm -f $(top_builddir)/po/translation-stamp

install-data-local:
	@for j in $(top_builddir)/po/*.mo; do \
		k=$$(basename $$j); \
		country=`echo $$k | cut -d "." -f 1`; \
		dir=$(prefix)/share/locale/$$country/LC_MESSAGES; \
		echo "installing $$dir/$(CATALOG).mo ($$k)"; \
		install -d $$dir; \
		cp $$j $$dir/$(CATALOG).mo; \
	done

uninstall-local:
	for j in $(top_srcdir)/po/*.po; do \
		k=$$(basename $$j); \
		country=`echo $$k | cut -d "." -f 1`; \
		dir=$(prefix)/share/locale/$$country/LC_MESSAGES; \
		rm -f $$dir/$(CATALOG).mo; \
	done

