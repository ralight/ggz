## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6959 2005-03-04 00:18:42Z josef $

EXTRA_DIST = ccheckers.dsc ccheckers.room

SUBDIRS = .

ggzdexecmod_PROGRAMS = ggzd.ccheckers
ggzdgame_DATA = ccheckers.dsc
ggzdroom_DATA = ccheckers.room

ggzd_ccheckers_SOURCES = main.c \
			 game.c \
			 game.h \
			 ai.c \
			 ai.h


AM_CPPFLAGS = -I$(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES)
ggzd_ccheckers_LDADD = $(LDADD) $(top_builddir)/ggzdmod/libggzdmod.la -lm

