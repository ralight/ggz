SUBDIRS = abstract widgets .

ggzcorepp_includes = -I $(top_srcdir)/ggz-kde/ggzcore++/src
ggz_libraries = $(top_srcdir)/ggz-kde/ggzcore++/src/libggzcorepp.la $(GGZCORE_LDFLAGS) $(LIB_GGZCORE) $(LIB_GGZ)

bin_PROGRAMS = kggz

kggz_SOURCES = \
	main.cpp \
	KGGZCommon.cpp \
	KGGZ.cpp \
	KGGZConnect.cpp \
	KGGZLaunch.cpp \
	KGGZInput.cpp \
	KGGZMeta.cpp

noinst_HEADERS = \
	KGGZCommon.h \
	KGGZ.h \
	KGGZConnect.h \
	KGGZLaunch.h \
	KGGZInput.h \
	KGGZMeta.h

INCLUDES = $(ggzcorepp_includes) -I widgets -I abstract $(all_includes) -I widgets/more

kggz_METASOURCES = AUTO

CXXFLAGS = @COMPILEOPTIONS@ $(GGZCORE_INCLUDES)

kggz_LDFLAGS = -ggdb3
LDADD = $(ggz_libraries) $(all_libraries) $(KDE_RPATH) $(LIB_KDEUI) widgets/libkggzui.a abstract/libkggzabstract.a @BROWSERLIB@ -lqt

DISTCLEANFILES = $(kggz_METASOURCES)

