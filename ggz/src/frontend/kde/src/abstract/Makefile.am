SUBDIRS = .

ggzcorepp_includes = -I $(top_srcdir)/ggz-kde/ggzcore++/src
ggz_libraries = $(top_srcdir)/ggz-kde/ggzcore++/src/libggzcorepp.la

noinst_LIBRARIES = libkggzabstract.a

libkggzabstract_a_SOURCES = \
	KGGZCallback.cpp

noinst_HEADERS = \
	KGGZCallback.h

CXXFLAGS = @COMPILEOPTIONS@

INCLUDES = $(ggzcorepp_includes) -I $(top_srcdir)/ggz-kde/src

