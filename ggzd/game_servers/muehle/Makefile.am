SUBDIRS = qtserv ggz variants

ggzdgame_DATA = muehle.dsc
ggzdroom_DATA = muehle.room

ggzdexecmod_PROGRAMS = muehleserv
muehleserv_SOURCES = muehleserver.cpp loader.cpp main.cpp
muehleserv_LDADD = ggz/libggzgameserver.a qtserv/libqtserv.a $(top_srcdir)/ggzdmod/libggzdmod.la
noinst_HEADERS = muehleserver.h
INCLUDES = -I ggz -I qtserv

