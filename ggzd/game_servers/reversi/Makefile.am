## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 3161 2002-01-20 09:00:05Z jdorje $

EXTRA_DIST = reversi.dsc reversi.room

SUBDIRS = .

ggzdexecmod_PROGRAMS = ggzd.reversi
ggzdgame_DATA = reversi.dsc
ggzdroom_DATA = reversi.room


ggzd_reversi_SOURCES = 	main.c \
			game.c \
			game.h


INCLUDES = -I$(top_srcdir)/ggzdmod
ggzd_reversi_LDADD = $(LDADD) $(top_builddir)/ggzdmod/libggzdmod.a

ggzd_reversi_DEPENDENCIES = $(top_builddir)/ggzdmod/libggzdmod.a

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

