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

INCLUDES = $(ggzcorepp_includes) -I $(top_srcdir)/ggz-kde/src

