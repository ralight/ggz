/*
 * File: main_dlg.c.c
 * Author: Rich Gade
 * Project: GGZ Chinese Checkers Client
 * Desc: Main dialog code
 * $Id: main_dlg.c 6293 2004-11-07 05:51:47Z jdorje $
 *
 * Copyright (C) 2001 Richard Gade.
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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "dlg_players.h"
#include "ggzintl.h"
#include "menus.h"

#include "main_cb.h"
#include "main_dlg.h"
#include "support.h"

static GtkWidget *create_menus(GtkWidget * window)
{
	GtkItemFactoryEntry items[] = {
		TABLE_MENU,
		{_("/_Options"), NULL, NULL, 0, "<Branch>"},
		{_("/Options/_Preferences"), "<ctrl>P",
		 on_preferences_menu_activate, 0, NULL},
		HELP_MENU
	};

	GtkWidget *menubar;

	menubar = ggz_create_menus(window,
				   items,
				   sizeof(items) / sizeof(items[0]));

	set_menu_sensitive(TABLE_SYNC, FALSE);

	return menubar;
}

GtkWidget *create_dlg_main(void)
{
	GtkWidget *dlg_main;
	GtkWidget *vbox1;
	GtkWidget *menubar;
	GtkWidget *hbox3;
	GtkWidget *p3_label;
	GtkWidget *p4_label;
	GtkWidget *p5_label;
	GtkWidget *draw_area;
	GtkWidget *hbox2;
	GtkWidget *p2_label;
	GtkWidget *p1_label;
	GtkWidget *p6_label;
	GtkWidget *statusbar1;

	dlg_main = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_name(dlg_main, "dlg_main");
	g_object_set_data(G_OBJECT(dlg_main), "dlg_main", dlg_main);
	gtk_window_set_title(GTK_WINDOW(dlg_main),
			     "Chinese Checkers for GGZ");
	//  gtk_window_set_policy (GTK_WINDOW (dlg_main), FALSE, FALSE, FALSE);

	vbox1 = gtk_vbox_new(FALSE, 0);
	gtk_widget_set_name(vbox1, "vbox1");
	gtk_widget_ref(vbox1);
	g_object_set_data_full(G_OBJECT(dlg_main), "vbox1", vbox1,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox1);
	gtk_container_add(GTK_CONTAINER(dlg_main), vbox1);

	menubar = create_menus(dlg_main);
	gtk_widget_set_name(menubar, "menubar");
	gtk_widget_ref(menubar);
	g_object_set_data_full(G_OBJECT(dlg_main), "menubar", menubar,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(menubar);
	gtk_box_pack_start(GTK_BOX(vbox1), menubar, FALSE, FALSE, 0);

	hbox3 = gtk_hbox_new(TRUE, 0);
	gtk_widget_set_name(hbox3, "hbox3");
	gtk_widget_ref(hbox3);
	g_object_set_data_full(G_OBJECT(dlg_main), "hbox3", hbox3,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox3);
	gtk_box_pack_start(GTK_BOX(vbox1), hbox3, FALSE, FALSE, 0);

	p3_label = gtk_label_new("");
	gtk_widget_set_name(p3_label, "p3_label");
	gtk_widget_ref(p3_label);
	g_object_set_data_full(G_OBJECT(dlg_main), "p3_label", p3_label,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(p3_label);
	gtk_box_pack_start(GTK_BOX(hbox3), p3_label, TRUE, TRUE, 0);

	p4_label = gtk_label_new("");
	gtk_widget_set_name(p4_label, "p4_label");
	gtk_widget_ref(p4_label);
	g_object_set_data_full(G_OBJECT(dlg_main), "p4_label", p4_label,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(p4_label);
	gtk_box_pack_start(GTK_BOX(hbox3), p4_label, TRUE, TRUE, 0);

	p5_label = gtk_label_new("");
	gtk_widget_set_name(p5_label, "p5_label");
	gtk_widget_ref(p5_label);
	g_object_set_data_full(G_OBJECT(dlg_main), "p5_label", p5_label,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(p5_label);
	gtk_box_pack_start(GTK_BOX(hbox3), p5_label, TRUE, TRUE, 0);

	draw_area = gtk_drawing_area_new();
	gtk_widget_set_name(draw_area, "draw_area");
	gtk_widget_ref(draw_area);
	g_object_set_data_full(G_OBJECT(dlg_main), "draw_area", draw_area,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(draw_area);
	gtk_box_pack_start(GTK_BOX(vbox1), draw_area, TRUE, TRUE, 0);
	gtk_widget_set_size_request(draw_area, 400, 400);
	gtk_widget_set_events(draw_area,
			      GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK);

	hbox2 = gtk_hbox_new(TRUE, 0);
	gtk_widget_set_name(hbox2, "hbox2");
	gtk_widget_ref(hbox2);
	g_object_set_data_full(G_OBJECT(dlg_main), "hbox2", hbox2,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox2);
	gtk_box_pack_start(GTK_BOX(vbox1), hbox2, FALSE, TRUE, 0);

	p2_label = gtk_label_new("");
	gtk_widget_set_name(p2_label, "p2_label");
	gtk_widget_ref(p2_label);
	g_object_set_data_full(G_OBJECT(dlg_main), "p2_label", p2_label,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(p2_label);
	gtk_box_pack_start(GTK_BOX(hbox2), p2_label, TRUE, TRUE, 0);

	p1_label = gtk_label_new("");
	gtk_widget_set_name(p1_label, "p1_label");
	gtk_widget_ref(p1_label);
	g_object_set_data_full(G_OBJECT(dlg_main), "p1_label", p1_label,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(p1_label);
	gtk_box_pack_start(GTK_BOX(hbox2), p1_label, TRUE, TRUE, 0);

	p6_label = gtk_label_new("");
	gtk_widget_set_name(p6_label, "p6_label");
	gtk_widget_ref(p6_label);
	g_object_set_data_full(G_OBJECT(dlg_main), "p6_label", p6_label,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(p6_label);
	gtk_box_pack_start(GTK_BOX(hbox2), p6_label, TRUE, TRUE, 0);

	statusbar1 = gtk_statusbar_new();
	gtk_widget_set_name(statusbar1, "statusbar1");
	gtk_widget_ref(statusbar1);
	g_object_set_data_full(G_OBJECT(dlg_main), "statusbar1",
			       statusbar1,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(statusbar1);
	gtk_box_pack_start(GTK_BOX(vbox1), statusbar1, FALSE, FALSE, 0);

	g_signal_connect(GTK_OBJECT(dlg_main), "delete_event",
			 GTK_SIGNAL_FUNC(on_dlg_main_delete_event), NULL);
	g_signal_connect(GTK_OBJECT(draw_area), "expose_event",
			 GTK_SIGNAL_FUNC(on_draw_area_expose_event), NULL);
	g_signal_connect(GTK_OBJECT(draw_area), "configure_event",
			 GTK_SIGNAL_FUNC(on_draw_area_configure_event),
			 NULL);
	g_signal_connect(GTK_OBJECT(draw_area), "button_press_event",
			 GTK_SIGNAL_FUNC(on_draw_area_button_press_event),
			 NULL);

	return dlg_main;
}
