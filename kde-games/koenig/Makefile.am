SUBDIRS = libcgc pics pieces

ggzexecmod_PROGRAMS = koenig

koenig_SOURCES = \
	main.cc \
	game.cc \
	board.cc \
	boardcontainer.cc \
	ggz.cc \
	toplevel.cc \
	options.cc \
	kexttabctl.cc \
	ai.c \
	themes.cc

noinst_HEADERS = \
	game.h \
	board.h \
	boardcontainer.h \
	ggz.h \
	toplevel.h \
	options.h \
	kexttabctl.h \
	ai.h \
	themes.h

AM_CPPFLAGS = -I$(top_srcdir)/lib -I$(top_srcdir)/koenig/libcgc \
	$(LIBGGZ_INCLUDES) $(GGZMOD_INCLUDES) $(all_includes)

koenig_LDFLAGS = $(LIBGGZ_LDFLAGS) $(LIBGGZMOD_LDFLAGS) $(all_libraries)
koenig_LDADD = $(top_builddir)/lib/libkggzgames.la $(top_builddir)/koenig/libcgc/libcgc.a \
	$(LIB_GGZ) $(LIB_GGZMOD) $(LIB_QT) $(LIB_KDECORE) $(LIB_KDEUI) $(KNEWSTUFF)

METASOURCES = AUTO

rcdir = $(kde_datadir)/koenig
dist_rc_DATA = koenigui.rc

dist_kde_conf_DATA = koenigrc

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force
 
uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

