SUBDIRS = .

noinst_LIBRARIES = libkggzuimore.a

libkggzuimore_a_SOURCES = KGGZBrowser.cpp

noinst_HEADERS = KGGZBrowser.h

INCLUDES = $(all_includes) -I $(top_srcdir)/ggz-kde/src

libkggzuimore_a_METASOURCES = AUTO

AM_CXXFLAGS = @COMPILEOPTIONS@ $(GGZCORE_INCLUDES)

DISTCLEANFILES = $(libkggzuimore_a_METASOURCES)

