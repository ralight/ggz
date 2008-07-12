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

AM_CPPFLAGS = -I $(top_srcdir)/lib $(all_includes) $(LIBGGZ_INCLUDES)

ktictactux_client_LDFLAGS = $(LIBGGZ_LDFLAGS) $(all_libraries)
ktictactux_client_LDADD = $(top_builddir)/lib/kggzmod/libkggzmod.la $(top_builddir)/lib/libkggzgames.la $(LIB_KDECORE) $(LIB_KDEUI) $(KNEWSTUFF) $(LIB_GGZ)

METASOURCES = AUTO

pixmapdir = $(ggzdatadir)/ktictactux
dist_pixmap_DATA = bg.png

desktopdir = $(datadir)/applications
desktop_DATA = ktictactux.desktop

dist_kde_conf_DATA = ktictactuxrc

KDE_ICON = ktictactux

EXTRA_DIST = \
	hi16-app-ktictactux.png \
	hi32-app-ktictactux.png \
	hi48-app-ktictactux.png

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

