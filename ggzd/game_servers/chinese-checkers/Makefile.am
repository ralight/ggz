## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 5458 2003-02-26 13:17:22Z dr_maux $

EXTRA_DIST = ccheckers.dsc ccheckers.room

SUBDIRS = .

ggzdexecmod_PROGRAMS = ggzd.ccheckers
ggzdgame_DATA = ccheckers.dsc
ggzdroom_DATA = ccheckers.room

ggzd_ccheckers_SOURCES = main.c \
			 game.c \
			 game.h \
			 ai.c \
			 ai.h


INCLUDES = -I$(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES)
ggzd_ccheckers_LDADD = $(LDADD) $(top_builddir)/ggzdmod/libggzdmod.la -lm

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

