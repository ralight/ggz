## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 7070 2005-03-30 13:44:17Z josef $

SUBDIRS = .

ggzcorepp_includes = -I $(top_srcdir)/ggz-kde/ggzcore++/src
ggz_libraries = $(top_srcdir)/ggz-kde/ggzcore++/src/libggzcorepp.la

noinst_LIBRARIES = libkggzabstract.a

libkggzabstract_a_SOURCES = \
	KGGZCallback.cpp \
	KGGZGameInfo.cpp

noinst_HEADERS = \
	KGGZCallback.h \
	KGGZGameInfo.h

AM_CXXFLAGS = @COMPILEOPTIONS@ $(GGZCORE_INCLUDES)

AM_CPPFLAGS = $(ggzcorepp_includes) -I $(top_srcdir)/ggz-kde/src -I $(all_includes)

