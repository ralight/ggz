## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 4666 2002-09-23 12:33:57Z jdorje $

SUBDIRS = . pixmaps maps

INCLUDES = -I$(top_srcdir)/common
CFLAGS = @GTK_CFLAGS@ @CFLAGS@

EXTRA_DIST = module.dsc.in

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

ggzexecmod_PROGRAMS = ggz.combat

ggz_combat_SOURCES =					\
			callbacks.c	callbacks.h	\
			combat.h	combat.c	\
			dlg_about.c	dlg_about.h	\
			dlg_options.c	dlg_options.h	\
			game.c		game.h		\
			gtk_player_info.c		\
			interface.c	interface.h	\
			main.c				\
			map.c		map.h		\
			support.c	support.h

ggz_combat_LDADD = @GTK_LIBS@ $(LDADD) $(top_builddir)/common/libgtk-games.a
