gurudatadir = $(prefix)/share/grubby
dist_gurudata_SCRIPTS = \
	dict.pl \
	rdf.pl \
	meeting.rb \
	messages.rb \
	embedtest.rb \
	fortune.py \
	phrasecatcher.py \
	statistics.py

guruconfigdir = $(prefix)/bin
guruconfig_SCRIPTS = grubby-config

EXTRA_DIST = grubby-config.in webtranslator.sh test.tcl \
	guru.pot de.po fr.po pt_BR.po

messages:
	touch guru.pot
	$(XGETTEXT) -k__ -kN_ -ki18n ../modules/*.c ../src/*.c ../../games/*/*.c *.pl -o guru.pot
	@for j in `ls -1 *.po`; do \
		echo "process $$j"; \
		$(MSGMERGE) $$j guru.pot > .$$j 2>/dev/null; \
		mv .$$j $$j; \
	done

install-data-local:
	for j in `ls -1 *.po`; do \
		country=`echo $$j | cut -d "." -f 1`; \
		dir=$(DESTDIR)$(prefix)/share/locale/$$country/LC_MESSAGES; \
		echo "installing $$dir/guru.mo ($$j)"; \
		install -d $$dir; \
		$(MSGFMT) -o $$dir/guru.mo $$j; \
	done

