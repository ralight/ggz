/*
 * File: dlg_pref.c
 * Author: Rich Gade
 * Project: GGZ Connect the Dots Client
 * Date: 01/22/2001
 * Desc: Create the "Preferences" Gtk dialog
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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "cb_pref.h"
#include "dlg_pref.h"
#include "support.h"
#include "ggzintl.h"

GtkWidget *create_dlg_pref(void)
{
	GtkWidget *dlg_pref;
	GtkWidget *dialog_vbox1;
	GtkWidget *frame1;
	GtkWidget *table1;
	GtkWidget *label5;
	GtkWidget *bg_colorspot;
	GtkWidget *label7;
	GtkWidget *label8;
	GtkWidget *p2_colorspot;
	GtkWidget *p1_colorspot;
	GtkWidget *fg_colorspot;
	GtkWidget *label6;
	GtkWidget *label9;
	GtkWidget *label10;
	GtkWidget *dialog_action_area1;
	GtkWidget *hbox1;
	GtkWidget *pref_button_ok;
	GtkWidget *pref_button_apply;
	GtkWidget *pref_button_cancel;

	dlg_pref = gtk_dialog_new();
	gtk_widget_set_name(dlg_pref, "dlg_pref");
	g_object_set_data(G_OBJECT(dlg_pref), "dlg_pref", dlg_pref);
	gtk_window_set_title(GTK_WINDOW(dlg_pref),
			     _("Dots - Color Preferences"));
	gtk_window_set_policy(GTK_WINDOW(dlg_pref), FALSE, FALSE, FALSE);

	dialog_vbox1 = GTK_DIALOG(dlg_pref)->vbox;
	gtk_widget_set_name(dialog_vbox1, "dialog_vbox1");
	g_object_set_data(G_OBJECT(dlg_pref), "dialog_vbox1",
			  dialog_vbox1);
	gtk_widget_show(dialog_vbox1);

	frame1 = gtk_frame_new(_("Click a colorspot to change"));
	gtk_widget_set_name(frame1, "frame1");
	gtk_widget_ref(frame1);
	g_object_set_data_full(G_OBJECT(dlg_pref), "frame1", frame1,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame1);
	gtk_box_pack_start(GTK_BOX(dialog_vbox1), frame1, TRUE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(frame1), 12);

	table1 = gtk_table_new(2, 5, FALSE);
	gtk_widget_set_name(table1, "table1");
	gtk_widget_ref(table1);
	g_object_set_data_full(G_OBJECT(dlg_pref), "table1", table1,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(table1);
	gtk_container_add(GTK_CONTAINER(frame1), table1);

	label5 = gtk_label_new(_("Foreground Color"));
	gtk_widget_set_name(label5, "label5");
	gtk_widget_ref(label5);
	g_object_set_data_full(G_OBJECT(dlg_pref), "label5", label5,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label5);
	gtk_table_attach(GTK_TABLE(table1), label5, 0, 1, 0, 1,
			 (GtkAttachOptions) (0),
			 (GtkAttachOptions) (0), 0, 0);

	bg_colorspot = gtk_drawing_area_new();
	gtk_widget_set_name(bg_colorspot, "bg_colorspot");
	gtk_widget_ref(bg_colorspot);
	g_object_set_data_full(G_OBJECT(dlg_pref), "bg_colorspot",
			       bg_colorspot,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(bg_colorspot);
	gtk_table_attach(GTK_TABLE(table1), bg_colorspot, 1, 2, 1, 2,
			 (GtkAttachOptions) (0),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_widget_set_usize(bg_colorspot, 16, 16);
	gtk_widget_set_events(bg_colorspot, GDK_BUTTON_PRESS_MASK);

	label7 = gtk_label_new(_("Player One Color"));
	gtk_widget_set_name(label7, "label7");
	gtk_widget_ref(label7);
	g_object_set_data_full(G_OBJECT(dlg_pref), "label7", label7,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label7);
	gtk_table_attach(GTK_TABLE(table1), label7, 3, 4, 0, 1,
			 (GtkAttachOptions) (0),
			 (GtkAttachOptions) (0), 0, 0);

	label8 = gtk_label_new(_("Player Two Color"));
	gtk_widget_set_name(label8, "label8");
	gtk_widget_ref(label8);
	g_object_set_data_full(G_OBJECT(dlg_pref), "label8", label8,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label8);
	gtk_table_attach(GTK_TABLE(table1), label8, 3, 4, 1, 2,
			 (GtkAttachOptions) (0),
			 (GtkAttachOptions) (0), 0, 0);

	p2_colorspot = gtk_drawing_area_new();
	gtk_widget_set_name(p2_colorspot, "p2_colorspot");
	gtk_widget_ref(p2_colorspot);
	g_object_set_data_full(G_OBJECT(dlg_pref), "p2_colorspot",
			       p2_colorspot,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(p2_colorspot);
	gtk_table_attach(GTK_TABLE(table1), p2_colorspot, 4, 5, 1, 2,
			 (GtkAttachOptions) (0),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_widget_set_usize(p2_colorspot, 16, 16);
	gtk_widget_set_events(p2_colorspot, GDK_BUTTON_PRESS_MASK);

	p1_colorspot = gtk_drawing_area_new();
	gtk_widget_set_name(p1_colorspot, "p1_colorspot");
	gtk_widget_ref(p1_colorspot);
	g_object_set_data_full(G_OBJECT(dlg_pref), "p1_colorspot",
			       p1_colorspot,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(p1_colorspot);
	gtk_table_attach(GTK_TABLE(table1), p1_colorspot, 4, 5, 0, 1,
			 (GtkAttachOptions) (0),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_widget_set_usize(p1_colorspot, 16, 16);
	gtk_widget_set_events(p1_colorspot, GDK_BUTTON_PRESS_MASK);

	fg_colorspot = gtk_drawing_area_new();
	gtk_widget_set_name(fg_colorspot, "fg_colorspot");
	gtk_widget_ref(fg_colorspot);
	g_object_set_data_full(G_OBJECT(dlg_pref), "fg_colorspot",
			       fg_colorspot,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(fg_colorspot);
	gtk_table_attach(GTK_TABLE(table1), fg_colorspot, 1, 2, 0, 1,
			 (GtkAttachOptions) (0),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_widget_set_usize(fg_colorspot, 16, 16);
	gtk_widget_set_events(fg_colorspot, GDK_BUTTON_PRESS_MASK);

	label6 = gtk_label_new(_("Background Color"));
	gtk_widget_set_name(label6, "label6");
	gtk_widget_ref(label6);
	g_object_set_data_full(G_OBJECT(dlg_pref), "label6", label6,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label6);
	gtk_table_attach(GTK_TABLE(table1), label6, 0, 1, 1, 2,
			 (GtkAttachOptions) (0),
			 (GtkAttachOptions) (0), 0, 0);

	label9 = gtk_label_new("    ");
	gtk_widget_set_name(label9, "label9");
	gtk_widget_ref(label9);
	g_object_set_data_full(G_OBJECT(dlg_pref), "label9", label9,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label9);
	gtk_table_attach(GTK_TABLE(table1), label9, 2, 3, 0, 1,
			 (GtkAttachOptions) (0),
			 (GtkAttachOptions) (0), 0, 0);

	label10 = gtk_label_new("    ");
	gtk_widget_set_name(label10, "label10");
	gtk_widget_ref(label10);
	g_object_set_data_full(G_OBJECT(dlg_pref), "label10", label10,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label10);
	gtk_table_attach(GTK_TABLE(table1), label10, 2, 3, 1, 2,
			 (GtkAttachOptions) (0),
			 (GtkAttachOptions) (0), 0, 0);

	dialog_action_area1 = GTK_DIALOG(dlg_pref)->action_area;
	gtk_widget_set_name(dialog_action_area1, "dialog_action_area1");
	g_object_set_data(G_OBJECT(dlg_pref), "dialog_action_area1",
			  dialog_action_area1);
	gtk_widget_show(dialog_action_area1);
	gtk_container_set_border_width(GTK_CONTAINER(dialog_action_area1),
				       10);

	hbox1 = gtk_hbox_new(TRUE, 0);
	gtk_widget_set_name(hbox1, "hbox1");
	gtk_widget_ref(hbox1);
	g_object_set_data_full(G_OBJECT(dlg_pref), "hbox1", hbox1,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox1);
	gtk_box_pack_start(GTK_BOX(dialog_action_area1), hbox1, TRUE, TRUE,
			   0);

	pref_button_ok = gtk_button_new_with_label(_("OK"));
	gtk_widget_set_name(pref_button_ok, "pref_button_ok");
	gtk_widget_ref(pref_button_ok);
	g_object_set_data_full(G_OBJECT(dlg_pref), "pref_button_ok",
			       pref_button_ok,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pref_button_ok);
	gtk_box_pack_start(GTK_BOX(hbox1), pref_button_ok, FALSE, FALSE,
			   0);
	gtk_widget_set_usize(pref_button_ok, 64, -2);

	pref_button_apply = gtk_button_new_with_label(_("Apply"));
	gtk_widget_set_name(pref_button_apply, "pref_button_apply");
	gtk_widget_ref(pref_button_apply);
	g_object_set_data_full(G_OBJECT(dlg_pref), "pref_button_apply",
			       pref_button_apply,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pref_button_apply);
	gtk_box_pack_start(GTK_BOX(hbox1), pref_button_apply, FALSE, FALSE,
			   0);
	gtk_widget_set_usize(pref_button_apply, 64, -2);

	pref_button_cancel = gtk_button_new_with_label(_("Cancel"));
	gtk_widget_set_name(pref_button_cancel, "pref_button_cancel");
	gtk_widget_ref(pref_button_cancel);
	g_object_set_data_full(G_OBJECT(dlg_pref), "pref_button_cancel",
			       pref_button_cancel,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pref_button_cancel);
	gtk_box_pack_start(GTK_BOX(hbox1), pref_button_cancel, FALSE,
			   FALSE, 0);
	gtk_widget_set_usize(pref_button_cancel, 64, -2);

	g_signal_connect(GTK_OBJECT(dlg_pref), "expose_event",
			 GTK_SIGNAL_FUNC(on_dlg_pref_expose_event), NULL);
	g_signal_connect(GTK_OBJECT(dlg_pref), "delete_event",
			 GTK_SIGNAL_FUNC(on_dlg_pref_delete_event), NULL);
	g_signal_connect(GTK_OBJECT(bg_colorspot), "button_press_event",
			 GTK_SIGNAL_FUNC
			 (on_bg_colorspot_button_press_event), NULL);
	g_signal_connect(GTK_OBJECT(p2_colorspot), "button_press_event",
			 GTK_SIGNAL_FUNC
			 (on_p2_colorspot_button_press_event), NULL);
	g_signal_connect(GTK_OBJECT(p1_colorspot), "button_press_event",
			 GTK_SIGNAL_FUNC
			 (on_p1_colorspot_button_press_event), NULL);
	g_signal_connect(GTK_OBJECT(fg_colorspot), "button_press_event",
			 GTK_SIGNAL_FUNC
			 (on_fg_colorspot_button_press_event), NULL);
	g_signal_connect(GTK_OBJECT(pref_button_ok), "clicked",
			 GTK_SIGNAL_FUNC(on_pref_button_ok_clicked), NULL);
	g_signal_connect(GTK_OBJECT(pref_button_apply), "clicked",
			 GTK_SIGNAL_FUNC(on_pref_button_apply_clicked),
			 NULL);
	g_signal_connect(GTK_OBJECT(pref_button_cancel), "clicked",
			 GTK_SIGNAL_FUNC(on_pref_button_cancel_clicked),
			 NULL);

	return dlg_pref;
}
