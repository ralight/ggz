SUBDIRS =

# fake package name
PACKAGE = kggz

POFILES = de.po pt_BR.po es.po fr.po

# don't do this! maintainer only (will mess up non-latin1 messages)
messages-update:
	-@for i in $(POFILES); do \
		echo "Language: $$i"; \
		msgmerge $$i kggz.pot > .$$i; \
		mv .$$i $$i; \
	done

