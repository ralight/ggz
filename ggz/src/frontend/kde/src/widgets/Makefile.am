SUBDIRS = .

noinst_LIBRARIES = libkggzui.a

libkggzui_a_SOURCES = \
	KGGZBase.cpp \
	KGGZChat.cpp \
	KGGZUsers.cpp \
	KGGZTables.cpp \
	KGGZWorkspace.cpp \
	KGGZAbout.cpp \
	KGGZSplash.cpp

noinst_HEADERS = \
	KGGZBase.h \
	KGGZChat.h \
	KGGZUsers.h \
	KGGZTables.h \
	KGGZWorkspace.h \
	KGGZWidgetCommon.h \
	KGGZAbout.h \
	KGGZSplash.h

INCLUDES = $(all_includes) -I $(top_srcdir)/ggz-kde/src -I $(top_srcdir)/ggz-kde/src/abstract

libkggzui_a_METASOURCES = AUTO

DISTCLEANFILES = $(libkggzui_a_METASOURCES)
