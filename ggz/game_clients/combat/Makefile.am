## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 4659 2002-09-23 07:42:26Z jdorje $

EXTRA_DIST = module.dsc.in

SUBDIRS = . pixmaps maps

INCLUDES = -I$(top_srcdir)/common

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) --remove --modfile=module.dsc

ggzexecmod_PROGRAMS = ggz.combat

ggz_combat_SOURCES = combat.h \
				combat.c \
	      main.c \
	      support.c \
	      support.h \
				callbacks.c \
				callbacks.h \
				interface.c \
				interface.h \
				gtk_player_info.c \
				dlg_options.c \
				dlg_options.h \
				dlg_about.c \
				dlg_about.h \
        map.c \
        map.h \
				game.h \
				game.c

CFLAGS = @GTK_CFLAGS@ @CFLAGS@
ggz_combat_LDADD = @GTK_LIBS@ $(LDADD) $(top_builddir)/common/libgtk-games.a -lggz
