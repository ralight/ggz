## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 2246 2001-08-25 15:42:06Z jdorje $

EXTRA_DIST = module.dsc.in

SUBDIRS = . pixmaps

INCLUDES = @GTK_CFLAGS@ 

install-data-local:
	@$(NORMAL_INSTALL)
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall:
	@$(NORMAL_UNINSTALL)
	-$(GGZ_CONFIG) --remove --modfile=module.dsc

ggzexecmod_PROGRAMS = ggz.ccheckers

ggz_ccheckers_SOURCES = display.c	display.h	\
			game.c		game.h		\
			main.c		main.h		\
			support.c	support.h	\
			protocol.h			\
			main_dlg.c	main_dlg.h	\
			main_cb.c	main_cb.h	\
			about_dlg.c	about_dlg.h	\
			prefs_dlg.c	prefs_dlg.h	\
			prefs_cb.c	prefs_cb.h

CFLAGS = @GTK_CFLAGS@ @CFLAGS@
ggz_ccheckers_LDADD = @GTK_LIBS@ $(LDADD)
