SUBDIRS = stackpath zoneserver

EXTRA_DIST = krosswater.dsc krosswater.room

ggzdexecmod_PROGRAMS = krosswater_server
ggzdroom_DATA = krosswater.room
ggzdgame_DATA = krosswater.dsc

INCLUDES = -I $(top_srcdir)/game_servers/krosswater/stackpath -I $(top_srcdir)/game_servers/krosswater/zoneserver

krosswater_server_SOURCES = main.cpp krosswater_server.h krosswater_server.cpp

krosswater_server_LDADD = zoneserver/libzoneserver.la stackpath/libstackpath.a -leasysock

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms
