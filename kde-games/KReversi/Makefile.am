SUBDIRS = pixmaps

ggzexecmod_PROGRAMS = ggz.kreversi

ggz_kreversi_LDFLAGS = $(GGZMOD_LDFLAGS) $(all_libraries)
ggz_kreversi_LDADD = $(LIB_GGZMOD) $(LIB_QT) $(LIB_KDECORE) $(LIB_KDEUI)

INCLUDES = $(GGZMOD_INCLUDES) $(all_includes)

ggz_kreversi_METASOURCES = AUTO

ggz_kreversi_SOURCES = \
	main.cpp\
	reversiprotocol.cpp\
	reversiview.cpp\
	kreversi.cpp \
	network.cpp

EXTRA_DIST = \
	ggz.kreversiui.rc

noinst_HEADERS = \
	reversiprotocol.h\
	reversiview.h\
	kreversi.h \
	network.h

rcdir = $(kde_datadir)/ggz.kreversi
rc_DATA = ggz.kreversiui.rc

install-data-local:
	@$(NORMAL_INSTALL)
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall:
	@$(NORMAL_UNINSTALL)
	-$(GGZ_CONFIG) --remove --modfile=module.dsc

