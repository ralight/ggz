AM_CPPFLAGS = -I$(top_srcdir)/common $(GGZMOD_INCLUDES) $(LIBGGZ_INCLUDES) \
		$(GTK_CFLAGS)

EXTRA_DIST = module.dsc.in

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

ggzexecmod_PROGRAMS = ggz.ttt

ggz_ttt_SOURCES =			\
	game.h				\
	main.c				\
	main_win.c	main_win.h	\
	support.c	support.h	\
	net.c		net.h

ggz_ttt_LDADD =	$(top_builddir)/common/libgtk-games.a			\
		$(GTK_LIBS) $(INTLLIBS) $(LDADD)		\
		$(LIB_GGZMOD) $(LIB_GGZ)
ggz_ttt_LDFLAGS = $(GGZMOD_LDFLAGS) $(LIBGGZ_LDFLAGS)

pixmapdir = $(ggzdatadir)/tictactoe/pixmaps
dist_pixmap_DATA = o.svg x.svg

include $(top_srcdir)/m4/modversion.mk

PROTOCOL = tictactoe
SOURCEFILE = net
include $(top_srcdir)/m4/protocol.mk
