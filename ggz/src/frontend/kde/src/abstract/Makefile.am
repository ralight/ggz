SUBDIRS = .

ggzcorepp_includes = -I $(top_srcdir)/ggz-kde/ggzcore++/src
ggz_libraries = $(top_srcdir)/ggz-kde/ggzcore++/src/libggzcorepp.la

noinst_LIBRARIES = libkggzabstract.a

libkggzabstract_a_SOURCES = \
	KGGZRoom.cpp \
	KGGZServer.cpp \
	KGGZCallback.cpp

noinst_HEADERS = \
	KGGZRoom.h \
	KGGZServer.h \
	KGGZCallback.h

INCLUDES = $(ggzcorepp_includes) -I $(top_srcdir)/ggz-kde/src
