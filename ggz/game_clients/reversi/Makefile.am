## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 5623 2003-07-17 02:27:09Z jdorje $

EXTRA_DIST = module.dsc.in

SUBDIRS = .

INCLUDES = -I$(top_srcdir)/common $(GGZMOD_INCLUDES) $(LIBGGZ_INCLUDES) \
		@GTK_CFLAGS@

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
	support.c	support.h \
	black.xpm \
	dot.xpm \
	white.xpm

ggz_reversi_LDADD =	@GTK_LIBS@ $(LDADD)			\
			$(top_builddir)/common/libgtk-games.a	\
			$(LIB_GGZMOD) $(LIB_GGZ)
ggz_reversi_LDFLAGS = $(GGZMOD_LDFLAGS) $(LIBGGZ_LDFLAGS)
