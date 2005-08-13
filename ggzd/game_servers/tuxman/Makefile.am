SUBDIRS = .

ggzdgame_DATA = tuxman.dsc
ggzdroom_DATA = tuxman.room

EXTRA_DIST = $(ggzdgame_DATA) $(ggzdroom_DATA)

ggzdexecmod_PROGRAMS = tuxmanserv
tuxmanserv_SOURCES = main.cpp map.cpp pacman.cpp tuxmanserv.cpp
tuxmanserv_LDADD = $(LDADD) \
			$(top_builddir)/ggzdmod++/libggzdmodpp.la

noinst_HEADERS = map.h pacman.h tuxmanserv.h proto.h

mapdir = $(ggzddatadir)/tuxmanserv
dist_map_DATA = level9.pac

AM_CPPFLAGS = -I$(top_srcdir)/ggzdmod++ \
	   -I$(top_srcdir)/ggzdmod \
	   $(LIBGGZ_INCLUDES)

