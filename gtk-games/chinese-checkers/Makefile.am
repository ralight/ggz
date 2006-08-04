## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 8466 2006-08-04 01:13:52Z jdorje $

SUBDIRS = . pixmaps

AM_CPPFLAGS = -I$(top_srcdir)/common/ $(GGZMOD_INCLUDES) $(LIBGGZ_INCLUDE) \
		$(GTK_CFLAGS)

EXTRA_DIST = module.dsc.in

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

ggzexecmod_PROGRAMS = ggz.ccheckers

ggz_ccheckers_SOURCES =	display.c	display.h	\
			game.c		game.h		\
			main.c		main.h		\
			main_cb.c	main_cb.h	\
			main_dlg.c	main_dlg.h	\
			prefs_cb.c	prefs_cb.h	\
			prefs_dlg.c	prefs_dlg.h	\
			protocol.h			\
			support.c	support.h

ggz_ccheckers_LDADD =	$(top_builddir)/common/libgtk-games.a		\
			$(GTK_LIBS) $(INTLLIBS) $(LDADD)	\
			$(LIB_GGZMOD) $(LIB_GGZ)
ggz_ccheckers_LDFLAGS = $(GGZMOD_LDFLAGS) $(LIBGGZ_LDFLAGS)


noinst_HEADERS = version.m4
