SUBDIRS = kioslave .

kde_module_LTLIBRARIES = libkio_ggzmeta.la

libkio_ggzmeta_la_SOURCES = kio_ggzmeta.cpp helper.cpp

noinst_HEADERS = kio_ggzmeta.h helper.h

INCLUDES = -I $(qt_includes) -I $(kde_includes)
libkio_ggzmeta_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
libkio_ggzmeta_la_LIBADD = $(LIB_KIO)

AM_CXXFLAGS = @COMPILEOPTIONS@

METASOURCES = AUTO

kdelnk_DATA = ggzmeta.protocol
kdelnkdir = $(kde_servicesdir)


