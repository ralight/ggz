## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 5623 2003-07-17 02:27:09Z jdorje $

SUBDIRS = .

INCLUDES = -I$(top_srcdir)/common $(GGZMOD_INCLUDES) $(LIBGGZ_INCLUDES) \
		@GTK_CFLAGS@

EXTRA_DIST = module.dsc.in

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

ggzexecmod_PROGRAMS = ggz.dots

ggz_dots_SOURCES =	cb_main.c	cb_main.h	\
			cb_new.c	cb_new.h	\
			cb_opt.c	cb_opt.h	\
			cb_pref.c	cb_pref.h	\
			dlg_main.c	dlg_main.h	\
			dlg_new.c	dlg_new.h	\
			dlg_opt.c	dlg_opt.h	\
			dlg_pref.c	dlg_pref.h	\
			game.c		game.h		\
			main.c		main.h		\
			support.c	support.h

ggz_dots_LDADD =	@GTK_LIBS@ $(LDADD)			\
			$(top_builddir)/common/libgtk-games.a	\
			$(LIB_GGZMOD) $(LIB_GGZ)
ggz_dots_LDFLAGS = $(GGZMOD_LDFLAGS) $(LIBGGZ_LDFLAGS)

