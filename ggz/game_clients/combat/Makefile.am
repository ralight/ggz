## Process this file with automake to produce Makefile.in

EXTRA_DIST = combat.dsc

SUBDIRS = .

INCLUDES = @GTK_CFLAGS@ @EASYSOCK_INC@ 

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
				game.h \
				game.c

CFLAGS = @GTK_CFLAGS@ @CFLAGS@
ggz_combat_LDADD = @GTK_LIBS@ $(LDADD) @EASYSOCK_LIB@ 



