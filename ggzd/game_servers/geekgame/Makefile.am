SUBDIRS = .

ggzdgame_DATA = geekgame.dsc
ggzdroom_DATA = geekgame.room

EXTRA_DIST = $(ggzdgame_DATA) $(ggzdroom_DATA)

ggzdexecmod_PROGRAMS = geekgameserver
geekgameserver_SOURCES = main.cpp geekgame.cpp
geekgameserver_LDADD = $(LDADD) \
	$(top_builddir)/ggzdmod++/libggzdmodpp.la

noinst_HEADERS = geekgame.h protocol.h

AM_CPPFLAGS = -I$(top_srcdir)/ggzdmod++ \
	-I$(top_srcdir)/ggzdmod \
	$(LIBGGZ_INCLUDES)

