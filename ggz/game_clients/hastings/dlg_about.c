/*
 * File: dlg_about.c
 * Author: Josef Spillner
 * Original Author: Rich Gade
 * Project: GGZ Hastings1066 Client
 * Date: 09/10/2000
 * Desc: Create the "About" Gtk dialog
 *
 * Copyright (C) 2000 - 2002 Josef Spillnr
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

/* Configration file */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

/* System includes */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

/* Gtk+ includes */
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

/* Hastings includes */
#include "dlg_about.h"
#include "support.h"
#include "ggzintl.h"

/* Create and show the about dialog */
GtkWidget* create_dlg_about (void)
{
  GtkWidget *dlg_about;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *label3;
  GtkWidget *label4;
  GtkWidget *dialog_action_area1;
  GtkWidget *ok_button;

  dlg_about = gtk_dialog_new ();
  gtk_object_set_data (GTK_OBJECT (dlg_about), "dlg_about", dlg_about);
  gtk_window_set_title (GTK_WINDOW (dlg_about), "About Hastings1066");
  GTK_WINDOW (dlg_about)->type = GTK_WINDOW_DIALOG;
  gtk_window_set_policy (GTK_WINDOW (dlg_about), TRUE, TRUE, FALSE);

  dialog_vbox1 = GTK_DIALOG (dlg_about)->vbox;
  gtk_object_set_data (GTK_OBJECT (dlg_about), "dialog_vbox1", dialog_vbox1);
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_about), "vbox1", vbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  label3 = gtk_label_new ("GGZ Gaming Zone\nHastings1066 Version 0.0.5pre");
  gtk_widget_ref (label3);
  gtk_object_set_data_full (GTK_OBJECT (dlg_about), "label3", label3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label3);
  gtk_box_pack_start (GTK_BOX (vbox1), label3, FALSE, FALSE, 0);

  label4 = gtk_label_new (_("Authors:\n"
	"        Gtk+ Client:\n"
	"            Josef Spillner        <dr_maux@users.sourceforge.net>\n\n"
	"        Game Server:\n"
	"            Josef Spillner        <dr_maux@users.sourceforge.net>\n\n"
	"Website:\n"
	"        http://ggz.sourceforge.net/games/hastings/"));
  gtk_widget_ref (label4);
  gtk_object_set_data_full (GTK_OBJECT (dlg_about), "label4", label4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label4);
  gtk_box_pack_start (GTK_BOX (vbox1), label4, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label4), GTK_JUSTIFY_LEFT);

  dialog_action_area1 = GTK_DIALOG (dlg_about)->action_area;
  gtk_object_set_data (GTK_OBJECT (dlg_about), "dialog_action_area1", dialog_action_area1);
  gtk_widget_show (dialog_action_area1);
  gtk_container_set_border_width (GTK_CONTAINER (dialog_action_area1), 10);

  ok_button = gtk_button_new_with_label ("OK");
  gtk_widget_ref (ok_button);
  gtk_object_set_data_full (GTK_OBJECT (dlg_about), "ok_button", ok_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (ok_button);
  gtk_box_pack_start (GTK_BOX (dialog_action_area1), ok_button, FALSE, FALSE, 0);
  gtk_widget_set_usize (ok_button, 64, -2);

  gtk_signal_connect_object (GTK_OBJECT (dlg_about), "delete_event",
                             GTK_SIGNAL_FUNC (gtk_widget_destroy),
                             GTK_OBJECT (dlg_about));
  gtk_signal_connect_object (GTK_OBJECT (ok_button), "clicked",
                             GTK_SIGNAL_FUNC (gtk_widget_destroy),
                             GTK_OBJECT (dlg_about));
  return dlg_about;
}

