SUBDIRS = data

ggzexecmod_PROGRAMS = fyrdman

fyrdman_SOURCES = \
	main.cpp \
	mainwindow.cpp \
	map.cpp

noinst_HEADERS = \
	mainwindow.h \
	map.h

INCLUDES = $(all_includes) $(LIBGGZ_INCLUDES) $(LIBGGZMOD_INCLUDES)

fyrdman_LDFLAGS = $(all_libraries) $(LIBGGZ_LDFLAGS) $(LIBGGZMOD_LDFLAGS)
fyrdman_LDADD = $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_GGZ) $(LIB_GGZMOD)

METASOURCES = AUTO

#EXTRA_DIST = module.dsc.in

#KDE_ICON = fyrdman

#install-data-local:
#	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

#uninstall-local:
#	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

