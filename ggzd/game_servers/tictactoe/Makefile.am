## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 2295 2001-08-28 04:30:16Z jdorje $

EXTRA_DIST = tictactoe.dsc tictactoe.room

SUBDIRS = .

ggzdexecmod_PROGRAMS = ggzd.tictactoe
ggzdgame_DATA = tictactoe.dsc
ggzdroom_DATA = tictactoe.room

ggzd_tictactoe_SOURCES = main.c \
		         game.c \
                         game.h


ggzd_tictactoe_LDADD = $(LDADD) ../libggzmod/libggzdmod.a

ggzd_tictactoe_DEPENDENCIES = ../libggzmod/libggzdmod.a

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

