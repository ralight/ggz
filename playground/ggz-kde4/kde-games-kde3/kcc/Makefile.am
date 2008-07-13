SUBDIRS = themes

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

AM_CPPFLAGS = -I $(top_srcdir)/lib $(all_includes) $(LIBGGZ_INCLUDES) $(LIBGGZMOD_INCLUDES)

kcc_LDFLAGS = $(all_libraries) $(LIBGGZ_LDFLAGS) $(LIBGGZMOD_LDFLAGS)
kcc_LDADD = $(top_builddir)/lib/libkggzgames.la $(LIB_KDECORE) $(LIB_KDEUI) $(KNEWSTUFF) $(LIB_GGZ) $(LIB_GGZMOD)

METASOURCES = AUTO

KDE_ICON = kcc

desktopdir = $(datadir)/applications
desktop_DATA = kcc.desktop

dist_kde_conf_DATA = kccrc

EXTRA_DIST = \
	hi16-app-kcc.png \
	hi32-app-kcc.png \
	hi48-app-kcc.png

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

