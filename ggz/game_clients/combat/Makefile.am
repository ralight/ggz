## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 6257 2004-11-04 22:53:36Z jdorje $

SUBDIRS = . pixmaps maps

AM_CPPFLAGS = -I$(top_srcdir)/common $(GGZMOD_INCLUDES) $(LIBGGZ_INCLUDES) \
		@GTK_CFLAGS@

EXTRA_DIST = module.dsc.in

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

ggzexecmod_PROGRAMS = ggz.combat

ggz_combat_SOURCES =					\
			callbacks.c	callbacks.h	\
			combat.h	combat.c	\
			dlg_options.c	dlg_options.h	\
			game.c		game.h		\
			gtk_player_info.c		\
			interface.c	interface.h	\
			main.c				\
			map.c		map.h		\
			support.c	support.h

ggz_combat_LDADD =	@GTK_LIBS@ @INTLLIBS@ $(LDADD)			\
			$(top_builddir)/common/libgtk-games.a		\
			$(LIB_GGZMOD) $(LIB_GGZ)
ggz_combat_LDFLAGS = $(GGZMOD_LDFLAGS) $(LIBGGZ_LDFLAGS)

