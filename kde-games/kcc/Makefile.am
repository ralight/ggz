## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6948 2005-02-27 21:54:15Z josef $

SUBDIRS = themes

ggzexecmod_PROGRAMS = kcc

kcc_SOURCES = \
	main.cpp \
	kcc.cpp \
	kccwin.cpp \
	kccproto.cpp

noinst_HEADERS = \
	kcc.h \
	kccwin.h \
	kccproto.h

AM_CPPFLAGS = $(all_includes) $(LIBGGZ_INCLUDES) $(LIBGGZMOD_INCLUDES)

kcc_LDFLAGS = $(all_libraries) $(LIBGGZ_LDFLAGS) $(LIBGGZMOD_LDFLAGS)
kcc_LDADD = $(LIB_KDECORE) $(LIB_KDEUI) $(KNEWSTUFF) $(LIB_GGZ) $(LIB_GGZMOD)

METASOURCES = AUTO

KDE_ICON = kcc

desktopdir = $(datadir)/applications
desktop_DATA = kcc.desktop

kde_conf_DATA = kccrc

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

