## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 10350 2008-07-16 01:30:46Z jdorje $

EXTRA_DIST = module.dsc.in

SUBDIRS = .

AM_CPPFLAGS = -I$(srcdir)/../common $(GGZMOD_INCLUDES) $(LIBGGZ_INCLUDES) \
		$(GTK_CFLAGS)

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

ggzexecmod_PROGRAMS = ggz.reversi

ggz_reversi_SOURCES = \
	game.h \
	main.c \
	main_win.c \
	options_dlg.c	options_dlg.h \
	support.c	support.h

ggz_reversi_LDADD =	$(builddir)/../common/libgtk-games.a		\
			$(GTK_LIBS) $(INTLLIBS) $(LDADD)	\
			$(LIB_GGZMOD) $(LIB_GGZ)
ggz_reversi_LDFLAGS = $(GGZMOD_LDFLAGS) $(LIBGGZ_LDFLAGS)

pixmapdir = $(ggzdatadir)/reversi/pixmaps
dist_pixmap_DATA = white.svg black.svg dot.svg enemydot.svg

noinst_HEADERS = version.m4
