prefix = /usr/local

XGETTEXT = xgettext
MSGMERGE = msgmerge
MSGFMT = msgfmt

SOURCES = ../lib/sdlnewstuff.py ../ggzboard/ggzboard ../ggzboard/module_*.py

all:
	for j in `ls -1 *.po`; do \
		country=`echo $$j | cut -d "." -f 1`; \
		echo "formatting $$country.mo ($$j)"; \
		$(MSGFMT) -o $$country.mo $$j; \
	done

install: install-data-local

messages:
	$(XGETTEXT) -k_ -kN_ -LPython $(SOURCES) -o ggzpython.pot; \
	for j in `ls -1 *.po`; do \
		echo "merge $$j"; \
		$(MSGMERGE) $$j ggzpython.pot > .$$j 2>/dev/null; \
		mv .$$j $$j; \
	done

install-data-local:
	for j in `ls -1 *.mo`; do \
		country=`echo $$j | cut -d "." -f 1`; \
		dir=$(DESTDIR)$(prefix)/share/locale/$$country/LC_MESSAGES; \
		echo "installing $$dir/ggzpython.mo ($$j)"; \
		install -d $$dir; \
		cp $$j $$dir/ggzpython.mo; \
	done

uninstall-local:
	for j in `ls -1 *.po`; do \
		country=`echo $$j | cut -d "." -f 1`; \
		dir=$(DESTDIR)$(prefix)/share/locale/$$country/LC_MESSAGES; \
		echo "uninstalling $$dir/ggzpython.mo ($$j)"; \
		rm -f $$dir/ggzpython.mo; \
	done

clean:
	for j in `ls -1 *.mo`; do \
		echo "deleting $$j"; \
		rm -f $j; \
	done

