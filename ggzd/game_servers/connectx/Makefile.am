## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 5300 2002-12-18 17:40:43Z dr_maux $

EXTRA_DIST = connectx.dsc connectx.room

SUBDIRS = .

ggzdexecmod_PROGRAMS = ggzd.connectx
ggzdgame_DATA = connectx.dsc
ggzdroom_DATA = connectx.room

ggzd_connectx_SOURCES = \
	main.c \
	game.c \
	game.h

INCLUDES = -I$(top_srcdir)/ggzdmod

ggzd_connectx_LDADD = $(LDADD) $(top_builddir)/ggzdmod/libggzdmod.la

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

