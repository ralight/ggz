## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 2300 2001-08-28 05:35:33Z jdorje $

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


ggzd_ccheckers_LDADD = $(LDADD) ../libggzmod/libggzdmod.a

ggzd_checkers_DEPENDENCIES = ../libggzmod/libggzdmod.a

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

