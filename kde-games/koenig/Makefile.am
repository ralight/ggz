## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6948 2005-02-27 21:54:15Z josef $

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
	themes.cc

noinst_HEADERS = \
	game.h \
	board.h \
	boardcontainer.h \
	ggz.h \
	toplevel.h \
	options.h \
	kexttabctl.h \
	themes.cc

AM_CPPFLAGS = -I$(top_srcdir)/koenig/libcgc \
	$(LIBGGZ_INCLUDES) $(GGZMOD_INCLUDES) $(all_includes) $(RSVG_INCLUDES)

koenig_LDFLAGS = $(LIBGGZ_LDFLAGS) $(LIBGGZMOD_LDFLAGS) $(all_libraries) $(RSVG_LDFLAGS)
koenig_LDADD = $(top_builddir)/koenig/libcgc/libcgc.a \
	$(LIB_GGZ) $(LIB_GGZMOD) $(LIB_QT) $(LIB_KDECORE) $(LIB_KDEUI) $(KNEWSTUFF)

METASOURCES = AUTO

EXTRA_DIST = koenigui.rc

rcdir = $(kde_datadir)/koenig
rc_DATA = koenigui.rc

kde_conf_DATA = koenigrc

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force
 
uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

