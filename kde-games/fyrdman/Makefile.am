SUBDIRS = data

ggzexecmod_PROGRAMS = fyrdman

fyrdman_SOURCES = \
	main.cpp \
	mainwindow.cpp \
	map.cpp \
	levelselector.cpp \
	network.cpp \
	level.cpp

noinst_HEADERS = \
	mainwindow.h \
	map.h \
	levelselector.h \
	network.h \
	level.h

INCLUDES = $(all_includes) $(LIBGGZ_INCLUDES) $(LIBGGZMOD_INCLUDES)

fyrdman_LDFLAGS = $(all_libraries) $(LIBGGZ_LDFLAGS) $(LIBGGZMOD_LDFLAGS)
fyrdman_LDADD = $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_GGZ) $(LIB_GGZMOD)

AM_CXXFLAGS = -ggdb3

METASOURCES = AUTO

EXTRA_DIST = module.dsc.in

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

