## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 4669 2002-09-23 16:31:25Z jdorje $

SUBDIRS = .

XGETTEXT = xgettext
MSGMERGE = msgmerge
MSGFMT = msgfmt

MODULES = chess chinese-checkers combat dots ggzcards hastings lapocha reversi spades tictactoe

messages:
	@for i in $(MODULES); do \
		$(XGETTEXT) -k_ ../$$i/*.c ../common/*.c -o $$i/$$i.pot \
		  --keyword=N_; \
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
			dir=$(DESTDIR)$(prefix)/share/locale/$$country/LC_MESSAGES; \
			echo "installing $$dir/$$i.mo ($$j)"; \
			install -d $$dir; \
			$(MSGFMT) -o $$dir/$$i.mo $$i/$$j; \
		done; \
	done

