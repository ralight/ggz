SUBDIRS = .

lib_LTLIBRARIES = libggzdmodpp.la
libggzdmodpp_la_SOURCES = ggzgameserver.cpp net.cpp mnet.cpp
libggzdmodpp_la_LDFLAGS = $(LIBGGZ_LDFLAGS) -version-info 4:0:0
libggzdmodpp_la_LIBADD = $(top_builddir)/ggzdmod/libggzdmod.la $(LIB_GGZ)

ggzdmodppdir = $(includedir)/ggzdmod++
ggzdmodpp_HEADERS = ggzgameserver.h net.h mnet.h

AM_CPPFLAGS = -I $(srcdir) -I $(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES)

EXTRA_DIST = README.ggzdmod++

