## Process this file with automake to produce Makefile.in

EXTRA_DIST = lapocha.dsc lapocha.room

SUBDIRS = .

ggzdexecmod_PROGRAMS = ggzd.lapocha
ggzdgame_DATA = lapocha.dsc
ggzdroom_DATA = lapocha.room

ggzd_lapocha_SOURCES = ggz.c ggz.h \
		       main.c \
		       game.c game.h \
		       cards.c cards.h \
		       protocols.h


ggzd_lapocha_LDADD = $(LDADD)

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

