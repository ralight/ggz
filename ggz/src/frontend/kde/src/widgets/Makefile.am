SUBDIRS = @BROWSERSUBDIR@ .

ggzcorepp_includes = -I $(top_srcdir)/ggz-kde/ggzcore++/src
#ggz_libraries = $(top_srcdir)/ggz-kde/ggzcore++/src/libggzcorepp.la

noinst_LIBRARIES = libkggzui.a

libkggzui_a_SOURCES = \
	KGGZBase.cpp \
	KGGZChat.cpp \
	KGGZUsers.cpp \
	KGGZTables.cpp \
	KGGZWorkspace.cpp \
	KGGZAbout.cpp \
	KGGZSplash.cpp \
	KGGZLogo.cpp \
	KGGZMotd.cpp \
	KGGZSplashScreen.cpp \
	KGGZChatLine.cpp \
	KGGZGrubby.cpp \
	KGGZPrefEnv.cpp \
	KGGZSelector.cpp

noinst_HEADERS = \
	KGGZBase.h \
	KGGZChat.h \
	KGGZUsers.h \
	KGGZTables.h \
	KGGZWorkspace.h \
	KGGZAbout.h \
	KGGZSplash.h \
	KGGZLogo.h \
	KGGZMotd.h \
	KGGZSplashScreen.h \
	KGGZChatLine.h \
	KGGZGrubby.h \
	KGGZPrefEnv.h \
	KGGZSelector.h

EXTRA_HEADERS = 

CXXFLAGS = @COMPILEOPTIONS@

INCLUDES = $(ggzcorepp_includes) $(all_includes) -I $(top_srcdir)/ggz-kde/src -I $(top_srcdir)/ggz-kde/src/abstract

libkggzui_a_METASOURCES = AUTO

DISTCLEANFILES = $(libkggzui_a_METASOURCES)

messages:
	$(XGETTEXT) $(libkggzui_a_SOURCES) -C -ki18n -kI18N_NOOP -o $(top_srcdir)/ggz-kde/po/kggz.pot

