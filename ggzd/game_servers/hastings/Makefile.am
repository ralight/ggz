## Process this file with automake to produce Makefile.in

EXTRA_DIST = hastings.dsc hastings.room

SUBDIRS = .

ggzdexecmod_PROGRAMS = ggzd.hastings
ggzdgame_DATA = hastings.dsc
ggzdroom_DATA = hastings.room

ggzd_hastings_SOURCES = ggz.c \
		         ggz.h \
		         main.c \
		         game.c \
                         game.h \
		         protocols.h


ggzd_hastings_LDADD = $(LDADD)

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

