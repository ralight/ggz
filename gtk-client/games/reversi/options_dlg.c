/*
 * File: options_dlg.c
 * Author: Ismael Orenstein
 * Project: GGZ Reversi game module
 * Date: 2000
 * Desc: Options dialog code
 * $Id: options_dlg.c 6354 2004-11-13 18:19:09Z jdorje $
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

#include <stdlib.h>	/* added by me --jds */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "options_dlg.h"
#include "support.h"

GtkWidget *color_sel;
GtkWidget *dialog1;

GtkWidget *create_options_dialog(GdkColor * _back_color,
				 GdkColor * _last_color)
{
	GtkWidget *dialog_vbox1;
	GtkWidget *bg_frame;
	GtkWidget *vbox1;
	GtkWidget *hbox5;
	GSList *bg_mode_group = NULL;
	GtkWidget *color_radio;
	GtkWidget *pixmap_radio;
	GtkWidget *hbox4;
	GtkWidget *bg_label;
	GtkWidget *back_button;
	GtkWidget *hbox7;
	GtkWidget *pixmap_label;
	GtkWidget *hbox6;
	GtkWidget *last_label;
	GtkWidget *last_button;
	GtkWidget *dialog_action_area1;
	GtkWidget *ok_button;
	GtkWidget *cancel_button;

	dialog1 = gtk_dialog_new();
	gtk_widget_set_name(dialog1, "dialog1");
	g_object_set_data(G_OBJECT(dialog1), "dialog1", dialog1);
	gtk_window_set_title(GTK_WINDOW(dialog1), ("Options dialog"));
	gtk_window_set_resizable(GTK_WINDOW(dialog1), TRUE);

	dialog_vbox1 = GTK_DIALOG(dialog1)->vbox;
	gtk_widget_set_name(dialog_vbox1, "dialog_vbox1");
	g_object_set_data(G_OBJECT(dialog1), "dialog_vbox1", dialog_vbox1);
	gtk_container_set_border_width(GTK_CONTAINER(dialog_vbox1), 5);
	gtk_widget_show(dialog_vbox1);

	bg_frame = gtk_frame_new(("Background"));
	gtk_widget_set_name(bg_frame, "bg_frame");
	gtk_widget_ref(bg_frame);
	g_object_set_data_full(G_OBJECT(dialog1), "bg_frame", bg_frame,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(bg_frame);
	gtk_box_pack_start(GTK_BOX(dialog_vbox1), bg_frame, FALSE, TRUE,
			   0);

	vbox1 = gtk_vbox_new(FALSE, 0);
	gtk_widget_set_name(vbox1, "vbox1");
	gtk_widget_ref(vbox1);
	g_object_set_data_full(G_OBJECT(dialog1), "vbox1", vbox1,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox1);
	gtk_container_add(GTK_CONTAINER(bg_frame), vbox1);

	hbox5 = gtk_hbox_new(FALSE, 0);
	gtk_widget_set_name(hbox5, "hbox5");
	gtk_widget_ref(hbox5);
	g_object_set_data_full(G_OBJECT(dialog1), "hbox5", hbox5,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox5);
	gtk_box_pack_start(GTK_BOX(vbox1), hbox5, TRUE, TRUE, 5);

	color_radio =
	    gtk_radio_button_new_with_label(bg_mode_group,
					    ("Background color"));
	bg_mode_group =
	    gtk_radio_button_get_group(GTK_RADIO_BUTTON(color_radio));
	gtk_widget_set_name(color_radio, "color_radio");
	gtk_widget_ref(color_radio);
	g_object_set_data_full(G_OBJECT(dialog1), "color_radio",
			       color_radio,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(color_radio);
	gtk_box_pack_start(GTK_BOX(hbox5), color_radio, FALSE, FALSE, 5);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(color_radio), TRUE);

	pixmap_radio =
	    gtk_radio_button_new_with_label(bg_mode_group,
					    ("Background Pixmap"));
	bg_mode_group =
	    gtk_radio_button_get_group(GTK_RADIO_BUTTON(pixmap_radio));
	gtk_widget_set_name(pixmap_radio, "pixmap_radio");
	gtk_widget_ref(pixmap_radio);
	g_object_set_data_full(G_OBJECT(dialog1), "pixmap_radio",
			       pixmap_radio,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pixmap_radio);
	gtk_widget_set_sensitive(pixmap_radio, FALSE);
	gtk_box_pack_start(GTK_BOX(hbox5), pixmap_radio, FALSE, FALSE, 5);

	hbox4 = gtk_hbox_new(FALSE, 0);
	gtk_widget_set_name(hbox4, "hbox4");
	gtk_widget_ref(hbox4);
	g_object_set_data_full(G_OBJECT(dialog1), "hbox4", hbox4,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox4);
	gtk_box_pack_start(GTK_BOX(vbox1), hbox4, TRUE, TRUE, 5);

	bg_label = gtk_label_new(("Background color: "));
	gtk_widget_set_name(bg_label, "bg_label");
	gtk_widget_ref(bg_label);
	g_object_set_data_full(G_OBJECT(dialog1), "bg_label", bg_label,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(bg_label);
	gtk_box_pack_start(GTK_BOX(hbox4), bg_label, FALSE, FALSE, 5);

	back_button = gtk_button_new_with_label("");
	gtk_widget_set_name(back_button, "back_button");
	gtk_widget_ref(back_button);
	g_object_set_data_full(G_OBJECT(dialog1), "back_button",
			       back_button,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(back_button);
	gtk_box_pack_start(GTK_BOX(hbox4), back_button, FALSE, FALSE, 50);

	hbox7 = gtk_hbox_new(FALSE, 0);
	gtk_widget_set_name(hbox7, "hbox7");
	gtk_widget_ref(hbox7);
	g_object_set_data_full(G_OBJECT(dialog1), "hbox7", hbox7,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox7);
	gtk_box_pack_start(GTK_BOX(vbox1), hbox7, TRUE, TRUE, 5);

	pixmap_label = gtk_label_new(("Background pixmap:"));
	gtk_widget_set_name(pixmap_label, "pixmap_label");
	gtk_widget_ref(pixmap_label);
	g_object_set_data_full(G_OBJECT(dialog1), "pixmap_label",
			       pixmap_label,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pixmap_label);
	gtk_widget_set_sensitive(pixmap_label, FALSE);
	gtk_box_pack_start(GTK_BOX(hbox7), pixmap_label, FALSE, FALSE, 5);

	hbox6 = gtk_hbox_new(FALSE, 0);
	gtk_widget_set_name(hbox6, "hbox6");
	gtk_widget_ref(hbox6);
	g_object_set_data_full(G_OBJECT(dialog1), "hbox6", hbox6,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox6);
	gtk_box_pack_start(GTK_BOX(dialog_vbox1), hbox6, TRUE, FALSE, 5);

	last_label = gtk_label_new(("Last played color :"));
	gtk_widget_set_name(last_label, "last_label");
	gtk_widget_ref(last_label);
	g_object_set_data_full(G_OBJECT(dialog1), "last_label", last_label,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(last_label);
	gtk_box_pack_start(GTK_BOX(hbox6), last_label, FALSE, FALSE, 5);

	last_button = gtk_button_new_with_label("");
	gtk_widget_set_name(last_button, "last_button");
	gtk_widget_ref(last_button);
	g_object_set_data_full(G_OBJECT(dialog1), "last_button",
			       last_button,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(last_button);
	gtk_box_pack_start(GTK_BOX(hbox6), last_button, FALSE, FALSE, 50);

	dialog_action_area1 = GTK_DIALOG(dialog1)->action_area;
	gtk_widget_set_name(dialog_action_area1, "dialog_action_area1");
	g_object_set_data(G_OBJECT(dialog1), "dialog_action_area1",
			  dialog_action_area1);
	gtk_widget_show(dialog_action_area1);
	gtk_container_set_border_width(GTK_CONTAINER(dialog_action_area1),
				       10);

	ok_button = gtk_button_new_with_label(("OK"));
	gtk_widget_set_name(ok_button, "ok_button");
	gtk_widget_ref(ok_button);
	g_object_set_data_full(G_OBJECT(dialog1), "ok_button", ok_button,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(ok_button);
	gtk_box_pack_start(GTK_BOX(dialog_action_area1), ok_button, FALSE,
			   FALSE, 0);

	cancel_button = gtk_button_new_with_label(("Cancel"));
	gtk_widget_set_name(cancel_button, "cancel_button");
	gtk_widget_ref(cancel_button);
	g_object_set_data_full(G_OBJECT(dialog1), "cancel_button",
			       cancel_button,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cancel_button);
	gtk_box_pack_start(GTK_BOX(dialog_action_area1), cancel_button,
			   FALSE, FALSE, 0);

	g_signal_connect(GTK_OBJECT(back_button), "clicked",
			 GTK_SIGNAL_FUNC(on_back_button_clicked), NULL);
	g_signal_connect(GTK_OBJECT(last_button), "clicked",
			 GTK_SIGNAL_FUNC(on_last_button_clicked), NULL);
	g_signal_connect_swapped(GTK_OBJECT(cancel_button), "clicked",
				 GTK_SIGNAL_FUNC(gtk_widget_destroy),
				 GTK_OBJECT(dialog1));

	if (_back_color)
		update_data(back_button, _back_color);
	if (_last_color)
		update_data(last_button, _last_color);

	return dialog1;
}

GtkWidget *create_colorselectiondialog(GdkColor * col)
{
	GtkWidget *colorselectiondialog1;
	GtkWidget *ok_button1;
	GtkWidget *cancel_button1;
	GtkWidget *help_button1;
	gdouble color[3];
	color[0] = col->red / 65535.0;
	color[1] = col->green / 65535.0;
	color[2] = col->blue / 65535.0;

	colorselectiondialog1 =
	    gtk_color_selection_dialog_new(("Select Color"));
	gtk_color_selection_set_color(GTK_COLOR_SELECTION
				      (GTK_COLOR_SELECTION_DIALOG
				       (colorselectiondialog1)->colorsel),
				      color);
	gtk_widget_set_name(colorselectiondialog1,
			    "colorselectiondialog1");
	g_object_set_data(G_OBJECT(colorselectiondialog1),
			  "colorselectiondialog1", colorselectiondialog1);
	gtk_container_set_border_width(GTK_CONTAINER
				       (colorselectiondialog1), 10);

	ok_button1 =
	    GTK_COLOR_SELECTION_DIALOG(colorselectiondialog1)->ok_button;
	gtk_widget_set_name(ok_button1, "ok_button1");
	g_object_set_data(G_OBJECT(colorselectiondialog1), "ok_button1",
			  ok_button1);
	gtk_widget_show(ok_button1);
	GTK_WIDGET_SET_FLAGS(ok_button1, GTK_CAN_DEFAULT);

	cancel_button1 =
	    GTK_COLOR_SELECTION_DIALOG(colorselectiondialog1)->
	    cancel_button;
	gtk_widget_set_name(cancel_button1, "cancel_button1");
	g_object_set_data(G_OBJECT(colorselectiondialog1),
			  "cancel_button1", cancel_button1);
	gtk_widget_show(cancel_button1);
	GTK_WIDGET_SET_FLAGS(cancel_button1, GTK_CAN_DEFAULT);

	help_button1 =
	    GTK_COLOR_SELECTION_DIALOG(colorselectiondialog1)->help_button;
	gtk_widget_destroy(help_button1);

	g_signal_connect_swapped(GTK_OBJECT(cancel_button1), "clicked",
				 GTK_SIGNAL_FUNC(gtk_widget_destroy),
				 GTK_OBJECT(colorselectiondialog1));

	g_signal_connect_data(ok_button1, "clicked",
			      GTK_SIGNAL_FUNC(gtk_widget_destroy),
			      colorselectiondialog1, NULL,
			      G_CONNECT_SWAPPED | G_CONNECT_AFTER);

	return colorselectiondialog1;
}

void on_back_button_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *ok;
	GdkColor *col = g_object_get_data(G_OBJECT(button), "color");
	color_sel = create_colorselectiondialog(col);
	ok = lookup_widget(color_sel, "ok_button1");
	g_signal_connect_object(ok, "clicked",
				GTK_SIGNAL_FUNC(prepare_data),
				"back_button", 0);
	gtk_widget_show_all(color_sel);

}

void prepare_data(GtkButton * button, gchar * data)
{
	GtkWidget *widget;
	GdkColor *color = (GdkColor *) malloc(sizeof(GdkColor));
	gdouble color_val[4];
	widget = lookup_widget(dialog1, data);
	gtk_color_selection_get_color(GTK_COLOR_SELECTION
				      (GTK_COLOR_SELECTION_DIALOG
				       (color_sel)->colorsel), color_val);
	color->red = color_val[0] * 65535;
	color->green = color_val[1] * 65535;
	color->blue = color_val[2] * 65535;
	gdk_colormap_alloc_color(gtk_widget_get_colormap(dialog1),
				 color, TRUE, TRUE);
	update_data(widget, color);
}

void update_data(GtkWidget * widget, GdkColor * color)
{
	GtkStyle *style;
	int a;

	g_object_set_data(G_OBJECT(widget), "color", color);

	style = gtk_style_copy(gtk_widget_get_style(widget));
	for (a = 0; a < 5; a++) {
		style->fg[a] = *color;
		style->bg[a] = *color;
		style->base[a] = *color;
	}
	gtk_widget_set_style(widget, style);
}


void on_last_button_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *ok;
	GdkColor *col = g_object_get_data(G_OBJECT(button), "color");
	color_sel = create_colorselectiondialog(col);
	ok = lookup_widget(color_sel, "ok_button1");
	g_signal_connect_object(ok, "clicked",
				GTK_SIGNAL_FUNC(prepare_data),
				"last_button", 0);
	gtk_widget_show_all(color_sel);
}
