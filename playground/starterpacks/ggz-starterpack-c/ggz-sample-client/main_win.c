/*
 * GGZ Starterpack for C - Sample Client
 * Copyright (C) 2008 GGZ Development Team
 *
 * This code is made available as public domain; you can use it as a base
 * for your own game, as long as its licence is compatible with the libraries
 * you use.
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "game.h"
#include "main_win.h"
#include "net.h"

/* Pixmaps */
GtkWidget *main_win;

/* Global game variables */
extern struct game_state_t game;

void game_status(const char *format, ...)
{
	int id;
	va_list ap;
	char *message;
	gpointer tmp;

	va_start(ap, format);
	message = g_strdup_vprintf(format, ap);
	va_end(ap);

	tmp = g_object_get_data(G_OBJECT(main_win), "statusbar");

	id = gtk_statusbar_get_context_id(GTK_STATUSBAR(tmp), "Main");

	gtk_statusbar_pop(GTK_STATUSBAR(tmp), id);
	gtk_statusbar_push(GTK_STATUSBAR(tmp), id, message);

	g_free(message);
}

static gboolean main_exit(GtkWidget * widget, GdkEvent * event,
			  gpointer user_data)
{
	gtk_main_quit();

	return FALSE;
}

GtkWidget *create_main_win(void)
{
	GtkWidget *win_main;
	GtkWidget *main_box;
	GtkWidget *menubar;
	GtkWidget *drawingarea;
	GtkWidget *statusbar;
	GtkWidget *chat;
	GtkAccelGroup *accel_group;

	accel_group = gtk_accel_group_new();

	win_main = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_object_set_data(G_OBJECT(win_main), "main_win", win_main);
	gtk_window_set_title(GTK_WINDOW(win_main), "GGZ Starterpack for C - Sample Client");

	main_box = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(main_box);
	g_object_set_data_full(G_OBJECT(win_main), "main_box", main_box,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(main_box);
	gtk_container_add(GTK_CONTAINER(win_main), main_box);

	drawingarea = gtk_drawing_area_new();
	gtk_widget_ref(drawingarea);
	g_object_set_data_full(G_OBJECT(win_main), "drawingarea",
			       drawingarea,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(drawingarea);
	gtk_box_pack_start(GTK_BOX(main_box), drawingarea, TRUE, TRUE, 0);
	gtk_widget_set_size_request(drawingarea, 200, 200);
	gtk_widget_set_events(drawingarea,
			      GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK);

	statusbar = gtk_statusbar_new();
	gtk_widget_ref(statusbar);
	g_object_set_data_full(G_OBJECT(win_main), "statusbar", statusbar,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(statusbar);
	gtk_box_pack_start(GTK_BOX(main_box), statusbar, FALSE, FALSE, 0);

	g_signal_connect(GTK_OBJECT(win_main), "delete_event",
			 GTK_SIGNAL_FUNC(main_exit), NULL);

	gtk_window_add_accel_group(GTK_WINDOW(win_main), accel_group);

	return win_main;
}
