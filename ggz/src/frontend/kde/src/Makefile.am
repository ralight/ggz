SUBDIRS = abstract widgets .

ggzcorepp_includes = -I $(top_srcdir)/ggz-kde/ggzcore++/src
ggz_libraries = $(top_srcdir)/ggz-kde/ggzcore++/src/libggzcorepp.la $(GGZCORE_LDFLAGS) $(LIB_GGZCORE)

bin_PROGRAMS = kggz

kggz_SOURCES = \
	main.cpp \
	KGGZCommon.cpp \
	KGGZ.cpp \
	KGGZConnect.cpp \
	KGGZLaunch.cpp \
	KGGZInput.cpp

noinst_HEADERS = \
	KGGZCommon.h \
	KGGZ.h \
	KGGZConnect.h \
	KGGZLaunch.h \
	KGGZInput.h

INCLUDES = $(ggzcorepp_includes) -I widgets -I abstract $(all_includes) -I widgets/more

kggz_METASOURCES = AUTO

CXXFLAGS = @COMPILEOPTIONS@ $(EASYSOCK_INCLUDES) $(GGZCORE_INCLUDES)

kggz_LDFLAGS = -ggdb3
LDADD = $(ggz_libraries) $(all_libraries) $(KDE_RPATH) $(LIB_KDEUI) -L widgets -lkggzui -L abstract -lkggzabstract @BROWSERLIB@ -lqt

DISTCLEANFILES = $(kggz_METASOURCES)

#POFILES = $(top_srcdir)/ggz-kde/po/kggz.pot
#KDE_LANG = de pt_BR

messages:
	$(XGETTEXT) $(kggz_SOURCES) -C -ki18n -kI18N_NOOP -s -j -o $(top_srcdir)/ggz-kde/po/kggz.pot
	
