## Process this file with automake to produce Makefile.in

SUBDIRS = 

INCLUDES = @GTK_CFLAGS@ -I$(top_srcdir)/easysock

bin_PROGRAMS = gspades

gspades_SOURCES = card.c \
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
gspades_LDADD = @GTK_LIBS@ $(LDADD)


