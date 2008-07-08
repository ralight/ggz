/*
 * File: about.c
 * Author: Justin Zaun
 * Project: GGZ GTK Client
 * $Id: about.c 10230 2008-07-08 19:26:50Z jdorje $
 *
 * About dialog: Displays information about the authors and the application.
 *
 * Copyright (C) 2000-2004 GGZ Development Team
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

#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "about.h"
#include "chat.h"
#include "client.h"
#include "support.h"

struct about {
	GtkWidget *dialog;
	guint tag;
	PangoFontDescription *font[4];
	GdkColormap *colormap;
	GdkPixmap *pixmap;
	GdkPixbuf *bg_img;
	gint Yloc;
};

static struct about about = {
	.Yloc = 320
};

static GtkWidget *create_dlg_about(void);
static void about_realize(GtkWidget * widget, gpointer data);
static gint about_update(gpointer data);
static gint about_draw_text(GtkDrawingArea * background, gchar * text,
			    PangoFontDescription * font, gint loc,
			    gint start);

void about_create_or_raise(void)
{
	if (!about.dialog) {
		about.Yloc = 320;
		about.dialog = create_dlg_about();
		gtk_widget_show_all(about.dialog);
	} else {
		gdk_window_show(about.dialog->window);
		gdk_window_raise(about.dialog->window);
	}
}

static void about_response(GtkDialog * dialog, gint response,
			   gpointer data)
{
	assert(response = GTK_RESPONSE_CLOSE);
	g_source_remove(about.tag);
	gtk_widget_destroy(about.dialog);
	about.dialog = NULL;
	about.tag = 0;
}

static GtkWidget *create_dlg_about(void)
{
	GtkWidget *dlg_about;
	GtkWidget *dialog_vbox1;
	GtkWidget *background;

	dlg_about = gtk_dialog_new_with_buttons(_("About"),
						GTK_WINDOW(main_window), 0,
						GTK_STOCK_CLOSE,
						GTK_RESPONSE_CLOSE, NULL);
	gtk_window_set_resizable(GTK_WINDOW(dlg_about), FALSE);

	dialog_vbox1 = GTK_DIALOG(dlg_about)->vbox;

	background = gtk_drawing_area_new();
	gtk_widget_set_name(GTK_WIDGET(background), "background");
	g_object_set_data(G_OBJECT(dlg_about), "background", background);
	gtk_box_pack_start(GTK_BOX(dialog_vbox1), background, FALSE, FALSE,
			   0);
	gtk_widget_set_size_request(background, 250, 300);
	GTK_WIDGET_UNSET_FLAGS(background, GTK_CAN_FOCUS);
	GTK_WIDGET_UNSET_FLAGS(background, GTK_CAN_DEFAULT);

	g_signal_connect(dlg_about, "realize",
			 GTK_SIGNAL_FUNC(about_realize), NULL);
	g_signal_connect(background, "expose_event",
			 GTK_SIGNAL_FUNC(about_update), NULL);
	g_signal_connect(dlg_about, "response",
			 GTK_SIGNAL_FUNC(about_response), NULL);

	return dlg_about;
}


static void about_realize(GtkWidget * widget, gpointer data)
{
	GtkStyle *oldstyle, *newstyle;
	GtkWidget *tmp;
	int i;

	about.colormap = gdk_colormap_get_system();
	about.pixmap = gdk_pixmap_new(widget->window, 250, 300, -1);
	about.bg_img = load_pixbuf("about_bg");
	if (about.bg_img == NULL) {
		g_error("Couldn't create about background pixmap.");
	}

	tmp = g_object_get_data(G_OBJECT(about.dialog), "background");
	oldstyle = gtk_widget_get_style(tmp);
	newstyle = gtk_style_copy(oldstyle);
	/* I don't know why it's the 4th element of text */
	newstyle->text[4] = colors[12];
	gtk_widget_set_style(tmp, newstyle);

	if (!about.font[0]) {
		for (i = 0; i < 4; i++) {
			about.font[i] =
			    pango_font_description_copy
			    (oldstyle->font_desc);
		}

		for (i = 0; i < 3; i++) {
			pango_font_description_set_weight(about.font[i],
							  PANGO_WEIGHT_BOLD);
		}

		pango_font_description_set_size(about.font[0],
						PANGO_SCALE * 16);
		pango_font_description_set_size(about.font[1],
						PANGO_SCALE * 14);
		pango_font_description_set_size(about.font[2],
						PANGO_SCALE * 12);
		pango_font_description_set_size(about.font[3],
						PANGO_SCALE * 9);
	}

	about.tag = g_timeout_add(100, about_update, NULL);
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
	    g_object_get_data(G_OBJECT(about.dialog), "background");
	gdk_draw_pixbuf(about.pixmap,
			GTK_WIDGET(background)->style->
			fg_gc[GTK_WIDGET_STATE(background)], about.bg_img,
			0, 0, 0, 0, 250, 300, GDK_RGB_DITHER_NONE, 0, 0);

	/* FIXME: we ignore all status checks but the last?? */
	status =
	    about_draw_text(background, "GGZ Gaming Zone", about.font[0],
			    about.Yloc, TRUE);

	status =
	    about_draw_text(background, VERSION, about.font[1], about.Yloc,
			    FALSE);

	status =
	    about_draw_text(background, "(C) 1999-2004", about.font[1],
			    about.Yloc, FALSE);
	status =
	    about_draw_text(background, "(http://www.ggzgamingzone.org)",
			    about.font[3], about.Yloc, FALSE);
	status =
	    about_draw_text(background, " ", about.font[1], about.Yloc,
			    FALSE);
	status =
	    about_draw_text(background, " ", about.font[1], about.Yloc,
			    FALSE);
	status =
	    about_draw_text(background, "Server", about.font[1],
			    about.Yloc, FALSE);
	status =
	    about_draw_text(background, "Rich Gade", about.font[2],
			    about.Yloc, FALSE);
	status =
	    about_draw_text(background, "Brent Hendricks", about.font[2],
			    about.Yloc, FALSE);
	status =
	    about_draw_text(background, "Jason Short", about.font[2],
			    about.Yloc, FALSE);
	status =
	    about_draw_text(background, " ", about.font[1], about.Yloc,
			    FALSE);
	status =
	    about_draw_text(background, "GTK+ Client", about.font[1],
			    about.Yloc, FALSE);
	status =
	    about_draw_text(background, "Brian Cox", about.font[2],
			    about.Yloc, FALSE);
	status =
	    about_draw_text(background, "Rich Gade", about.font[2],
			    about.Yloc, FALSE);
	status =
	    about_draw_text(background, "Brent Hendricks", about.font[2],
			    about.Yloc, FALSE);
	status =
	    about_draw_text(background, "Ismael Orenstein", about.font[2],
			    about.Yloc, FALSE);
	status =
	    about_draw_text(background, "Dan Papasian", about.font[2],
			    about.Yloc, FALSE);
	status =
	    about_draw_text(background, "Justin Zaun", about.font[2],
			    about.Yloc, FALSE);
	status =
	    about_draw_text(background, "Jason Short", about.font[2],
			    about.Yloc, FALSE);
	status =
	    about_draw_text(background, " ", about.font[1], about.Yloc,
			    FALSE);
	status =
	    about_draw_text(background, "KDE2 Client", about.font[1],
			    about.Yloc, FALSE);
	status =
	    about_draw_text(background, "Josef Spillner", about.font[2],
			    about.Yloc, FALSE);
	status =
	    about_draw_text(background, " ", about.font[1], about.Yloc,
			    FALSE);
	status =
	    about_draw_text(background, "Windows Client", about.font[1],
			    about.Yloc, FALSE);
	status =
	    about_draw_text(background, "Doug Hudson", about.font[2],
			    about.Yloc, FALSE);
	status =
	    about_draw_text(background, "Roger Light", about.font[2],
			    about.Yloc, FALSE);
	status =
	    about_draw_text(background, "Justin Zaun", about.font[2],
			    about.Yloc, FALSE);
	status =
	    about_draw_text(background, " ", about.font[1], about.Yloc,
			    FALSE);
	status =
	    about_draw_text(background, "Text Client", about.font[1],
			    about.Yloc, FALSE);
	status =
	    about_draw_text(background, "Brent Hendricks", about.font[2],
			    about.Yloc, FALSE);
	status =
	    about_draw_text(background, "Justin Zaun", about.font[2],
			    about.Yloc, FALSE);
	status =
	    about_draw_text(background, " ", about.font[1], about.Yloc,
			    FALSE);
	status =
	    about_draw_text(background, "Libraries", about.font[1],
			    about.Yloc, FALSE);
	status =
	    about_draw_text(background, "Rich Gade", about.font[2],
			    about.Yloc, FALSE);
	status =
	    about_draw_text(background, "Brent Hendricks", about.font[2],
			    about.Yloc, FALSE);
	status =
	    about_draw_text(background, "Ismael Orenstein", about.font[2],
			    about.Yloc, FALSE);
	status =
	    about_draw_text(background, "Justin Zaun", about.font[2],
			    about.Yloc, FALSE);
	status =
	    about_draw_text(background, "Jason Short", about.font[2],
			    about.Yloc, FALSE);
	status =
	    about_draw_text(background, "Josef Spillner", about.font[2],
			    about.Yloc, FALSE);
	gdk_draw_drawable(GTK_WIDGET(background)->window,
			  GTK_WIDGET(background)->style->
			  fg_gc[GTK_WIDGET_STATE(background)],
			  about.pixmap, 0, 0, 0, 0, 250, 300);

	if (status)
		about.Yloc = 320;
	about.Yloc = about.Yloc - 2;
	return TRUE;
}

static gint about_draw_text(GtkDrawingArea * background, gchar * text,
			    PangoFontDescription * font, gint loc,
			    gint start)
{
	static int l;
	PangoLayout *layout;
	PangoRectangle rect;

	layout = pango_layout_new(gdk_pango_context_get());
	pango_layout_set_font_description(layout, font);
	pango_layout_set_text(layout, text, -1);
	pango_layout_get_pixel_extents(layout, NULL, &rect);

	if (start) {
		l = 0;
	}


	/* I don't know why it's the 4th element of text_gc */
	gdk_draw_layout(about.pixmap,
			GTK_WIDGET(background)->style->text_gc[4],
			(250 - rect.width) / 2, loc + l, layout);
	l += rect.height;

	if (loc + l <= 0)
		return TRUE;
	else
		return FALSE;
}
