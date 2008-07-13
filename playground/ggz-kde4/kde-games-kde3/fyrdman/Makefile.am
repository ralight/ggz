SUBDIRS = data

ggzexecmod_PROGRAMS = fyrdman

fyrdman_SOURCES = \
	main.cpp \
	mainwindow.cpp \
	map.cpp \
	levelselector.cpp \
	network.cpp \
	level.cpp \
	unitinfo.cpp \
	fyrdman.skel

noinst_HEADERS = \
	mainwindow.h \
	map.h \
	levelselector.h \
	network.h \
	level.h \
	unitinfo.h \
	fyrdman.h

AM_CPPFLAGS = -I $(top_srcdir)/lib $(all_includes) $(LIBGGZ_INCLUDES) $(LIBGGZMOD_INCLUDES)

fyrdman_LDFLAGS = $(all_libraries) $(LIBGGZ_LDFLAGS) $(LIBGGZMOD_LDFLAGS)
fyrdman_LDADD = $(top_builddir)/lib/libkggzgames.la $(LIB_KDECORE) $(LIB_KDEUI) $(KNEWSTUFF) $(LIB_GGZ) $(LIB_GGZMOD)

METASOURCES = AUTO

dist_kde_conf_DATA = fyrdmanrc

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

