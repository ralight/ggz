## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 4599 2002-09-17 03:28:35Z jdorje $

SUBDIRS = src

ggzdgame_DATA = keepalive.dsc
ggzdroom_DATA = keepalive.room

EXTRA_DIST = README.keepalive $(ggzdgame_DATA) $(ggzdroom_DATA)

test: test-recursive
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

test-recursive:
	@set fnord $(MAKEFLAGS); amf=$$2; \
	dot_seen=no; \
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
	fi; test -z "$$fail"

test-am: # avoid warning
