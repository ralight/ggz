## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 3802 2002-04-07 07:18:01Z dr_maux $

SUBDIRS = stackpath zoneserver

ggzdexecmod_PROGRAMS = krosswater_server

krosswater_server_SOURCES = main.cpp krosswater_server.h krosswater_server.cpp
krosswater_server_LDADD = zoneserver/libzoneserver.la \
	stackpath/libstackpath.a \
	$(top_builddir)/ggzdmod/libggzdmod.a

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

