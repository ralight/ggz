## Process this file with automake to produce Makefile.in

EXTRA_DIST = reversi.dsc reversi.room

SUBDIRS = .

ggzdexecmod_PROGRAMS = ggzd.reversi
ggzdgame_DATA = reversi.dsc
ggzdroom_DATA = reversi.room


ggzd_reversi_SOURCES = ggz.c \
		         ggz.h \
		         main.c \
						 game.c \
						 game.h \
		         protocols.h


ggzd_reversi_LDADD = $(LDADD)

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

