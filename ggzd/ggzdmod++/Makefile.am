SUBDIRS = .

lib_LTLIBRARIES = libggzdmodpp.la
libggzdmodpp_la_SOURCES = ggzgameserver.cpp net.cpp mnet.cpp

ggzdmodppdir = $(includedir)/ggzdmod++
ggzdmodpp_HEADERS = ggzgameserver.h net.h mnet.h

AM_CPPFLAGS = -I $(srcdir) -I $(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES)

EXTRA_DIST = README.ggzdmod++

