## Process this file with automake to produce Makefile.in

DIST_SUBDIRS = ggzdmod ggzd game_servers man tests
CLEAN_FILES = ggzd.debug ggzd.log
SUBDIRS = @SUBDIRS@

EXTRA_DIST = HACKING README.GGZ \
		m4/c99.m4 m4/ggz.m4 m4/ggz-debug.m4 m4/iconv.m4

test: all testdir test-recursive

testdir:
	-rm -rf test
	-mkdir -p test
	-mkdir -p test/games
	-mkdir -p test/rooms

test-recursive:
	@dot_seen=no; \
	target=`echo $@ | sed s/-recursive//`; \
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  echo "Making $$target in $$subdir"; \
	  if test "$$subdir" = "."; then \
	    dot_seen=yes; \
	    local_target="$$target-am"; \
	  else \
	    local_target="$$target"; \
	  fi; \
	  (cd $$subdir && $(MAKE) -s $(AM_MAKEFLAGS) $$local_target) \
	done; \
	if test "$$dot_seen" = "no"; then \
	  $(MAKE) -s $(AM_MAKEFLAGS) "$$target-am" || exit 1; \
	fi; 

test-am:
	@echo; echo "You may now run $(top_builddir)/ggzd/ggzd -F -f $(top_builddir)/ggzd/ggzd.test to test server"; echo

#install-data-local:
#	@$(NORMAL_INSTALL)
#	if test -d $(srcdir)/pixmaps; then \
#	  $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/pixmaps; \
#	  for pixmap in $(srcdir)/pixmaps/*; do \
#	    if test -f $$pixmap; then \
#	      $(INSTALL_DATA) $$pixmap $(DESTDIR)$(pkgdatadir)/pixmaps; \
#	    fi \
#	  done \
#	fi

#dist-hook:
#	if test -d pixmaps; then \
#	  mkdir $(distdir)/pixmaps; \
#	  for pixmap in pixmaps/*; do \
#	    if test -f $$pixmap; then \
#	      cp -p $$pixmap $(distdir)/pixmaps; \
#	    fi \
#	  done \
#	fi

