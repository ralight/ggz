SUBDIRS = .

ggzdgame_DATA = geekgame.dsc
ggzdroom_DATA = geekgame.room

EXTRA_DIST = $(ggzdgame_DATA) $(ggzdroom_DATA)

ggzdexecmod_PROGRAMS = geekgameserver
geekgameserver_SOURCES = main.cpp geekgame.cpp
geekgameserver_LDADD = $(LDADD) \
	$(top_builddir)/game_servers/muehle/ggz/libggzgameserver.a \
	$(top_builddir)/ggzdmod/libggzdmod.la

noinst_HEADERS = geekgame.h protocol.h

AM_CPPFLAGS = -I$(top_srcdir)/game_servers/muehle/ggz \
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
