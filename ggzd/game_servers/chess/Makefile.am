## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 2214 2001-08-24 02:54:33Z jdorje $

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

ggzd_chess_DEPENDENCIES = ../libggzmod/libggzdmod.a

ggzd_chess_LDADD = $(LDADD) libcgc/libcgc.a ../libggzmod/libggzdmod.a

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

