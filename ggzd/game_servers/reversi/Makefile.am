## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 2280 2001-08-27 18:29:25Z jdorje $

EXTRA_DIST = reversi.dsc reversi.room

SUBDIRS = .

ggzdexecmod_PROGRAMS = ggzd.reversi
ggzdgame_DATA = reversi.dsc
ggzdroom_DATA = reversi.room


ggzd_reversi_SOURCES = 	main.c \
			game.c \
			game.h


ggzd_reversi_LDADD = $(LDADD) -leasysock ../libggzmod/libggzdmod.a

ggzd_reversi_DEPENDENCIES = ../libggzmod/libggzdmod.a

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

