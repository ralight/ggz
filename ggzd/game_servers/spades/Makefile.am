## Process this file with automake to produce Makefile.in

EXTRA_DIST = spades.dsc spades.room

SUBDIRS = .

ggzdexecmod_PROGRAMS = ggzd.spades
ggzdgame_DATA = spades.dsc
ggzdroom_DATA = spades.room

ggzd_spades_SOURCES = card.c \
		      card.h \
                      engine_main.c \
		      engine_func.c \
		      engine.h \
		      err_func.c \
		      err_func.h \
		      ai.c \
		      options.h \
		      protocols.h \
		      socketfunc.c \
		      socketfunc.h

ggzd_spades_LDADD = $(LDADD) $(top_builddir)/ggzdmod/libggzdmod.la

AM_CPPFLAGS = $(LIBGGZ_INCLUDES)

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

