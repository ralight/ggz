## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 2649 2001-11-04 17:33:57Z jdorje $

EXTRA_DIST = tictactoe.dsc tictactoe.room

SUBDIRS = .

ggzdexecmod_PROGRAMS = ggzd.tictactoe
ggzdgame_DATA = tictactoe.dsc
ggzdroom_DATA = tictactoe.room

ggzd_tictactoe_SOURCES = main.c \
		         game.c \
                         game.h


ggzd_tictactoe_LDADD = $(LDADD) $(top_builddir)/ggzdmod/libggzdmod.a

ggzd_tictactoe_DEPENDENCIES = $(top_builddir)/ggzdmod/libggzdmod.a

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

