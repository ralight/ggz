## Process this file with automake to produce Makefile.in

SUBDIRS = @SUBDIRS@ po intl
DIST_SUBDIRS = easysock game_clients src po intl

EXTRA_DIST = HACKING

#ggzconf_DATA = ggz.rc
#
#ggz.rc:	Makefile
#	rm -f ggz.rc
#	touch ggz.rc
#	find game_clients -name \*.dsc -exec cat {} > ggz.rc \;

install-data-local:
	@$(NORMAL_INSTALL)
	if test -d $(srcdir)/pixmaps; then \
	  $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/pixmaps; \
	  for pixmap in $(srcdir)/pixmaps/*; do \
	    if test -f $$pixmap; then \
	      $(INSTALL_DATA) $$pixmap $(DESTDIR)$(pkgdatadir)/pixmaps; \
	    fi \
	  done \
	fi

dist-hook:
	if test -d pixmaps; then \
	  mkdir $(distdir)/pixmaps; \
	  for pixmap in pixmaps/*; do \
	    if test -f $$pixmap; then \
	      cp -p $$pixmap $(distdir)/pixmaps; \
	    fi \
	  done \
	fi

