## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6959 2005-03-04 00:18:42Z josef $

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

