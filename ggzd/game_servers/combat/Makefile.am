## Process this file with automake to produce Makefile.in

SUBDIRS = .

ggzdexecmod_PROGRAMS = ggzd.combat
ggzdgame_DATA = combat.dsc
ggzdroom_DATA = combat.room


ggzd_combat_SOURCES = ggz.c \
		         ggz.h \
		         main.c \
						 game.c \
						 game.h \
						 combat.h \
						 combat.c \
		         protocols.h


ggzd_combat_LDADD = $(LDADD)

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

