## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 5623 2003-07-17 02:27:09Z jdorje $

SUBDIRS = .

INCLUDES = -I$(top_srcdir)/common/ $(GGZMOD_INCLUDES) $(LIBGGZ_INCLUDES) \
		@GTK_CFLAGS@

EXTRA_DIST = module.dsc.in

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

ggzexecmod_PROGRAMS = ggz.lapocha

ggz_lapocha_SOURCES =		\
	cb_bid.c cb_bid.h	\
	cb_main.c cb_main.h	\
	dlg_bid.c dlg_bid.h	\
	dlg_main.c dlg_main.h	\
	game.c game.h		\
	hand.c hand.h		\
	main.c main.h		\
	support.c support.h	\
	table.c table.h

ggz_lapocha_LDADD = @GTK_LIBS@ $(LDADD)				\
			$(top_builddir)/common/libgtk-games.a	\
			$(LIB_GGZMOD) $(LIB_GGZ)
ggz_lapocha_LDFLAGS = $(GGZMOD_LDFLAGS) $(LIBGGZ_LDFLAGS)
