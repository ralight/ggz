## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6959 2005-03-04 00:18:42Z josef $

EXTRA_DIST = chess.dsc chess.room

SUBDIRS = libcgc

AM_CPPFLAGS = -Ilibcgc -I$(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES)

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

