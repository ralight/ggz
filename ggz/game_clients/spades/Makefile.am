## Process this file with automake to produce Makefile.in

EXTRA_DIST = spades.dsc

SUBDIRS = .

INCLUDES = @GTK_CFLAGS@

ggzexecmod_PROGRAMS = ggz.spades

ggz_spades_SOURCES = card.c \
                    card.h \
		    client_main.c \
		    client_func.c \
                    client.h \
		    display.h \
		    gtk_connect.c \
		    gtk_connect.h \
		    gtk_dlg_about.h \
		    gtk_dlg_options.h \
		    gtk_dlg_disconnect.h \
		    gtk_dlg_error.h \
		    gtk_dlg_exit.h \
		    gtk_dlg_stat.h \
		    gtk_io.c \
		    gtk_io.h \
		    gtk_menu.h \
		    gtk_play.h \
		    gtk_taunt.h \
		    gtkspinbutton.c \
		    gtkspinbutton.h \
		    gtk_menu.c \
		    gtk_taunt.c \
		    gtk_play.c \
		    gtk_dlg_about.c \
		    gtk_dlg_options.c \
		    gtk_dlg_disconnect.c \
		    gtk_dlg_exit.c \
		    gtk_dlg_error.c \
		    gtk_dlg_stat.c \
		    options.h \
		    socketfunc.c \
		    socketfunc.h \
		    callbacks.c \
		    callbacks.h \
		    all-cards.xpm


CFLAGS = @GTK_CFLAGS@ @CFLAGS@
ggz_spades_LDADD = @GTK_LIBS@ $(LDADD)

install-data-local:
	@$(NORMAL_INSTALL)
	$(top_srcdir)/src/ggzcore/ggz-config -D --install --fromfile=spades.dsc --force


uninstall:
	@$(NORMAL_UNINSTALL)
	-ggz-config --remove --fromfile=spades.dsc
