## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 4722 2002-09-26 11:13:59Z dr_maux $

SUBDIRS = . pixmaps

INCLUDES = -I$(top_srcdir)/common/ $(GGZMOD_INCLUDES)
CFLAGS = @GTK_CFLAGS@ @CFLAGS@

EXTRA_DIST = module.dsc.in

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

ggzexecmod_PROGRAMS = ggz.ccheckers

ggz_ccheckers_SOURCES =	about_dlg.c	about_dlg.h	\
			display.c	display.h	\
			game.c		game.h		\
			main.c		main.h		\
			main_cb.c	main_cb.h	\
			main_dlg.c	main_dlg.h	\
			prefs_cb.c	prefs_cb.h	\
			prefs_dlg.c	prefs_dlg.h	\
			protocol.h			\
			support.c	support.h

ggz_ccheckers_LDADD = @GTK_LIBS@ $(top_builddir)/common/libgtk-games.a $(LDADD) $(LIB_GGZMOD)
ggz_ccheckers_LDFLAGS = $(GGZMOD_LDFLAGS)

