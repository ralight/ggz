## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 2649 2001-11-04 17:33:57Z jdorje $

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


ggzd_combat_LDADD = $(LDADD) $(top_builddir)/ggzdmod/libggzdmod.a

ggzd_combat_DEPENDENCIES = $(top_builddir)/ggzdmod/libggzdmod.a

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms
