SUBDIRS = .

ggzexecmod_PROGRAMS = kcc

kcc_SOURCES = \
	main.cpp \
	kcc.cpp \
	kccwin.cpp \
	kccproto.cpp

noinst_HEADERS = \
	kcc.h \
	kccwin.h \
	kccproto.h

INCLUDES = $(all_includes) $(LIBGGZ_INCLUDES) $(LIBGGZMOD_INCLUDES)

kcc_LDFLAGS = $(all_libraries) $(LIBGGZ_LDFLAGS) $(LIBGGZMOD_LDFLAGS)
kcc_LDADD = $(LIB_KDECORE) $(LIB_KDEUI) $(KNEWSTUFF) $(LIB_GGZ) $(LIB_GGZMOD)

METASOURCES = AUTO

EXTRA_DIST = module.dsc.in

pixmapdir = @ggzdatadir@/kcc
pixmap_DATA = bg.png mask.png point0.png \
	point1.png point2.png point3.png point4.png point5.png point6.png

KDE_ICON = kcc

appsdir = $(kde_appsdir)/Games/ggz
apps_DATA = kcc.desktop

kde_conf_DATA = ktictactuxrc

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

