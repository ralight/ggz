/*
 * File: dlg_opt.c
 * Author: Rich Gade
 * Project: GGZ Connect the Dots Client
 * Date: 08/14/2000
 * Desc: Creates the "Game Options" Gtk dialog
 *
 * Copyright (C) 2000 Brent Hendricks.
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

#include "cb_opt.h"
#include "dlg_opt.h"
#include "main.h"
#include "support.h"

GtkWidget*
create_dlg_opt (void)
{
  GtkWidget *dlg_opt;
  GtkWidget *vbox1;
  GtkWidget *frame1;
  GtkWidget *table1;
  GtkWidget *label2;
  GtkObject *opt_spin_y_adj;
  GtkWidget *opt_spin_y;
  GtkObject *opt_spin_x_adj;
  GtkWidget *opt_spin_x;
  GtkWidget *label1;
  GtkWidget *hbox2;
  GtkWidget *opt_btn_ok;

  dlg_opt = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_transient_for(GTK_WINDOW(dlg_opt), GTK_WINDOW(main_win));
  g_object_set_data(G_OBJECT (dlg_opt), "dlg_opt", dlg_opt);
  gtk_window_set_title (GTK_WINDOW (dlg_opt), _("Game Options"));

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox1);
  g_object_set_data_full(G_OBJECT (dlg_opt), "vbox1", vbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (dlg_opt), vbox1);
  gtk_container_set_border_width (GTK_CONTAINER (vbox1), 1);

  frame1 = gtk_frame_new (_("Number of squares "));
  gtk_widget_ref (frame1);
  g_object_set_data_full(G_OBJECT (dlg_opt), "frame1", frame1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame1);
  gtk_box_pack_start (GTK_BOX (vbox1), frame1, TRUE, TRUE, 2);

  table1 = gtk_table_new (2, 2, FALSE);
  gtk_widget_ref (table1);
  g_object_set_data_full(G_OBJECT (dlg_opt), "table1", table1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (table1);
  gtk_container_add (GTK_CONTAINER (frame1), table1);

  label2 = gtk_label_new (_("Vertical"));
  gtk_widget_ref (label2);
  g_object_set_data_full(G_OBJECT (dlg_opt), "label2", label2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label2);
  gtk_table_attach (GTK_TABLE (table1), label2, 0, 1, 1, 2,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  opt_spin_y_adj = gtk_adjustment_new (5, 2, 24, 1, 10, 10);
  opt_spin_y = gtk_spin_button_new (GTK_ADJUSTMENT (opt_spin_y_adj), 1, 0);
  gtk_widget_ref (opt_spin_y);
  g_object_set_data_full(G_OBJECT (dlg_opt), "opt_spin_y", opt_spin_y,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (opt_spin_y);
  gtk_table_attach (GTK_TABLE (table1), opt_spin_y, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (opt_spin_y), TRUE);

  opt_spin_x_adj = gtk_adjustment_new (5, 2, 24, 1, 10, 10);
  opt_spin_x = gtk_spin_button_new (GTK_ADJUSTMENT (opt_spin_x_adj), 1, 0);
  gtk_widget_ref (opt_spin_x);
  g_object_set_data_full(G_OBJECT (dlg_opt), "opt_spin_x", opt_spin_x,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (opt_spin_x);
  gtk_table_attach (GTK_TABLE (table1), opt_spin_x, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (opt_spin_x), TRUE);

  label1 = gtk_label_new (_("Horizontal"));
  gtk_widget_ref (label1);
  g_object_set_data_full(G_OBJECT (dlg_opt), "label1", label1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label1);
  gtk_table_attach (GTK_TABLE (table1), label1, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND),
                    (GtkAttachOptions) (0), 0, 0);

  hbox2 = gtk_hbox_new (TRUE, 0);
  gtk_widget_ref (hbox2);
  g_object_set_data_full(G_OBJECT (dlg_opt), "hbox2", hbox2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox2);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox2, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox2), 3);

  opt_btn_ok = gtk_button_new_with_label ("     OK     ");
  gtk_widget_ref (opt_btn_ok);
  g_object_set_data_full(G_OBJECT (dlg_opt), "opt_btn_ok", opt_btn_ok,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (opt_btn_ok);
  gtk_box_pack_start (GTK_BOX (hbox2), opt_btn_ok, FALSE, FALSE, 0);

  g_signal_connect (GTK_OBJECT (dlg_opt), "delete_event",
                      GTK_SIGNAL_FUNC (on_dlg_opt_delete_event),
                      NULL);
  g_signal_connect (GTK_OBJECT (opt_btn_ok), "clicked",
                      GTK_SIGNAL_FUNC (on_opt_btn_ok_clicked),
                      NULL);
  g_signal_connect_swapped (GTK_OBJECT (opt_btn_ok), "clicked",
                             GTK_SIGNAL_FUNC (gtk_widget_destroy),
                             GTK_OBJECT (dlg_opt));

  return dlg_opt;
}

