## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6636 2005-01-11 03:14:32Z jdorje $

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
	KGGZChatLine.cpp \
	KGGZGrubby.cpp \
	KGGZPrefEnv.cpp \
	KGGZSelector.cpp \
	KGGZLineSeparator.cpp \
	KGGZCaption.cpp \
	KGGZGameInfoDialog.cpp \
	KGGZTeam.cpp

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
	KGGZChatLine.h \
	KGGZGrubby.h \
	KGGZPrefEnv.h \
	KGGZSelector.h \
	KGGZLineSeparator.h \
	KGGZCaption.h \
	KGGZGameInfoDialog.h \
	KGGZTeam.h

EXTRA_HEADERS = 

AM_CXXFLAGS = @COMPILEOPTIONS@ $(GGZCORE_INCLUDES)

AM_CPPFLAGS = $(ggzcorepp_includes) $(all_includes) -I $(top_srcdir)/ggz-kde/src -I $(top_srcdir)/ggz-kde/src/abstract

libkggzui_a_METASOURCES = AUTO

DISTCLEANFILES = $(libkggzui_a_METASOURCES)

messages:
	$(XGETTEXT) $(libkggzui_a_SOURCES) -C -ki18n -kI18N_NOOP -o $(top_srcdir)/ggz-kde/po/kggz.pot

