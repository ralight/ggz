SUBDIRS = .

noinst_LIBRARIES = libggzgameserver.a
libggzgameserver_a_SOURCES = ggzgameserver.cpp net.cpp
noinst_HEADERS = ggzgameserver.h net.h

AM_CPPFLAGS = -I $(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES)

test: # avoid warning
