## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 6478 2004-12-15 00:17:01Z josef $

MODULES = chess chinese-checkers combat dots ggzcards hastings reversi spades tictactoe

SUBDIRS = .
EXTRA_DIST = $(MODULES) common

XGETTEXT = xgettext
MSGMERGE = msgmerge
MSGFMT = msgfmt

messages:
	@for i in common $(MODULES); do \
		$(XGETTEXT) -k_ ../$$i/*.c ../common/*.c -o $$i/$$i.pot \
		  --keyword=N_; \
		for j in `cd $$i; ls -1 *.po`; do \
			echo "process $$i.$$j"; \
			$(MSGMERGE) -C common/$$j $$i/$$j $$i/$$i.pot > .$$j 2>/dev/null; \
			mv .$$j $$i/$$j; \
		done; \
	done

install-data-local:
	@for i in $(MODULES); do \
		for j in $(top_srcdir)/po/$$i/*.po; do \
			k=$$(basename $$j); \
			country=`echo $$k | cut -d "." -f 1`; \
			dir=$(DESTDIR)$(prefix)/share/locale/$$country/LC_MESSAGES; \
			echo "installing $$dir/$$i.mo ($$k)"; \
			install -d $$dir; \
			$(MSGFMT) -o $$dir/$$i.mo $$j; \
		done; \
	done

uninstall-local:
	@for i in $(MODULES); do \
		for j in $(top_srcdir)/po/$$i/*.po; do \
			k=$$(basename $$j); \
			country=`echo $$k | cut -d "." -f 1`; \
			dir=$(DESTDIR)$(prefix)/share/locale/$$country/LC_MESSAGES; \
			rm -f $$dir/$$i.mo; \
		done; \
	done

