## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6959 2005-03-04 00:18:42Z josef $

EXTRA_DIST = hastings.dsc hastings.room

SUBDIRS = maps

ggzdexecmod_PROGRAMS = ggzd.hastings
ggzdgame_DATA = hastings.dsc
ggzdroom_DATA = hastings.room

ggzd_hastings_SOURCES =  main.c \
		         game.c \
                         game.h

AM_CPPFLAGS = -I$(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES)

ggzd_hastings_LDADD = $(LDADD) $(top_builddir)/ggzdmod/libggzdmod.la

