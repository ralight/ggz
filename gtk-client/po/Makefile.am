SUBDIRS = .

XGETTEXT = xgettext
MSGMERGE = msgmerge
MSGFMT = msgfmt

messages:
	$(XGETTEXT) -k_ -kN_ ../ggz-gtk/*.c -o ggz-gtk.pot
	@for j in `ls -1 *.po`; do \
		echo "process $$j"; \
		$(MSGMERGE) $$j ggz-gtk.pot > .$$j 2>/dev/null; \
		mv .$$j $$j; \
	done

install-data-local:
	for j in $(top_srcdir)/po/*.po; do \
		k=$$(basename $$j); \
		country=`echo $$k | cut -d "." -f 1`; \
		dir=$(prefix)/share/locale/$$country/LC_MESSAGES; \
		echo "installing $$dir/ggz-gtk.mo ($$k)"; \
		install -d $$dir; \
		$(MSGFMT) -o $$dir/ggz-gtk.mo $$j; \
	done

