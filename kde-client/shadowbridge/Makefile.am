SUBDIRS = pics .

bin_PROGRAMS = shadowbridge

shadowbridge_SOURCES = \
	main.cpp \
	shadowapp.cpp \
	shadowcontainer.cpp \
	shadowclient.cpp \
	shadowclientggz.cpp \
	shadownet.cpp

noinst_HEADERS = \
	shadowapp.h \
	shadowcontainer.h \
	shadowclient.h \
	shadowclientggz.h \
	shadownet.h

METASOURCES = AUTO

INCLUDES = -I $(top_srcdir)/ggz-kde/ggzcore++/src $(all_includes)
LDFLAGS = $(all_libraries) $(LIB_KDEUI) -lqt
LDADD = $(top_srcdir)/ggz-kde/ggzcore++/src/libggzcorepp.la
CXXFLAGS = -ggdb3

