SUBDIRS = libcgc pics .

bindir = $(ggzexecmoddir)

bin_PROGRAMS = koenig

koenig_SOURCES = \
	main.cc \
	game.cc \
	board.cc \
	boardcontainer.cc \
	ggz.cc \
	toplevel.cc \
	options.cc \
	kexttabctl.cc

noinst_HEADERS = \
	game.h \
	board.h \
	boardcontainer.h \
	ggz.h \
	toplevel.h \
	options.h \
	kexttabctl.h

INCLUDES = -I$(top_srcdir)/koenig/libcgc $(LIBGGZ_INCLUDES) $(all_includes)

koenig_LDADD = $(top_builddir)/koenig/libcgc/libcgc.a $(all_libraries) $(LIB_QT) $(LIB_KDECORE) $(LIB_KDEUI)

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

