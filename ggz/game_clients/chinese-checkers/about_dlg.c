/*
 * File: about_dlg.c
 * Author: Rich Gade
 * Project: GGZ Chinese Checkers Client
 * Date: 01/13/2001
 * Desc: Create the "Help/About" dialog
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

#include "ggzintl.h"

#include "about_dlg.h"
#include "support.h"

GtkWidget*
create_dlg_about (void)
{
  GtkWidget *dlg_about;
  GtkWidget *dialog_vbox1;
  GtkWidget *label1;
  GtkWidget *dialog_action_area1;
  GtkWidget *ok_button;

  dlg_about = gtk_dialog_new ();
  gtk_widget_set_name (dlg_about, "dlg_about");
  gtk_object_set_data (GTK_OBJECT (dlg_about), "dlg_about", dlg_about);
  gtk_window_set_title (GTK_WINDOW (dlg_about), _("About Chinese Checkers for GGZ"));
  gtk_window_set_policy (GTK_WINDOW (dlg_about), FALSE, FALSE, FALSE);

  dialog_vbox1 = GTK_DIALOG (dlg_about)->vbox;
  gtk_widget_set_name (dialog_vbox1, "dialog_vbox1");
  gtk_object_set_data (GTK_OBJECT (dlg_about), "dialog_vbox1", dialog_vbox1);
  gtk_widget_show (dialog_vbox1);

  label1 = gtk_label_new (_("\nChinese Checkers for GGZ, version 0.0.1\n"
                            "is part of the GGZ Gaming Zone project.\n"
                            "Visit us at http://ggz.sourceforge.net/\n\n"
                            " Copyright (C) 2000, 2001 Richard Gade and Brent Hendricks \n"));
  gtk_widget_set_name (label1, "label1");
  gtk_widget_ref (label1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_about), "label1", label1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), label1, FALSE, FALSE, 0);

  dialog_action_area1 = GTK_DIALOG (dlg_about)->action_area;
  gtk_widget_set_name (dialog_action_area1, "dialog_action_area1");
  gtk_object_set_data (GTK_OBJECT (dlg_about), "dialog_action_area1", dialog_action_area1);
  gtk_widget_show (dialog_action_area1);
  gtk_container_set_border_width (GTK_CONTAINER (dialog_action_area1), 10);

  ok_button = gtk_button_new_with_label (_("OK"));
  gtk_widget_set_name (ok_button, "ok_button");
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

