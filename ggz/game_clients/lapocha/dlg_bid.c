/*
 * File: dlg_bid.c
 * Author: Rich Gade
 * Project: GGZ La Pocha Client
 * Date: 08/14/2000
 * Desc: Creates the bid request Gtk fixed dialog
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

#include "cb_bid.h"
#include "dlg_bid.h"
#include "support.h"
#include "main.h"

static GtkWidget *main_fixed;
GtkWidget *dlg_bid_fixed;
GtkWidget *dlg_bid_spin;

void dlg_bid_setup(GtkWidget *widget)
{
  GtkWidget *bid_frame1;
  GtkWidget *bid_vbox1;
  GtkWidget *bid_hbox1;
  GtkWidget *bid_label1;
  GtkWidget *dlg_bid_submit;
  GtkObject *dlg_bid_spin_adj;

  main_fixed = widget;

  dlg_bid_fixed = gtk_fixed_new ();
  gtk_widget_set_name (dlg_bid_fixed, "dlg_bid_fixed");
  gtk_widget_ref (dlg_bid_fixed);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "dlg_bid_fixed", dlg_bid_fixed,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_fixed_put(GTK_FIXED(main_fixed), dlg_bid_fixed,
		(main_fixed->allocation.width/2)-50,
		(main_fixed->allocation.height/2)-50);
  /*gtk_widget_show (dlg_bid_fixed);*/
  gtk_widget_set_usize (dlg_bid_fixed, 100, 100);

  bid_frame1 = gtk_frame_new ("Enter Your Bid");
  gtk_widget_set_name (bid_frame1, "bid_frame1");
  gtk_widget_ref (bid_frame1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "bid_frame1", bid_frame1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (bid_frame1);
  gtk_fixed_put (GTK_FIXED (dlg_bid_fixed), bid_frame1, 0, 0);
  gtk_widget_set_uposition (bid_frame1, 0, 0);
  gtk_widget_set_usize (bid_frame1, 100, 100);

  bid_vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_set_name (bid_vbox1, "bid_vbox1");
  gtk_widget_ref (bid_vbox1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "bid_vbox1", bid_vbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (bid_vbox1);
  gtk_container_add (GTK_CONTAINER (bid_frame1), bid_vbox1);

  bid_hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_set_name (bid_hbox1, "bid_hbox1");
  gtk_widget_ref (bid_hbox1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "bid_hbox1", bid_hbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (bid_hbox1);
  gtk_box_pack_start (GTK_BOX (bid_vbox1), bid_hbox1, TRUE, TRUE, 5);

  bid_label1 = gtk_label_new (" Your Bid ");
  gtk_widget_set_name (bid_label1, "bid_label1");
  gtk_widget_ref (bid_label1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "bid_label1", bid_label1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (bid_label1);
  gtk_box_pack_start (GTK_BOX (bid_hbox1), bid_label1, FALSE, FALSE, 0);

  dlg_bid_spin_adj = gtk_adjustment_new (0, 0, 10, 1, 10, 10);
  dlg_bid_spin = gtk_spin_button_new (GTK_ADJUSTMENT (dlg_bid_spin_adj), 1, 0);
  gtk_widget_set_name (dlg_bid_spin, "dlg_bid_spin");
  gtk_widget_ref (dlg_bid_spin);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "dlg_bid_spin", dlg_bid_spin,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (dlg_bid_spin);
  gtk_box_pack_start (GTK_BOX (bid_hbox1), dlg_bid_spin, TRUE, TRUE, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (dlg_bid_spin), TRUE);

  dlg_bid_submit = gtk_button_new_with_label ("Submit Bid");
  gtk_widget_set_name (dlg_bid_submit, "dlg_bid_submit");
  gtk_widget_ref (dlg_bid_submit);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "dlg_bid_submit", dlg_bid_submit,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (dlg_bid_submit);
  gtk_box_pack_start (GTK_BOX (bid_vbox1), dlg_bid_submit, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (dlg_bid_submit), 5);

  gtk_signal_connect (GTK_OBJECT (dlg_bid_submit), "clicked",
                      GTK_SIGNAL_FUNC (on_dlg_bid_submit_clicked),
                      NULL);
}


void dlg_bid_display(int max_bid)
{
	GtkAdjustment *adj;

	adj = gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(dlg_bid_spin));
	adj->upper = max_bid;
	gtk_spin_button_set_adjustment(GTK_SPIN_BUTTON(dlg_bid_spin), adj);
	gtk_widget_show(dlg_bid_fixed);
}
