/*
 * File: popup.c
 * Author: Ismael Orenstein
 * Project: GGZ Chess game module
 * Date: 09/17/2000
 * Desc: Popup windows (draw game and time option)
 *
 * Copyright (C) 2000 Ismael Orenstein.
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

#include <gtk/gtk.h>
#include "popup.h"
#include "board.h"
#include "support.h"

GtkWidget*
create_draw_dialog (const char *message)
{
  GtkWidget *draw_dialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *draw_message;
  GtkWidget *dialog_action_area1;
  GtkWidget *hbuttonbox1;
  GtkWidget *draw;
  GtkWidget *cancel;

  draw_dialog = gtk_dialog_new ();
  gtk_object_set_data (GTK_OBJECT (draw_dialog), "draw_dialog", draw_dialog);
  gtk_window_set_title (GTK_WINDOW (draw_dialog), _("Do you want do draw the game ?"));
  gtk_window_set_policy (GTK_WINDOW (draw_dialog), TRUE, TRUE, FALSE);

  dialog_vbox1 = GTK_DIALOG (draw_dialog)->vbox;
  gtk_object_set_data (GTK_OBJECT (draw_dialog), "dialog_vbox1", dialog_vbox1);
  gtk_widget_show (dialog_vbox1);

  draw_message = gtk_label_new (_(message));
  gtk_widget_ref (draw_message);
  gtk_object_set_data_full (GTK_OBJECT (draw_dialog), "draw_message", draw_message,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (draw_message);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), draw_message, FALSE, FALSE, 0);

  dialog_action_area1 = GTK_DIALOG (draw_dialog)->action_area;
  gtk_object_set_data (GTK_OBJECT (draw_dialog), "dialog_action_area1", dialog_action_area1);
  gtk_widget_show (dialog_action_area1);
  gtk_container_set_border_width (GTK_CONTAINER (dialog_action_area1), 10);

  hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_ref (hbuttonbox1);
  gtk_object_set_data_full (GTK_OBJECT (draw_dialog), "hbuttonbox1", hbuttonbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbuttonbox1);
  gtk_box_pack_start (GTK_BOX (dialog_action_area1), hbuttonbox1, TRUE, TRUE, 0);

  draw = gtk_button_new_with_label (_("Draw"));
  gtk_widget_ref (draw);
  gtk_object_set_data_full (GTK_OBJECT (draw_dialog), "draw", draw,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (draw);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), draw);
  GTK_WIDGET_SET_FLAGS (draw, GTK_CAN_DEFAULT);

  cancel = gtk_button_new_with_label (_("Don't Draw"));
  gtk_widget_ref (cancel);
  gtk_object_set_data_full (GTK_OBJECT (draw_dialog), "cancel", cancel,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cancel);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), cancel);
  GTK_WIDGET_SET_FLAGS (cancel, GTK_CAN_DEFAULT);

  gtk_signal_connect_object (GTK_OBJECT (cancel), "clicked",
                      GTK_SIGNAL_FUNC (gtk_widget_destroy),
                      GTK_OBJECT(draw_dialog));

  gtk_signal_connect_object_after (GTK_OBJECT (draw), "clicked",
                      GTK_SIGNAL_FUNC (gtk_widget_destroy),
                      GTK_OBJECT(draw_dialog));

  gtk_signal_connect (GTK_OBJECT (draw), "clicked",
                      GTK_SIGNAL_FUNC (board_request_draw),
                      NULL);
  

  return draw_dialog;
}
