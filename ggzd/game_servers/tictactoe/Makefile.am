## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 7119 2005-04-21 18:00:51Z josef $

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
	net.h

AM_CPPFLAGS = -I$(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES)
ggzd_tictactoe_LDADD = $(LDADD) $(top_builddir)/ggzdmod/libggzdmod.la

