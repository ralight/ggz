## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 2244 2001-08-25 15:21:40Z jdorje $

EXTRA_DIST = hastings.dsc hastings.room

SUBDIRS = .

ggzdexecmod_PROGRAMS = ggzd.hastings
ggzdgame_DATA = hastings.dsc
ggzdroom_DATA = hastings.room

ggzd_hastings_SOURCES =  main.c \
		         game.c \
                         game.h

ggzd_hastings_DEPENDENCIES = ../libggzmod/libggzdmod.a

ggzd_hastings_LDADD = $(LDADD) ../libggzmod/libggzdmod.a

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

