## Process this file with automake to produce Makefile.in

EXTRA_DIST = dots.dsc dots.room

SUBDIRS = .

ggzdexecmod_PROGRAMS = ggzd.dots
ggzdgame_DATA = dots.dsc
ggzdroom_DATA = dots.room

ggzd_dots_SOURCES = ggz.c \
		    ggz.h \
		    main.c \
		    game.c \
                    game.h \
		    ai.c \
		    ai.h \
		    protocols.h


ggzd_dots_LDADD = $(LDADD)

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

