## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 4741 2002-09-26 20:56:08Z dr_maux $

EXTRA_DIST = hastings.dsc hastings.room

SUBDIRS = .

ggzdexecmod_PROGRAMS = ggzd.hastings
ggzdgame_DATA = hastings.dsc
ggzdroom_DATA = hastings.room

ggzd_hastings_SOURCES =  main.c \
		         game.c \
                         game.h

INCLUDES = -I$(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES)

ggzd_hastings_LDADD = $(LDADD) $(top_builddir)/ggzdmod/libggzdmod.la

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

