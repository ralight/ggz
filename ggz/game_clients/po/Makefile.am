SUBDIRS = .

XGETTEXT = xgettext
MSGMERGE = msgmerge
MSGFMT = msgfmt

MODULES = hastings tictactoe dots chess combat ggzcards reversi chinese-checkers lapocha spades

messages:
	@for i in $(MODULES); do \
		$(XGETTEXT) -k_ ../$$i/*.c ../common/*.c -o $$i/$$i.pot; \
		for j in `cd $$i; ls -1 *.po`; do \
			echo "process $$i.$$j"; \
			$(MSGMERGE) $$i/$$j $$i/$$i.pot > .$$j 2>/dev/null; \
			mv .$$j $$i/$$j; \
		done; \
	done

install-data-local:
	@for i in $(MODULES); do \
		for j in `cd $$i; ls -1 *.po`; do \
			country=`echo $$j | cut -d "." -f 1`; \
			dir=$(prefix)/share/locale/$$country/LC_MESSAGES; \
			echo "installing $$dir/$$i.mo ($$j)"; \
			install -d $$dir; \
			$(MSGFMT) -o $$dir/$$i.mo $$i/$$j; \
		done; \
	done

