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

INCLUDES = $(all_includes) $(LIBGGZ_INCLUDES) $(LIBGGZMOD_INCLUDES)

ktictactux_client_LDFLAGS = $(LIBGGZ_LDFLAGS) $(LIBGGZMOD_LDFLAGS) $(all_libraries)
ktictactux_client_LDADD = $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_GGZ) $(LIB_GGZMOD)

METASOURCES = AUTO

EXTRA_DIST = module.dsc.in

pixmapdir = @ggzdatadir@/ktictactux
pixmap_DATA = bg.png

KDE_ICON = ktictactux

appsdir = $(kde_appsdir)/Games/ggz
apps_DATA = ktictactux.desktop

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

