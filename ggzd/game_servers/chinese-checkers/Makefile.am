## Process this file with automake to produce Makefile.in

EXTRA_DIST = ccheckers.dsc ccheckers.room

SUBDIRS = .

ggzdexecmod_PROGRAMS = ggzd.ccheckers
ggzdgame_DATA = ccheckers.dsc
ggzdroom_DATA = ccheckers.room

ggzd_ccheckers_SOURCES = ggz.c \
			 ggz.h \
			 main.c \
			 game.c \
			 game.h \
			 ai.c \
			 ai.h \
			 protocols.h


ggzd_ccheckers_LDADD = $(LDADD)

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

