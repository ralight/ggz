## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 2769 2001-12-01 06:53:01Z bmh $

EXTRA_DIST = chess.dsc chess.room

SUBDIRS = libcgc .

INCLUDES = -Ilibcgc 

ggzdexecmod_PROGRAMS = ggzd.chess
ggzdgame_DATA = chess.dsc
ggzdroom_DATA = chess.room


ggzd_chess_SOURCES = main.c \
		     chess.h \
		     game.h \
		     game.c

INCLUDES = -I$(top_srcdir)/ggzdmod
ggzd_chess_DEPENDENCIES = $(top_builddir)/ggzdmod/libggzdmod.a

ggzd_chess_LDADD = $(LDADD) libcgc/libcgc.a $(top_builddir)/ggzdmod/libggzdmod.a

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

