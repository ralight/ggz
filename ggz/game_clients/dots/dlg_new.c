/*
 * File: dlg_new.c
 * Author: Rich Gade
 * Project: GGZ Connect the Dots Client
 * Date: 08/14/2000
 * Desc: Creates the "New Game?" Gtk dialog
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

#include "cb_new.h"
#include "dlg_new.h"
#include "main.h"
#include "support.h"

GtkWidget*
create_dlg_new (void)
{
  GtkWidget *dlg_new;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *lbl_winner;
  GtkWidget *lbl_score;
  GtkWidget *label3;
  GtkWidget *dialog_action_area1;
  GtkWidget *hbox1;
  GtkWidget *new_btn_yes;
  GtkWidget *new_btn_no;

  dlg_new = gtk_dialog_new ();
  gtk_window_set_transient_for(GTK_WINDOW(dlg_new), GTK_WINDOW(main_win));
  gtk_object_set_data (GTK_OBJECT (dlg_new), "dlg_new", dlg_new);
  gtk_window_set_title (GTK_WINDOW (dlg_new), _("Game Over"));
  gtk_window_set_policy (GTK_WINDOW (dlg_new), TRUE, TRUE, FALSE);

  dialog_vbox1 = GTK_DIALOG (dlg_new)->vbox;
  gtk_object_set_data (GTK_OBJECT (dlg_new), "dialog_vbox1", dialog_vbox1);
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_new), "vbox1", vbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, FALSE, FALSE, 0);

  lbl_winner = gtk_label_new (_("You beat NAME"));
  gtk_widget_ref (lbl_winner);
  gtk_object_set_data_full (GTK_OBJECT (dlg_new), "lbl_winner", lbl_winner,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (lbl_winner);
  gtk_box_pack_start (GTK_BOX (vbox1), lbl_winner, FALSE, FALSE, 0);

  lbl_score = gtk_label_new (_("SCR to SCR"));
  gtk_widget_ref (lbl_score);
  gtk_object_set_data_full (GTK_OBJECT (dlg_new), "lbl_score", lbl_score,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (lbl_score);
  gtk_box_pack_start (GTK_BOX (vbox1), lbl_score, FALSE, FALSE, 0);

  label3 = gtk_label_new (_("Play another game?"));
  gtk_widget_ref (label3);
  gtk_object_set_data_full (GTK_OBJECT (dlg_new), "label3", label3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label3);
  gtk_box_pack_start (GTK_BOX (vbox1), label3, FALSE, FALSE, 0);
  gtk_misc_set_padding (GTK_MISC (label3), 0, 6);

  dialog_action_area1 = GTK_DIALOG (dlg_new)->action_area;
  gtk_object_set_data (GTK_OBJECT (dlg_new), "dialog_action_area1", dialog_action_area1);
  gtk_widget_show (dialog_action_area1);
  gtk_container_set_border_width (GTK_CONTAINER (dialog_action_area1), 10);

  hbox1 = gtk_hbox_new (TRUE, 35);
  gtk_widget_ref (hbox1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_new), "hbox1", hbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (dialog_action_area1), hbox1, TRUE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox1), 1);

  new_btn_yes = gtk_button_new_with_label (_("     Yes     "));
  gtk_widget_ref (new_btn_yes);
  gtk_object_set_data_full (GTK_OBJECT (dlg_new), "new_btn_yes", new_btn_yes,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (new_btn_yes);
  gtk_box_pack_start (GTK_BOX (hbox1), new_btn_yes, FALSE, FALSE, 0);

  new_btn_no = gtk_button_new_with_label (_("     No     "));
  gtk_widget_ref (new_btn_no);
  gtk_object_set_data_full (GTK_OBJECT (dlg_new), "new_btn_no", new_btn_no,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (new_btn_no);
  gtk_box_pack_start (GTK_BOX (hbox1), new_btn_no, FALSE, FALSE, 0);

  gtk_signal_connect (GTK_OBJECT (dlg_new), "delete_event",
                      GTK_SIGNAL_FUNC (on_dlg_new_delete_event),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (new_btn_yes), "clicked",
                      GTK_SIGNAL_FUNC (on_new_btn_yes_clicked),
                      NULL);
  gtk_signal_connect_object (GTK_OBJECT (new_btn_yes), "clicked",
                             GTK_SIGNAL_FUNC (gtk_widget_destroy),
                             GTK_OBJECT (dlg_new));
  gtk_signal_connect (GTK_OBJECT (new_btn_no), "clicked",
                      GTK_SIGNAL_FUNC (on_new_btn_no_clicked),
                      NULL);
  gtk_signal_connect_object (GTK_OBJECT (new_btn_no), "clicked",
                             GTK_SIGNAL_FUNC (gtk_widget_destroy),
                             GTK_OBJECT (dlg_new));

  return dlg_new;
}

