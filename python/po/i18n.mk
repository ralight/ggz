# Common i18n message catalog handling
#
# CATALOG = <name> (for <name>.mo)
# POSOURCES = <filelist> (like ($top_srcdir)/*.c)
# optional: POFLAGS = ... (like -LPython)
# optional: CATALOG_VERSION = _... (for i18n'd libraries)

builddir = $(top_builddir)/$(subdir)
POFILES = $(srcdir)/*.po
MOFILES = $(builddir)/*.mo
STAMP = $(builddir)/translation.stamp
SHELL = /bin/bash

all-local: $(STAMP)

messages:
	@echo "updating catalog $(CATALOG)"
	@$(XGETTEXT) -k_ -kN_ $(POFLAGS) $(POSOURCES) -o $(CATALOG).pot;
	@for j in $(POFILES); do \
		echo "process $$j"; \
		$(MSGMERGE) $$j $(CATALOG).pot > .$$j 2>/dev/null; \
		mv .$$j $$j; \
	done

$(STAMP): $(POFILES)
	@for j in $(POFILES); do \
		echo "converting $$j"; \
		k=$$(basename $$j); \
		country=`echo $$k | cut -d "." -f 1`; \
		$(MSGFMT) -o $$country.mo $$j; \
	done
	@touch $(STAMP)

clean-local:
	@for j in $(MOFILES); do \
		rm -f $$j; \
	done
	@rm -f $(STAMP)

install-data-local:
	@for j in $(MOFILES); do \
		k=$$(basename $$j); \
		country=`echo $$k | cut -d "." -f 1`; \
		dir=${DESTDIR}$(prefix)/share/locale/$$country/LC_MESSAGES; \
		echo "installing $$dir/$(CATALOG)$(CATALOG_VERSION).mo ($$k)"; \
		install -d $$dir; \
		cp $$j $$dir/$(CATALOG)$(CATALOG_VERSION).mo; \
	done

uninstall-local:
	@for j in $(POFILES); do \
		k=$$(basename $$j); \
		country=`echo $$k | cut -d "." -f 1`; \
		dir=${DESTDIR}$(prefix)/share/locale/$$country/LC_MESSAGES; \
		rm -f $$dir/$(CATALOG)$(CATALOG_VERSION).mo; \
	done

