## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 2297 2001-08-28 04:38:37Z jdorje $

EXTRA_DIST = dots.dsc dots.room

SUBDIRS = .

ggzdexecmod_PROGRAMS = ggzd.dots
ggzdgame_DATA = dots.dsc
ggzdroom_DATA = dots.room

ggzd_dots_SOURCES = main.c \
		    game.c \
		    game.h \
		    ai.c \
		    ai.h

ggzd_dots_LDADD = $(LDADD) ../libggzmod/libggzdmod.a

ggzd_dots_DEPENDENCIES = ../libggzmod/libggzdmod.a

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

