## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 6242 2004-11-03 20:27:08Z jdorje $

SUBDIRS = .

INCLUDES = -I$(top_srcdir)/common $(GGZMOD_INCLUDES) $(LIBGGZ_INCLUDES) \
		@GTK_CFLAGS@

EXTRA_DIST = module.dsc.in

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

ggzexecmod_PROGRAMS = ggz.ttt

ggz_ttt_SOURCES =			\
	game.h				\
	main.c				\
	main_win.c	main_win.h	\
	support.c	support.h

ggz_ttt_LDADD =	@GTK_LIBS@ @INTLLIBS@ $(LDADD)				\
		$(top_builddir)/common/libgtk-games.a			\
		$(LIB_GGZMOD) $(LIB_GGZ)
ggz_ttt_LDFLAGS = $(GGZMOD_LDFLAGS) $(LIBGGZ_LDFLAGS)

pixmapdir = $(ggzdatadir)/tictactoe/pixmaps
dist_pixmap_DATA = o.png x.png
