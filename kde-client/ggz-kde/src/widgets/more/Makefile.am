## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6636 2005-01-11 03:14:32Z jdorje $

SUBDIRS = .

noinst_LIBRARIES = libkggzuimore.a

libkggzuimore_a_SOURCES = KGGZBrowser.cpp

noinst_HEADERS = KGGZBrowser.h

AM_CPPFLAGS = $(all_includes) -I $(top_srcdir)/ggz-kde/src

libkggzuimore_a_METASOURCES = AUTO

AM_CXXFLAGS = @COMPILEOPTIONS@ $(GGZCORE_INCLUDES)

DISTCLEANFILES = $(libkggzuimore_a_METASOURCES)

