SUBDIRS = abstract widgets .

ggzcorepp_includes = -I $(top_srcdir)/ggz-kde/ggzcore++/src
ggz_libraries = $(top_builddir)/ggz-kde/ggzcore++/src/libggzcorepp.la $(LIB_GGZCORE) $(LIB_GGZ)

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

AM_CPPFLAGS = -I $(top_srcdir)/ggz-kde/src/abstract \
	-I $(top_srcdir)/ggz-kde/src/widgets \
	-I $(top_srcdir)/ggz-kde/src/widgets/more \
	$(ggzcorepp_includes) $(all_includes)

kggz_METASOURCES = AUTO

AM_CXXFLAGS = @COMPILEOPTIONS@ $(GGZCORE_INCLUDES)

kggz_LDFLAGS = $(GGZCORE_LDFLAGS) $(LIBGGZ_LDFLFAGS)
kggz_LDADD = $(top_builddir)/ggz-kde/src/widgets/libkggzui.a \
	$(top_builddir)/ggz-kde/src/abstract/libkggzabstract.a \
	$(all_libraries) $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_KDNSSD) $(LIB_QT) \
	$(ggz_libraries)

if COMPILE_BROWSER
kggz_LDADD += $(top_builddir)/ggz-kde/src/widgets/more/libkggzuimore.a $(LIB_KHTML)
endif

DISTCLEANFILES = $(kggz_METASOURCES)

