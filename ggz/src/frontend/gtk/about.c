/*
 * File: about.c
 * Author: Justin Zaun
 * Project: GGZ GTK Client
 * $Id: about.c 6273 2004-11-05 21:49:00Z jdorje $
 *
 * This is the main program body for the GGZ client
 *
 * Copyright (C) 2000, 2001 Justin Zaun.
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

/*
 * Dialog Description
 *
 * Displayes information about the authors and the application.
 */

#include <config.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "about.h"
#include "chat.h"
#include "support.h"

static GtkWidget *about_dialog;
static GtkWidget *create_dlg_about(void);
static gint about_tag;
static GdkFont *font1, *font2, *font3, *font4;
static GdkColormap *colormap;
static GdkPixmap *pixmap;
static GdkPixbuf *bg_img;
static gint Yloc = 320;

static void about_realize(GtkWidget * widget, gpointer data);
static void about_ok(GtkWidget * widget, gpointer data);
static gint about_update(gpointer data);
static gint about_draw_text(GtkDrawingArea * background, gchar * text,
			    GdkFont * font, gint loc, gint start);


void about_create_or_raise(void)
{
	if (!about_dialog) {
		Yloc = 320;
		about_dialog = create_dlg_about();
		gtk_widget_show(about_dialog);
	} else {
		gdk_window_show(about_dialog->window);
		gdk_window_raise(about_dialog->window);
	}
}

static void about_ok(GtkWidget * widget, gpointer data)
{
	gtk_timeout_remove(about_tag);
	gtk_widget_destroy(about_dialog);
	about_dialog = NULL;
	about_tag = 0;
}


GtkWidget *create_dlg_about(void)
{
	GtkWidget *dlg_about;
	GtkWidget *dialog_vbox1;
	GtkWidget *background;
	GtkWidget *dialog_action_area1;
	GtkWidget *button_box;
	GtkWidget *ok_button;

	dlg_about = gtk_dialog_new();
	g_object_set_data(G_OBJECT(dlg_about), "dlg_about", dlg_about);
	gtk_window_set_title(GTK_WINDOW(dlg_about), _("About"));
	gtk_window_set_policy(GTK_WINDOW(dlg_about), FALSE, FALSE, FALSE);

	dialog_vbox1 = GTK_DIALOG(dlg_about)->vbox;
	g_object_set_data(G_OBJECT(dlg_about), "dialog_vbox1",
			  dialog_vbox1);
	gtk_widget_show(dialog_vbox1);

	background = gtk_drawing_area_new();
	gtk_widget_set_name(GTK_WIDGET(background), "background");
	gtk_widget_ref(background);
	g_object_set_data_full(G_OBJECT(dlg_about), "background",
			       background,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(background);
	gtk_box_pack_start(GTK_BOX(dialog_vbox1), background, FALSE, FALSE,
			   0);
	gtk_widget_set_size_request(background, 250, 300);
	GTK_WIDGET_UNSET_FLAGS(background, GTK_CAN_FOCUS);
	GTK_WIDGET_UNSET_FLAGS(background, GTK_CAN_DEFAULT);

	dialog_action_area1 = GTK_DIALOG(dlg_about)->action_area;
	g_object_set_data(G_OBJECT(dlg_about), "dialog_action_area1",
			  dialog_action_area1);
	gtk_widget_show(dialog_action_area1);
	gtk_container_set_border_width(GTK_CONTAINER(dialog_action_area1),
				       10);

	button_box = gtk_hbutton_box_new();
	gtk_widget_ref(button_box);
	g_object_set_data_full(G_OBJECT(dlg_about), "button_box",
			       button_box,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(button_box);
	gtk_box_pack_start(GTK_BOX(dialog_action_area1), button_box, TRUE,
			   TRUE, 0);

	ok_button = gtk_button_new_with_label(_("OK"));
	gtk_widget_ref(ok_button);
	g_object_set_data_full(G_OBJECT(dlg_about), "ok_button",
			       ok_button,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(ok_button);
	gtk_container_add(GTK_CONTAINER(button_box), ok_button);
	GTK_WIDGET_SET_FLAGS(ok_button, GTK_CAN_DEFAULT);

	g_signal_connect(dlg_about, "destroy",
			   GTK_SIGNAL_FUNC(about_ok), NULL);
	g_signal_connect(GTK_OBJECT(dlg_about), "realize",
			   GTK_SIGNAL_FUNC(about_realize), NULL);
	g_signal_connect(GTK_OBJECT(background), "expose_event",
			   GTK_SIGNAL_FUNC(about_update), NULL);
	g_signal_connect(GTK_OBJECT(ok_button), "clicked",
			   GTK_SIGNAL_FUNC(about_ok), NULL);

	return dlg_about;
}


static void about_realize(GtkWidget * widget, gpointer data)
{
	GtkStyle *oldstyle, *newstyle;
	GtkWidget *tmp;

	font1 = gdk_font_load
		("-*-helvetica-bold-r-normal-*-*-200-*-*-p-*-iso8859-1");
	font2 = gdk_font_load
		("-*-helvetica-bold-r-normal-*-*-180-*-*-p-*-iso8859-1");
	font3 = gdk_font_load
		("-*-helvetica-bold-r-normal-*-*-130-*-*-p-*-iso8859-1");
	font4 = gdk_font_load
		("-*-helvetica-medium-r-normal-*-*-110-*-*-p-*-iso8859-1");
	colormap = gdk_colormap_get_system();
	pixmap = gdk_pixmap_new(widget->window, 250, 300, -1);
	bg_img = load_pixbuf("about_bg");
	if (bg_img == NULL)
		g_error("Couldn't create about background pixmap.");

	tmp = g_object_get_data(G_OBJECT(about_dialog), "background");
	oldstyle = gtk_widget_get_style(tmp);
	newstyle = gtk_style_copy(oldstyle);
	newstyle->text[5] = colors[12];
	gtk_widget_set_style(tmp, newstyle);

	about_tag = gtk_timeout_add(100, about_update, NULL);
}


/*
 * Make a new pixmap and draw to it, coping it to
 * the real background made above
 */
static gint about_update(gpointer data)
{
	GtkDrawingArea *background;
	int status;

	background =
		g_object_get_data(G_OBJECT(about_dialog), "background");
	gdk_pixbuf_render_to_drawable(bg_img, pixmap,
				      GTK_WIDGET(background)->style->
				      fg_gc[GTK_WIDGET_STATE(background)],
				      0, 0, 0, 0, 250, 300,
				      GDK_RGB_DITHER_NONE, 0, 0);

	/* FIXME: we ignore all status checks but the last?? */
	status = about_draw_text(background, "GGZ Gaming Zone", font1, Yloc,
				 TRUE);

	status = about_draw_text(background, VERSION, font2, Yloc, FALSE);

	status = about_draw_text(background, "(C) 1999, 2000, 2001", font2,
				 Yloc, FALSE);
	status = about_draw_text(background, "(http://ggz.sourceforge.net)",
				 font4, Yloc, FALSE);
	status = about_draw_text(background, " ", font2, Yloc, FALSE);
	status = about_draw_text(background, " ", font2, Yloc, FALSE);
	status = about_draw_text(background, "Server", font2, Yloc, FALSE);
	status = about_draw_text(background, "Rich Gade", font3, Yloc, FALSE);
	status = about_draw_text(background, "Brent Hendricks", font3, Yloc,
				 FALSE);
	status = about_draw_text(background, " ", font2, Yloc, FALSE);
	status = about_draw_text(background, "GTK+ Client", font2, Yloc,
				 FALSE);
	status = about_draw_text(background, "Brian Cox", font3, Yloc, FALSE);
	status = about_draw_text(background, "Rich Gade", font3, Yloc, FALSE);
	status = about_draw_text(background, "Brent Hendricks", font3, Yloc,
				 FALSE);
	status = about_draw_text(background, "Ismael Orenstein", font3, Yloc,
				 FALSE);
	status = about_draw_text(background, "Dan Papasian", font3, Yloc,
				 FALSE);
	status = about_draw_text(background, "Justin Zaun", font3, Yloc,
				 FALSE);
	status = about_draw_text(background, " ", font2, Yloc, FALSE);
	status = about_draw_text(background, "KDE2 Client", font2, Yloc,
				 FALSE);
	status = about_draw_text(background, "Josef Spillner", font3, Yloc,
				 FALSE);
	status = about_draw_text(background, " ", font2, Yloc, FALSE);
	status = about_draw_text(background, "Windows Client", font2, Yloc,
				 FALSE);
	status = about_draw_text(background, "Doug Hudson", font3, Yloc,
				 FALSE);
	status = about_draw_text(background, "Roger Light", font3, Yloc,
				 FALSE);
	status = about_draw_text(background, "Justin Zaun", font3, Yloc,
				 FALSE);
	status = about_draw_text(background, " ", font2, Yloc, FALSE);
	status = about_draw_text(background, "Text Client", font2, Yloc,
				 FALSE);
	status = about_draw_text(background, "Brent Hendricks", font3, Yloc,
				 FALSE);
	status = about_draw_text(background, "Justin Zaun", font3, Yloc,
				 FALSE);
	status = about_draw_text(background, " ", font2, Yloc, FALSE);
	status = about_draw_text(background, "Libraries", font2, Yloc, FALSE);
	status = about_draw_text(background, "Rich Gade", font3, Yloc, FALSE);
	status = about_draw_text(background, "Brent Hendricks", font3, Yloc,
				 FALSE);
	status = about_draw_text(background, "Ismael Orenstein", font3, Yloc,
				 FALSE);
	status = about_draw_text(background, "Justin Zaun", font3, Yloc,
				 FALSE);
	gdk_draw_pixmap(GTK_WIDGET(background)->window,
			GTK_WIDGET(background)->style->
			fg_gc[GTK_WIDGET_STATE(background)], pixmap, 0, 0, 0,
			0, 250, 300);

	if (status)
		Yloc = 320;
	Yloc = Yloc - 2;
	return TRUE;
}

static gint about_draw_text(GtkDrawingArea * background, gchar * text,
			    GdkFont * font, gint loc, gint start)
{
	static int l;

	if (start) {
		l = 0;
	} else {
		l = l + gdk_string_height(font, text) + 10;
	}


	gdk_draw_text(pixmap, font, GTK_WIDGET(background)->style->text_gc[5],
		      (250 / 2) - (gdk_string_width(font, text) / 2), loc + l,
		      text, strlen(text));

	if (loc + l + 10 <= 0)
		return TRUE;
	else
		return FALSE;
}
