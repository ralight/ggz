SUBDIRS = stackpath zoneserver

ggzdexecmod_PROGRAMS = krosswater_server

krosswater_server_SOURCES = main.cpp krosswater_server.h krosswater_server.cpp
krosswater_server_LDADD = $(LDADD) \
	zoneserver/libzoneserver.la \
	stackpath/libstackpath.a \
	$(top_builddir)/ggzdmod/libggzdmod.la

AM_CPPFLAGS = -I $(top_srcdir)/game_servers/krosswater/stackpath  \
	   -I $(top_srcdir)/game_servers/krosswater/zoneserver \
	   -I $(top_srcdir)/ggzdmod \
	   $(LIBGGZ_INCLUDES)

dist_ggzdroom_DATA = krosswater.room
dist_ggzdgame_DATA = krosswater.dsc

