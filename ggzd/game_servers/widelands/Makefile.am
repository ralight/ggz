SUBDIRS = lib

ggzdgame_DATA = widelands.dsc
ggzdroom_DATA = widelands.room

EXTRA_DIST = $(ggzdgame_DATA) $(ggzdroom_DATA)

ggzdexecmod_PROGRAMS = widelands_server
widelands_server_SOURCES = main.cpp widelands_server.cpp
widelands_server_LDADD = $(LDADD) \
			lib/libggzgameserver.a \
			$(top_builddir)/ggzdmod/libggzdmod.la

noinst_HEADERS = widelands_server.h protocol.h

AM_CPPFLAGS = -I$(top_srcdir)/game_servers/widelands/lib \
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
