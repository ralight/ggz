SUBDIRS = pics

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

AM_CPPFLAGS = -I $(top_srcdir)/ggz-kde/ggzcore++/src $(LIBGGZ_INCLUDES) $(all_includes)
AM_LDFLAGS = $(all_libraries)
LDADD = $(top_builddir)/ggz-kde/ggzcore++/src/libggzcorepp.la $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_QT)
AM_CXXFLAGS = @COMPILEOPTIONS@

desktopdir = $(datadir)/applications
dist_desktop_DATA = shadowbridge.desktop

KDE_ICON = shadowbridge

EXTRA_DIST = \
	hi16-app-shadowbridge.png \
	hi32-app-shadowbridge.png \
	hi48-app-shadowbridge.png

