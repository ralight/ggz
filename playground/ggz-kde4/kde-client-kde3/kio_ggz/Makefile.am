SUBDIRS = kioslave

kde_module_LTLIBRARIES = libkio_ggz.la

libkio_ggz_la_SOURCES = kio_ggz.cpp helper.cpp
libkio_ggz_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
libkio_ggz_la_LIBADD = $(top_builddir)/ggz-kde/ggzcore++/src/libggzcorepp.la $(LIB_KIO)

AM_CXXFLAGS = @COMPILEOPTIONS@

noinst_HEADERS = kio_ggz.h helper.h

AM_CPPFLAGS = $(GGZCORE_INCLUDES) -I $(top_srcdir)/ggz-kde/ggzcore++/src -I $(qt_includes) -I $(kde_includes)

METASOURCES = AUTO

dist_kde_services_DATA = ggz.protocol

