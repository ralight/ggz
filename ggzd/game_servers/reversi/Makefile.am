## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 3880 2002-04-10 13:53:51Z dr_maux $

EXTRA_DIST = reversi.dsc reversi.room

SUBDIRS = .

ggzdexecmod_PROGRAMS = ggzd.reversi
ggzdgame_DATA = reversi.dsc
ggzdroom_DATA = reversi.room


ggzd_reversi_SOURCES = 	main.c \
			game.c \
			game.h


INCLUDES = -I$(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES)
ggzd_reversi_LDADD = $(LDADD) $(top_builddir)/ggzdmod/libggzdmod.a

ggzd_reversi_DEPENDENCIES = $(top_builddir)/ggzdmod/libggzdmod.a

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

