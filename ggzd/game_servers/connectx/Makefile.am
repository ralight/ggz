## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6258 2004-11-04 23:00:59Z jdorje $

EXTRA_DIST = connectx.dsc connectx.room

SUBDIRS = .

ggzdexecmod_PROGRAMS = ggzd.connectx
ggzdgame_DATA = connectx.dsc
ggzdroom_DATA = connectx.room

ggzd_connectx_SOURCES = \
	main.c \
	game.c \
	game.h

AM_CPPFLAGS = -I$(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES)

ggzd_connectx_LDADD = $(LDADD) $(top_builddir)/ggzdmod/libggzdmod.la

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

