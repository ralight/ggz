SUBDIRS = libcgc pics .

bindir = $(ggzexecmoddir)

bin_PROGRAMS = koenig

koenig_SOURCES = \
	main.cc \
	game.cc \
	board.cc \
	ggz.cc \
	toplevel.cc \
	options.cc

noinst_HEADERS = \
	game.h \
	board.h \
	ggz.h \
	toplevel.h \
	options.h

INCLUDES = $(EASYSOCK_INCLUDES) $(all_includes) -I libcgc

UI = -L$(qt_libraries) -L$(kde_libraries) -lqt -lkdeui -lkdecore
koenig_LDADD = libcgc/libcgc.a $(all_libraries) $(UI) 

METASOURCES = AUTO

EXTRA_DIST = module.dsc.in koenigui.rc

rcdir = $(kde_datadir)/koenig
rc_DATA = koenigui.rc


install-data-local:
	@$(NORMAL_INSTALL)
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force
 
uninstall:
	@$(NORMAL_UNINSTALL)
	-$(GGZ_CONFIG) --remove --modfile=module.dsc

