SUBDIRS = qtserv variants

dist_ggzdgame_DATA = muehle.dsc
dist_ggzdroom_DATA = muehle.room

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

