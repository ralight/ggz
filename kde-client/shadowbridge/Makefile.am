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
LDFLAGS = $(all_libraries)
LDADD = $(top_builddir)/ggz-kde/ggzcore++/src/libggzcorepp.la $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_QT)
CXXFLAGS = @COMPILEOPTIONS@

