SUBDIRS = .

XGETTEXT = xgettext
MSGMERGE = msgmerge
MSGFMT = msgfmt

messages:
	$(XGETTEXT) -k_ -kN_ ../src/*.c -o ggz-gnome-client.pot
	@for j in `ls -1 *.po`; do \
		echo "process $$j"; \
		$(MSGMERGE) $$j ggz-gnome-client.pot > .$$j 2>/dev/null; \
		mv .$$j $$j; \
	done

install-data-local:
	for j in `ls -1 *.po`; do \
		country=`echo $$j | cut -d "." -f 1`; \
		dir=$(prefix)/share/locale/$$country/LC_MESSAGES; \
		echo "installing $$dir/ggz-gnome-client.mo ($$j)"; \
		install -d $$dir; \
		$(MSGFMT) -o $$dir/ggz-gnome-client.mo $$j; \
	done

