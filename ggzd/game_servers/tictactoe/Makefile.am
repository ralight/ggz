## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 2327 2001-08-30 00:58:20Z jdorje $

EXTRA_DIST = tictactoe.dsc tictactoe.room

SUBDIRS = .

ggzdexecmod_PROGRAMS = ggzd.tictactoe
ggzdgame_DATA = tictactoe.dsc
ggzdroom_DATA = tictactoe.room

ggzd_tictactoe_SOURCES = main.c \
		         game.c \
                         game.h


ggzd_tictactoe_LDADD = $(LDADD) $(top_builddir)/game_servers/libggzmod/libggzdmod.a

ggzd_tictactoe_DEPENDENCIES = $(top_builddir)/game_servers/libggzmod/libggzdmod.a

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

