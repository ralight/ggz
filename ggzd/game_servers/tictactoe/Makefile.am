## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 7658 2005-12-12 11:47:24Z josef $

EXTRA_DIST = tictactoe.dsc tictactoe.room

SUBDIRS = .

ggzdexecmod_PROGRAMS = ggzd.tictactoe
ggzdgame_DATA = tictactoe.dsc
ggzdroom_DATA = tictactoe.room

ggzd_tictactoe_SOURCES = \
	main.c \
	game.c \
	game.h \
	net.c \
	net.h \
	ttt-ai.c \
	ttt-ai.h

AM_CPPFLAGS = -I$(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES)
ggzd_tictactoe_LDADD = $(LDADD) $(top_builddir)/ggzdmod/libggzdmod.la

