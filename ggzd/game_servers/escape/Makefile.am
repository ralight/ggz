## Process this file with automake to produce Makefile.in

EXTRA_DIST = escape.dsc escape.room

SUBDIRS = .

ggzdexecmod_PROGRAMS = ggzd.escape
ggzdgame_DATA = escape.dsc
ggzdroom_DATA = escape.room

ggzd_escape_SOURCES = main.c \
		      game.c \
                      game.h


ggzd_escape_DEPENDENCIES = ../libggzmod/libggzdmod.a

ggzd_escape_LDADD = $(LDADD) ../libggzmod/libggzdmod.a

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

