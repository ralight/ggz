## Process this file with automake to produce Makefile.in

EXTRA_DIST = ccheckers.dsc

SUBDIRS = .

INCLUDES = @GTK_CFLAGS@ @EASYSOCK_INC@ 

ggzexecmod_PROGRAMS = ggz.ccheckers

ggz_ccheckers_SOURCES = display.c	display.h	\
			game.c		game.h		\
			ggz.c		ggz.h		\
			main.c		main.h		\
			support.c	support.h	\
							\
			main_dlg.c	main_dlg.h	\
			main_cb.c	main_cb.h	\
			about_dlg.c	about_dlg.h	\
							\
			board.xpm			\
			hole.xpm			\
			blue.xpm cyan.xpm green.xpm	\
			purple.xpm red.xpm yellow.xpm

CFLAGS = @GTK_CFLAGS@ @CFLAGS@
ggz_ccheckers_LDADD = @GTK_LIBS@ $(LDADD) @EASYSOCK_LIB@ 

