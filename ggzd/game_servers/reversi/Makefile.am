## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6959 2005-03-04 00:18:42Z josef $

EXTRA_DIST = reversi.dsc reversi.room

SUBDIRS = .

ggzdexecmod_PROGRAMS = ggzd.reversi
ggzdgame_DATA = reversi.dsc
ggzdroom_DATA = reversi.room


ggzd_reversi_SOURCES = 	main.c \
			game.c \
			game.h


AM_CPPFLAGS = -I$(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES)
ggzd_reversi_LDADD = $(LDADD) $(top_builddir)/ggzdmod/libggzdmod.la

