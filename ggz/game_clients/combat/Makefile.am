## Process this file with automake to produce Makefile.in

EXTRA_DIST = combat.dsc

SUBDIRS = . pixmaps maps

INCLUDES = @GTK_CFLAGS@ @EASYSOCK_INC@ -I$(top_srcdir)/src/ggzcore

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
ggz_combat_LDADD = @GTK_LIBS@ $(LDADD) @EASYSOCK_LIB@ $(top_builddir)/src/ggzcore/libggzcore.a
