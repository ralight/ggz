## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6636 2005-01-11 03:14:32Z jdorje $

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
	unitinfo.h

AM_CPPFLAGS = $(all_includes) $(LIBGGZ_INCLUDES) $(LIBGGZMOD_INCLUDES)

fyrdman_LDFLAGS = $(all_libraries) $(LIBGGZ_LDFLAGS) $(LIBGGZMOD_LDFLAGS)
fyrdman_LDADD = $(LIB_KDECORE) $(LIB_KDEUI) $(KNEWSTUFF) $(LIB_GGZ) $(LIB_GGZMOD)

AM_CXXFLAGS = -ggdb3

METASOURCES = AUTO

EXTRA_DIST = module.dsc.in

kde_conf_DATA = fyrdmanrc

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

