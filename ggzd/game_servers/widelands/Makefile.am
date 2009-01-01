dist_ggzdgame_DATA = widelands.dsc
dist_ggzdroom_DATA = widelands.room

ggzdexecmod_PROGRAMS = widelands_server
widelands_server_SOURCES = main.cpp widelands_server.cpp
widelands_server_LDADD = $(LDADD) \
			$(top_builddir)/ggzdmod++/libggzdmod++.la

noinst_HEADERS = widelands_server.h protocol.h

AM_CPPFLAGS = -I$(top_srcdir)/ggzdmod++/ \
	   -I$(top_srcdir)/ggzdmod \
	   $(LIBGGZ_INCLUDES)
