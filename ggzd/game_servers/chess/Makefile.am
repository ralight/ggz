## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6061 2004-06-27 19:34:15Z josef $

EXTRA_DIST = chess.dsc chess.room

SUBDIRS = libcgc

INCLUDES = -Ilibcgc -I$(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES)

ggzdexecmod_PROGRAMS = ggzd.chess
ggzdgame_DATA = chess.dsc
ggzdroom_DATA = chess.room

ggzd_chess_SOURCES = main.c \
	chess.h \
	game.h \
	game.c \
	ai.h \
	ai.c

ggzd_chess_LDADD = $(LDADD) libcgc/libcgc.a $(top_builddir)/ggzdmod/libggzdmod.la

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

