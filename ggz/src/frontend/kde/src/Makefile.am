SUBDIRS = abstract widgets .

ggzcorepp_includes = -I $(top_srcdir)/ggz-kde/ggzcore++/src
ggz_libraries = $(top_srcdir)/ggz-kde/ggzcore++/src/libggzcorepp.la

bin_PROGRAMS = kggz

kggz_SOURCES = \
	main.cpp \
	KGGZCommon.cpp \
	KGGZ.cpp \
	KGGZConnect.cpp \
	KGGZLaunch.cpp

noinst_HEADERS = \
	KGGZCommon.h \
	KGGZ.h \
	KGGZConnect.h \
	KGGZLaunch.h

INCLUDES = $(ggzcorepp_includes) -I widgets -I abstract $(all_includes) -I widgets/more

kggz_METASOURCES = AUTO

kggz_LDFLAGS = -ggdb3
LDADD = $(ggz_libraries) $(all_libraries) $(KDE_RPATH) $(LIB_KDEUI) -L widgets -lkggzui -L abstract -lkggzabstract @BROWSERLIB@

DISTCLEANFILES = $(kggz_METASOURCES)
