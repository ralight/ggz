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
