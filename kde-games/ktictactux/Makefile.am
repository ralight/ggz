## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6948 2005-02-27 21:54:15Z josef $

SUBDIRS = themes

ggzexecmod_PROGRAMS = ktictactux_client

ktictactux_client_SOURCES = \
	main.cpp \
	ktictactux.cpp \
	ktictactuxwin.cpp \
	ktictactuxproto.cpp \
	qwhiteframe.cpp

noinst_HEADERS = \
	ktictactux.h \
	ktictactuxwin.h \
	ktictactuxproto.h \
	qwhiteframe.h

AM_CPPFLAGS = $(all_includes) $(LIBGGZ_INCLUDES) $(LIBGGZMOD_INCLUDES)

ktictactux_client_LDFLAGS = $(LIBGGZ_LDFLAGS) $(LIBGGZMOD_LDFLAGS) $(all_libraries)
ktictactux_client_LDADD = $(LIB_KDECORE) $(LIB_KDEUI) $(KNEWSTUFF) $(LIB_GGZ) $(LIB_GGZMOD)

METASOURCES = AUTO

pixmapdir = @ggzdatadir@/ktictactux
pixmap_DATA = bg.png

KDE_ICON = ktictactux

desktopdir = $(datadir)/applications
desktop_DATA = ktictactux.desktop

kde_conf_DATA = ktictactuxrc

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

