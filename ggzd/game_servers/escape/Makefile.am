## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 2769 2001-12-01 06:53:01Z bmh $

EXTRA_DIST = escape.dsc escape.room

SUBDIRS = .

ggzdexecmod_PROGRAMS = ggzd.escape
ggzdgame_DATA = escape.dsc
ggzdroom_DATA = escape.room

ggzd_escape_SOURCES = main.c \
		      game.c \
                      game.h

INCLUDES = -I$(top_srcdir)/ggzdmod
ggzd_escape_DEPENDENCIES = $(top_builddir)/ggzdmod/libggzdmod.a

ggzd_escape_LDADD = $(LDADD) $(top_builddir)/ggzdmod/libggzdmod.a

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

