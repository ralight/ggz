SUBDIRS = kioslave .

lib_LTLIBRARIES = libkio_ggz.la

libkio_ggz_la_SOURCES = kio_ggz.cpp helper.cpp

noinst_HEADERS = kio_ggz.h helper.h

INCLUDES = -I $(qt_includes) -I $(kde_includes) -I $(top_srcdir)/ggz-kde/ggzcore++/src
libkio_ggz_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
libkio_ggz_la_LIBADD = $(LIB_KIO) $(top_srcdir)/ggz-kde/ggzcore++/src/libggzcorepp.la

METASOURCES = AUTO

kdelnk_DATA = ggz.protocol
kdelnkdir = $(kde_servicesdir)


