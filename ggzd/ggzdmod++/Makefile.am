lib_LTLIBRARIES = libggzdmod++.la
libggzdmod___la_SOURCES = ggzgameserver.cpp net.cpp mnet.cpp
libggzdmod___la_LDFLAGS = $(LIBGGZ_LDFLAGS) -version-info 1:0:0
libggzdmod___la_LIBADD = $(top_builddir)/ggzdmod/libggzdmod.la $(LIB_GGZ)
libggzdmod___la_CPPFLAGS = -I $(srcdir) -I $(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES)

ggzdmodppdir = $(includedir)/ggzdmod++
ggzdmodpp_HEADERS = ggzgameserver.h net.h mnet.h

EXTRA_DIST = README.ggzdmod++

