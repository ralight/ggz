SUBDIRS = pixmaps

AM_CPPFLAGS = -I$(srcdir)/../common/ $(GGZMOD_INCLUDES) $(LIBGGZ_INCLUDE) \
		$(GTK_CFLAGS)

EXTRA_DIST = module.dsc.in

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

ggz_ccheckers_LDADD =	$(builddir)/../common/libgtk-games.a	\
			$(GTK_LIBS) $(INTLLIBS) $(LDADD)	\
			$(LIB_GGZMOD) $(LIB_GGZ)
ggz_ccheckers_LDFLAGS = $(GGZMOD_LDFLAGS) $(LIBGGZ_LDFLAGS)

noinst_HEADERS = version.m4

include $(top_srcdir)/m4/ggz.mk
