## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 8279 2006-06-27 07:29:39Z josef $

SUBDIRS = qtserv variants

ggzdgame_DATA = muehle.dsc
ggzdroom_DATA = muehle.room

EXTRA_DIST = $(ggzdgame_DATA) $(ggzdroom_DATA)

ggzdexecmod_PROGRAMS = muehleserv
muehleserv_SOURCES = muehleserver.cpp loader.cpp main.cpp net.cpp
muehleserv_LDADD = $(LDADD) \
	qtserv/libqtserv.a \
	$(top_builddir)/ggzdmod++/libggzdmod++.la

noinst_HEADERS = muehleserver.h loader.h net.h

AM_CPPFLAGS = -I$(top_srcdir)/game_servers/muehle/qtserv \
	-I$(top_srcdir)/ggzdmod++ \
	-I$(top_srcdir)/ggzdmod \
	$(LIBGGZ_INCLUDES)

