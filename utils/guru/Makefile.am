SUBDIRS = src modules .

gurui18n: guruclean
	mkdir tmp
	touch data/de.po data/fr.po
	xgettext -k_ src/test_i18n.c -o data/guru.pot
	msgmerge data/de.po data/guru.pot > .de.po 2>/dev/null
	mv .de.po data/de.po
	msgfmt -o tmp/guru.mo.de data/de.po
	msgmerge data/fr.po data/guru.pot > .fr.po 2>/dev/null
	mv .fr.po data/fr.po
	msgfmt -o tmp/guru.mo.fr data/fr.po

guruclean:
	rm -rf tmp

guruinstall: gurui18n
	mkdir tmp/de
	mkdir tmp/fr
	mkdir tmp/de/LC_MESSAGES
	mkdir tmp/fr/LC_MESSAGES
	cp tmp/guru.mo.de tmp/de/LC_MESSAGES/guru.mo
	cp tmp/guru.mo.fr tmp/fr/LC_MESSAGES/guru.mo


