## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 2327 2001-08-30 00:58:20Z jdorje $

EXTRA_DIST = ccheckers.dsc ccheckers.room

SUBDIRS = .

ggzdexecmod_PROGRAMS = ggzd.ccheckers
ggzdgame_DATA = ccheckers.dsc
ggzdroom_DATA = ccheckers.room

ggzd_ccheckers_SOURCES = main.c \
			 game.c \
			 game.h \
			 ai.c \
			 ai.h


ggzd_ccheckers_LDADD = $(LDADD) $(top_builddir)/game_servers/libggzmod/libggzdmod.a

ggzd_ccheckers_DEPENDENCIES = $(top_builddir)/game_servers/libggzmod/libggzdmod.a

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

