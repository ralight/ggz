/*
 * File: dlg_about.c
 * Author: Brent Hendricks
 * Project: GGZ Client
 * Date: 4/01/00
 *
 * GGZ Client About window 
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

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "dlg_about.h"
#include "support.h"

/* Global for this dialog */
GtkWidget* dlg_about;

/* Local Functions */
static void dlg_about_realize(GtkWidget* widget, gpointer data);


static void dlg_about_realize(GtkWidget* widget, gpointer data)
{
	GtkWidget *tmp;
	gchar* title;

	title = g_strdup_printf(_("GGZ Gaming Zone Client version %s"), 
				VERSION);
	tmp = lookup_widget(dlg_about, "title_label");
	gtk_label_set_text(GTK_LABEL(tmp), title);
	g_free(title);
}


GtkWidget*
create_dlg_about (void)
{
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *title_label;
  GtkWidget *label4;
  GtkWidget *label2;
  GtkWidget *dialog_action_area1;
  GtkWidget *button_box;
  GtkWidget *ok_button;

  dlg_about = gtk_dialog_new ();
  gtk_object_set_data (GTK_OBJECT (dlg_about), "dlg_about", dlg_about);
  gtk_window_set_title (GTK_WINDOW (dlg_about), _("About"));
  gtk_window_set_policy (GTK_WINDOW (dlg_about), FALSE, FALSE, FALSE);

  dialog_vbox1 = GTK_DIALOG (dlg_about)->vbox;
  gtk_object_set_data (GTK_OBJECT (dlg_about), "dialog_vbox1", dialog_vbox1);
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_about), "vbox1", vbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (vbox1), 10);

  title_label = gtk_label_new ("");
  gtk_widget_ref (title_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_about), "title_label", title_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (title_label);
  gtk_box_pack_start (GTK_BOX (vbox1), title_label, FALSE, FALSE, 8);

  label4 = gtk_label_new (_("(C) 1999, 2000"));
  gtk_widget_ref (label4);
  gtk_object_set_data_full (GTK_OBJECT (dlg_about), "label4", label4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label4);
  gtk_box_pack_start (GTK_BOX (vbox1), label4, FALSE, FALSE, 0);

  label2 = gtk_label_new (_("Authors:\n\t\tGTK+ Version\n\t\t\t\t\t\t\t\tBrian Cox          (bcox@users.sourceforge.net)\n\t\t\t\t\t\t\t\tRich Gade\t\t\t\t\t\t\t\t\t(rgade@users.sourceforge.net)\n\t\t\t\t\t\t\t\tBrent Hendricks\t (bmh@users.sourceforge.net)\n\t\t\t\t\t\t\t\tJustin Zaun\t\t\t\t\t\t\t(jzaun@users.sourceforge.net)\n\n\t\tWindows Version\n\t\t\t\t\t\t\t\tDoug Hudson\t\t\t(djh@users.sourceforge.net)\n\nWebsite:\n\t\t\t\t\t\t\t\thttp://ggz.sourceforge.net\n"));
  gtk_widget_ref (label2);
  gtk_object_set_data_full (GTK_OBJECT (dlg_about), "label2", label2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label2);
  gtk_box_pack_start (GTK_BOX (vbox1), label2, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label2), 0.25, 5.21541e-08);

  dialog_action_area1 = GTK_DIALOG (dlg_about)->action_area;
  gtk_object_set_data (GTK_OBJECT (dlg_about), "dialog_action_area1", dialog_action_area1);
  gtk_widget_show (dialog_action_area1);
  gtk_container_set_border_width (GTK_CONTAINER (dialog_action_area1), 10);

  button_box = gtk_hbutton_box_new ();
  gtk_widget_ref (button_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_about), "button_box", button_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button_box);
  gtk_box_pack_start (GTK_BOX (dialog_action_area1), button_box, TRUE, TRUE, 0);

  ok_button = gtk_button_new_with_label (_("OK"));
  gtk_widget_ref (ok_button);
  gtk_object_set_data_full (GTK_OBJECT (dlg_about), "ok_button", ok_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (ok_button);
  gtk_container_add (GTK_CONTAINER (button_box), ok_button);
  GTK_WIDGET_SET_FLAGS (ok_button, GTK_CAN_DEFAULT);

  gtk_signal_connect (GTK_OBJECT (dlg_about), "realize",
                      GTK_SIGNAL_FUNC (dlg_about_realize),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (dlg_about), "destroy",
                      GTK_SIGNAL_FUNC (gtk_widget_destroyed),
                      &dlg_about);
  gtk_signal_connect_object (GTK_OBJECT (ok_button), "clicked",
                             GTK_SIGNAL_FUNC (gtk_widget_destroy),
                             GTK_OBJECT (dlg_about));

  return dlg_about;
}

