## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6678 2005-01-14 22:46:32Z josef $

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

AM_CPPFLAGS = -I $(top_srcdir)/ggz-kde/ggzcore++/src $(LIBGGZ_INCLUDES) $(all_includes)
AM_LDFLAGS = $(all_libraries)
LDADD = $(top_builddir)/ggz-kde/ggzcore++/src/libggzcorepp.la $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_QT)
AM_CXXFLAGS = @COMPILEOPTIONS@

KDE_ICON = shadowbridge

appsdir = $(kde_appsdir)/.hidden
apps_DATA = shadowbridge.desktop

