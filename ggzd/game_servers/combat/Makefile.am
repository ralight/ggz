## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 3880 2002-04-10 13:53:51Z dr_maux $

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


INCLUDES = -I$(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES)
ggzd_combat_LDADD = $(LDADD) $(top_builddir)/ggzdmod/libggzdmod.a

ggzd_combat_DEPENDENCIES = $(top_builddir)/ggzdmod/libggzdmod.a

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms
