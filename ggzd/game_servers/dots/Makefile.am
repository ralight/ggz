## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 2769 2001-12-01 06:53:01Z bmh $

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

INCLUDES = -I$(top_srcdir)/ggzdmod
ggzd_dots_LDADD = $(LDADD) $(top_builddir)/ggzdmod/libggzdmod.a

ggzd_dots_DEPENDENCIES = $(top_builddir)/ggzdmod/libggzdmod.a

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

