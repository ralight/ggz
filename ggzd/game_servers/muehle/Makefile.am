## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6258 2004-11-04 23:00:59Z jdorje $

SUBDIRS = qtserv ggz variants

ggzdgame_DATA = muehle.dsc
ggzdroom_DATA = muehle.room

EXTRA_DIST = $(ggzdgame_DATA) $(ggzdroom_DATA)

ggzdexecmod_PROGRAMS = muehleserv
muehleserv_SOURCES = muehleserver.cpp loader.cpp main.cpp net.cpp
muehleserv_LDADD = $(LDADD) \
			ggz/libggzgameserver.a \
			qtserv/libqtserv.a \
			$(top_builddir)/ggzdmod/libggzdmod.la

noinst_HEADERS = muehleserver.h loader.h net.h

AM_CPPFLAGS = -I$(top_srcdir)/game_servers/muehle/ggz \
	   -I$(top_srcdir)/game_servers/muehle/qtserv \
	   -I$(top_srcdir)/ggzdmod \
	   $(LIBGGZ_INCLUDES)

test: test-recursive
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
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
