noinst_LIBRARIES = libkggzui.a

libkggzui_a_SOURCES = \
	KGGZBase.cpp \
	KGGZChat.cpp \
	KGGZUsers.cpp \
	KGGZTables.cpp \
	KGGZWorkspace.cpp \
	KGGZAbout.cpp

noinst_HEADERS = \
	KGGZBase.h \
	KGGZChat.h \
	KGGZUsers.h \
	KGGZTables.h \
	KGGZWorkspace.h \
	KGGZWidgetCommon.h \
	KGGZAbout.h

INCLUDES = $(all_includes) -I ..
#libkggzui_a_LDFLAGS = $(KDE_RPATH)

libkggzui_a_METASOURCES = AUTO

DISTCLEANFILES = $(libkggzui_a_METASOURCES)
