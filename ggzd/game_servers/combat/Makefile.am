## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 2299 2001-08-28 05:13:32Z jdorje $

EXTRA_DIST = combat.dsc combat.room

SUBDIRS = .

ggzdexecmod_PROGRAMS = ggzd.combat
ggzdgame_DATA = combat.dsc
ggzdroom_DATA = combat.room


ggzd_combat_SOURCES = main.c \
		      game.c \
		      game.h \
		      combat.h \
		      combat.c


ggzd_combat_LDADD = $(LDADD) ../libggzmod/libggzdmod.a

ggzd_combat_DEPENDENCIES = ../libggzmod/libggzdmod.a

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms
