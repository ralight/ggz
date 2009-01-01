dist_ggzdgame_DATA = geekgame.dsc
dist_ggzdroom_DATA = geekgame.room

ggzdexecmod_PROGRAMS = geekgameserver
geekgameserver_SOURCES = main.cpp geekgame.cpp
geekgameserver_LDADD = $(LDADD) \
	$(top_builddir)/ggzdmod++/libggzdmod++.la

noinst_HEADERS = geekgame.h protocol.h

AM_CPPFLAGS = -I$(top_srcdir)/ggzdmod++ \
	-I$(top_srcdir)/ggzdmod \
	$(LIBGGZ_INCLUDES)

