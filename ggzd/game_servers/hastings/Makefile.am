## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 2327 2001-08-30 00:58:20Z jdorje $

EXTRA_DIST = hastings.dsc hastings.room

SUBDIRS = .

ggzdexecmod_PROGRAMS = ggzd.hastings
ggzdgame_DATA = hastings.dsc
ggzdroom_DATA = hastings.room

ggzd_hastings_SOURCES =  main.c \
		         game.c \
                         game.h

ggzd_hastings_DEPENDENCIES = $(top_builddir)/game_servers/libggzmod/libggzdmod.a

ggzd_hastings_LDADD = $(LDADD) $(top_builddir)/game_servers/libggzmod/libggzdmod.a

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

