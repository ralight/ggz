SUBDIRS = .

#bindir = $(ggzexecmoddir)

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

INCLUDES = $(all_includes) $(LIBGGZ_INCLUDES)

#UI = -L$(qt_libraries) -L$(kde_libraries) -lqt -lkdeui -lkdecore
#ktictactux_client_LDFLAGS = -rpath
ktictactux_client_LDFLAGS = $(all_libraries) $(LIBGGZ_LDFLAGS)
ktictactux_client_LDADD = $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_GGZ)

METASOURCES = AUTO

EXTRA_DIST = module.dsc.in

pixmapdir = @ggzdatadir@/ktictactux
pixmap_DATA = tux.png merlin.png kde.png gnome.png cross.png circle.png bg.png

KDE_ICON = ktictactux

appsdir = $(kde_appsdir)/Games/ggz
apps_DATA = ktictactux.desktop

install-data-local:
	@$(NORMAL_INSTALL)
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	@$(NORMAL_UNINSTALL)
	-$(GGZ_CONFIG) --remove --modfile=module.dsc

