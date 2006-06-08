## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 8139 2006-06-08 19:53:35Z jdorje $

SUBDIRS = .

AM_CPPFLAGS = -I$(top_srcdir)/common/ $(GGZMOD_INCLUDES) $(LIBGGZ_INCLUDES) \
		$(GTK_CFLAGS)

EXTRA_DIST = module.dsc.in

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

ggzexecmod_PROGRAMS = ggz.spades

ggz_spades_SOURCES = \
		callbacks.c	callbacks.h		\
		card.c card.h				\
		client_main.c				\
		client_func.c				\
		client.h				\
		display.h				\
		gtk_connect.c		gtk_connect.h	\
		gtk_dlg_disconnect.c	gtk_dlg_disconnect.h \
		gtk_dlg_error.c		gtk_dlg_error.h	\
		gtk_dlg_options.c	gtk_dlg_options.h \
		gtk_dlg_stat.c		gtk_dlg_stat.h	\
		gtk_io.c		gtk_io.h	\
		gtk_menu.c		gtk_menu.h	\
		gtk_play.c		gtk_play.h	\
		gtk_taunt.c		gtk_taunt.h	\
		options.h				\
		socketfunc.c	socketfunc.h


ggz_spades_LDADD = 	$(top_builddir)/common/libgtk-games.a		\
			$(GTK_LIBS) $(INTLLIBS) $(LDADD)		\
			$(LIB_GGZMOD) $(LIB_GGZ)
ggz_spades_LDFLAGS = $(GGZMOD_LDFLAGS) $(LIBGGZ_LDFLAGS)
