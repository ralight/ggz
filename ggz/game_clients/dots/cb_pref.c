/*
 * File: cb_pref.c
 * Author: Rich Gade
 * Project: GGZ Connect the Dots Client
 * Date: 01/22/2001
 * Desc: Callback functions for the "Preferences" Gtk dialog
 *
 * Copyright (C) 2001 Brent Hendricks.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <string.h>

#include "cb_pref.h"
#include "dlg_pref.h"
#include "support.h"
#include "main.h"
#include "game.h"
#include "cb_main.h"

static GtkWidget *fg_spot=NULL, *bg_spot, *p1_spot, *p2_spot;
static GdkColormap *sys_colormap;
static GtkWidget *dlg_colorsel = NULL;
static int state=-1;
static GdkColor new_fg, new_bg, new_p1, new_p2;


static void
on_dlg_colorsel_ok_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{
	gdouble color[3];

	gtk_color_selection_get_color(GTK_COLOR_SELECTION(user_data),
				      color);

	switch(state) {
		case 0:
			new_bg.red = (guint16)(color[0]*65535.0);
			new_bg.green = (guint16)(color[1]*65535.0);
			new_bg.blue = (guint16)(color[2]*65535.0);
			gdk_color_alloc(sys_colormap, &new_bg);
			gdk_window_set_background(bg_spot->window, &new_bg);
			gdk_window_clear(bg_spot->window);
			break;
		case 1:
			new_p2.red = (guint16)(color[0]*65535.0);
			new_p2.green = (guint16)(color[1]*65535.0);
			new_p2.blue = (guint16)(color[2]*65535.0);
			gdk_color_alloc(sys_colormap, &new_p2);
			gdk_window_set_background(p2_spot->window, &new_p2);
			gdk_window_clear(p2_spot->window);
			break;
		case 2:
			new_p1.red = (guint16)(color[0]*65535.0);
			new_p1.green = (guint16)(color[1]*65535.0);
			new_p1.blue = (guint16)(color[2]*65535.0);
			gdk_color_alloc(sys_colormap, &new_p1);
			gdk_window_set_background(p1_spot->window, &new_p1);
			gdk_window_clear(p1_spot->window);
			break;
		case 3:
			new_fg.red = (guint16)(color[0]*65535.0);
			new_fg.green = (guint16)(color[1]*65535.0);
			new_fg.blue = (guint16)(color[2]*65535.0);
			gdk_color_alloc(sys_colormap, &new_fg);
			gdk_window_set_background(fg_spot->window, &new_fg);
			gdk_window_clear(fg_spot->window);
			break;
	}

	gtk_widget_destroy(dlg_colorsel);
	dlg_colorsel = NULL;
	state = -1;
}


static void
on_dlg_colorsel_cancel_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_destroy(dlg_colorsel);
	dlg_colorsel = NULL;
	state = -1;
}


static gboolean
on_dlg_colorsel_delete_event           (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	dlg_colorsel = NULL;
	state = -1;
	return FALSE;
}


gboolean
on_bg_colorspot_button_press_event     (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
	GtkWidget *colorsel, *ok_button, *cancel_button;
	gdouble color[3];

	if(dlg_colorsel == NULL) {
		dlg_colorsel = gtk_color_selection_dialog_new("Select background color");
		colorsel = GTK_COLOR_SELECTION_DIALOG(dlg_colorsel)->colorsel;
		ok_button = GTK_COLOR_SELECTION_DIALOG(dlg_colorsel)->ok_button;
		cancel_button = GTK_COLOR_SELECTION_DIALOG(dlg_colorsel)->cancel_button;
		color[0] = (gdouble)new_bg.red / 65535.0;
		color[1] = (gdouble)new_bg.green / 65535.0;
		color[2] = (gdouble)new_bg.blue / 65535.0;
		gtk_color_selection_set_color(GTK_COLOR_SELECTION(colorsel),
					      color);
		g_signal_connect(GTK_OBJECT(dlg_colorsel), "delete_event",
				   GTK_SIGNAL_FUNC(on_dlg_colorsel_delete_event),
				   NULL);
		g_signal_connect(GTK_OBJECT(ok_button), "clicked",
				   GTK_SIGNAL_FUNC(on_dlg_colorsel_ok_clicked),
				   colorsel);
		g_signal_connect(GTK_OBJECT(cancel_button), "clicked",
				   GTK_SIGNAL_FUNC(on_dlg_colorsel_cancel_clicked),
				   colorsel);
		gtk_widget_show(dlg_colorsel);
		state = 0;
	}

	return FALSE;
}


gboolean
on_p2_colorspot_button_press_event     (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
	GtkWidget *colorsel, *ok_button, *cancel_button;
	gdouble color[3];

	if(dlg_colorsel == NULL) {
		dlg_colorsel = gtk_color_selection_dialog_new("Select player two color");
		colorsel = GTK_COLOR_SELECTION_DIALOG(dlg_colorsel)->colorsel;
		ok_button = GTK_COLOR_SELECTION_DIALOG(dlg_colorsel)->ok_button;
		cancel_button = GTK_COLOR_SELECTION_DIALOG(dlg_colorsel)->cancel_button;
		color[0] = (gdouble)new_p2.red / 65535.0;
		color[1] = (gdouble)new_p2.green / 65535.0;
		color[2] = (gdouble)new_p2.blue / 65535.0;
		gtk_color_selection_set_color(GTK_COLOR_SELECTION(colorsel),
					      color);
		g_signal_connect(GTK_OBJECT(dlg_colorsel), "delete_event",
				   GTK_SIGNAL_FUNC(on_dlg_colorsel_delete_event),
				   NULL);
		g_signal_connect(GTK_OBJECT(ok_button), "clicked",
				   GTK_SIGNAL_FUNC(on_dlg_colorsel_ok_clicked),
				   colorsel);
		g_signal_connect(GTK_OBJECT(cancel_button), "clicked",
				   GTK_SIGNAL_FUNC(on_dlg_colorsel_cancel_clicked),
				   colorsel);
		gtk_widget_show(dlg_colorsel);
		state = 1;
	}

	return FALSE;
}


gboolean
on_p1_colorspot_button_press_event     (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
	GtkWidget *colorsel, *ok_button, *cancel_button;
	gdouble color[3];

	if(dlg_colorsel == NULL) {
		dlg_colorsel = gtk_color_selection_dialog_new("Select player one color");
		colorsel = GTK_COLOR_SELECTION_DIALOG(dlg_colorsel)->colorsel;
		ok_button = GTK_COLOR_SELECTION_DIALOG(dlg_colorsel)->ok_button;
		cancel_button = GTK_COLOR_SELECTION_DIALOG(dlg_colorsel)->cancel_button;
		color[0] = (gdouble)new_p1.red / 65535.0;
		color[1] = (gdouble)new_p1.green / 65535.0;
		color[2] = (gdouble)new_p1.blue / 65535.0;
		gtk_color_selection_set_color(GTK_COLOR_SELECTION(colorsel),
					      color);
		g_signal_connect(GTK_OBJECT(dlg_colorsel), "delete_event",
				   GTK_SIGNAL_FUNC(on_dlg_colorsel_delete_event),
				   NULL);
		g_signal_connect(GTK_OBJECT(ok_button), "clicked",
				   GTK_SIGNAL_FUNC(on_dlg_colorsel_ok_clicked),
				   colorsel);
		g_signal_connect(GTK_OBJECT(cancel_button), "clicked",
				   GTK_SIGNAL_FUNC(on_dlg_colorsel_cancel_clicked),
				   colorsel);
		gtk_widget_show(dlg_colorsel);
		state = 2;
	}

  return FALSE;
}


gboolean
on_fg_colorspot_button_press_event     (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
	GtkWidget *colorsel, *ok_button, *cancel_button;
	gdouble color[3];

	if(dlg_colorsel == NULL) {
		dlg_colorsel = gtk_color_selection_dialog_new("Select foreground color");
		colorsel = GTK_COLOR_SELECTION_DIALOG(dlg_colorsel)->colorsel;
		ok_button = GTK_COLOR_SELECTION_DIALOG(dlg_colorsel)->ok_button;
		cancel_button = GTK_COLOR_SELECTION_DIALOG(dlg_colorsel)->cancel_button;
		color[0] = (gdouble)new_fg.red / 65535.0;
		color[1] = (gdouble)new_fg.green / 65535.0;
		color[2] = (gdouble)new_fg.blue / 65535.0;
		gtk_color_selection_set_color(GTK_COLOR_SELECTION(colorsel),
					      color);
		g_signal_connect(GTK_OBJECT(dlg_colorsel), "delete_event",
				   GTK_SIGNAL_FUNC(on_dlg_colorsel_delete_event),
				   NULL);
		g_signal_connect(GTK_OBJECT(ok_button), "clicked",
				   GTK_SIGNAL_FUNC(on_dlg_colorsel_ok_clicked),
				   colorsel);
		g_signal_connect(GTK_OBJECT(cancel_button), "clicked",
				   GTK_SIGNAL_FUNC(on_dlg_colorsel_cancel_clicked),
				   colorsel);
		gtk_widget_show(dlg_colorsel);
		state = 3;
	}

	return FALSE;
}


void
on_pref_button_ok_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
	memcpy(&fg_color, &new_fg, sizeof(GdkColor));
	memcpy(&bg_color, &new_bg, sizeof(GdkColor));
	memcpy(&p1_color, &new_p1, sizeof(GdkColor));
	memcpy(&p2_color, &new_p2, sizeof(GdkColor));

	game_apply_colors(new_fg, new_bg, new_p1, new_p2);
	game_write_colors();

	fg_spot = bg_spot = p1_spot = p2_spot = NULL;
	gtk_widget_destroy(dlg_pref);
}


void
on_pref_button_apply_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{
	game_apply_colors(new_fg, new_bg, new_p1, new_p2);
}


void
on_pref_button_cancel_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
	game_apply_colors(fg_color, bg_color, p1_color, p2_color);
	fg_spot = bg_spot = p1_spot = p2_spot = NULL;
	gtk_widget_destroy(dlg_pref);
}


gboolean
on_dlg_pref_expose_event               (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
	if(fg_spot == NULL) {
		fg_spot = gtk_object_get_data(GTK_OBJECT(dlg_pref),
					      "fg_colorspot");
		bg_spot = gtk_object_get_data(GTK_OBJECT(dlg_pref),
					      "bg_colorspot");
		p1_spot = gtk_object_get_data(GTK_OBJECT(dlg_pref),
					      "p1_colorspot");
		p2_spot = gtk_object_get_data(GTK_OBJECT(dlg_pref),
					      "p2_colorspot");

		sys_colormap = gdk_colormap_get_system();

		memcpy(&new_fg, &fg_color, sizeof(GdkColor));
		memcpy(&new_bg, &bg_color, sizeof(GdkColor));
		memcpy(&new_p1, &p1_color, sizeof(GdkColor));
		memcpy(&new_p2, &p2_color, sizeof(GdkColor));
		gdk_window_set_background(fg_spot->window, &new_fg);
		gdk_window_set_background(bg_spot->window, &new_bg);
		gdk_window_set_background(p1_spot->window, &new_p1);
		gdk_window_set_background(p2_spot->window, &new_p2);
	}

	gdk_window_clear(fg_spot->window);
	gdk_window_clear(bg_spot->window);
	gdk_window_clear(p1_spot->window);
	gdk_window_clear(p2_spot->window);

	return FALSE;
}


gboolean
on_dlg_pref_delete_event               (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	fg_spot = bg_spot = p1_spot = p2_spot = NULL;
	game_apply_colors(fg_color, bg_color, p1_color, p2_color);
	return FALSE;
}

