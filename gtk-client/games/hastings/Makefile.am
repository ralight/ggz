SUBDIRS = pixmaps

AM_CPPFLAGS = -I$(srcdir)/../common/ $(GGZMOD_INCLUDES) $(LIBGGZ_INCLUDES) \
		$(GTK_CFLAGS)

EXTRA_DIST = module.dsc.in

ggzexecmod_PROGRAMS = ggz.hastings

ggz_hastings_SOURCES = \
	dlg_yesno.c dlg_yesno.h \
	game.h \
	main.c \
	main_win.c main_win.h \
	support.c support.h \
	selector.c selector.h

ggz_hastings_LDADD =	$(builddir)/../common/libgtk-games.a	\
			$(GTK_LIBS) $(INTLLIBS) $(LDADD)	\
			$(LIB_GGZMOD) $(LIB_GGZ)
ggz_hastings_LDFLAGS = $(GGZMOD_LDFLAGS) $(LIBGGZ_LDFLAGS)

noinst_HEADERS = version.m4

include $(top_srcdir)/m4/ggz.mk
