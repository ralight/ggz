## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 5299 2002-12-18 11:49:40Z oojah $

EXTRA_DIST = connectx.dsc connectx.room

SUBDIRS = .

ggzdexecmod_PROGRAMS = ggzd.connectx
ggzdgame_DATA = connectx.dsc
ggzdroom_DATA = connectx.room

ggzd_escape_SOURCES = main.c \
		      game.c \
                      game.h

INCLUDES = -I$(top_srcdir)/ggzdmod

ggzd_escape_LDADD = $(LDADD) $(top_builddir)/ggzdmod/libggzdmod.la

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

