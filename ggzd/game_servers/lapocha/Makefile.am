## Process this file with automake to produce Makefile.in

EXTRA_DIST = lapocha.dsc lapocha.room

SUBDIRS = .

ggzdexecmod_PROGRAMS = ggzd.lapocha
ggzdgame_DATA = lapocha.dsc
ggzdroom_DATA = lapocha.room

ggzd_lapocha_SOURCES = main.c \
		       game.c game.h \
		       cards.c cards.h

INCLUDES = -I$(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES)
ggzd_lapocha_DEPENDENCIES = $(top_builddir)/ggzdmod/libggzdmod.a

ggzd_lapocha_LDADD = $(LDADD) $(top_builddir)/ggzdmod/libggzdmod.a

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

