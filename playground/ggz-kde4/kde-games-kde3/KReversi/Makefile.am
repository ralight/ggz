SUBDIRS = pixmaps

ggzexecmod_PROGRAMS = ggz.kreversi

ggz_kreversi_LDFLAGS = $(GGZMOD_LDFLAGS) $(all_libraries)
ggz_kreversi_LDADD = $(LIB_GGZMOD) $(LIB_QT) $(LIB_KDECORE) $(LIB_KDEUI)

AM_CPPFLAGS = $(GGZMOD_INCLUDES) $(all_includes)

ggz_kreversi_METASOURCES = AUTO

ggz_kreversi_SOURCES = \
	main.cpp\
	reversiprotocol.cpp\
	reversiview.cpp\
	kreversi.cpp \
	network.cpp

EXTRA_DIST = \
	THEME.HOWTO

noinst_HEADERS = \
	reversiprotocol.h\
	reversiview.h\
	kreversi.h \
	network.h

rcdir = $(kde_datadir)/ggz.kreversi
dist_rc_DATA = ggz.kreversiui.rc

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

