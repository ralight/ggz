SUBDIRS = stackpath zoneserver

ggzdexecmod_PROGRAMS = krosswater_server

krosswater_server_SOURCES = main.cpp krosswater_server.h krosswater_server.cpp
krosswater_server_LDADD = zoneserver/libzoneserver.la stackpath/libstackpath.a \
	$(top_srcdir)/ggzdmod/libggzdmod.la -leasysock

INCLUDES = -I $(top_srcdir)/game_servers/krosswater/stackpath  \
	   -I $(top_srcdir)/game_servers/krosswater/zoneserver \
	   -I $(top_srcdir)/ggzdmod

ggzdroom_DATA = krosswater.room
ggzdgame_DATA = krosswater.dsc

EXTRA_DIST = krosswater.dsc krosswater.room

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

