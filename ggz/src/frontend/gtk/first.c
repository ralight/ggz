/*
 * File: first.c
 * Author: Justin Zaun
 * Project: GGZ GTK Client
 *
 * This is the main program body for the GGZ client
 *
 * Copyright (C) 2000, 2001 Justin Zaun.
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

/*
 * Dialof Description
 *
 * Displayes information about the authors and the application.
 */

#include <config.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "client.h"
#include "first.h"
#include "ggz.h"
#include "ggzcore.h"
#include "login.h"
#include "server.h"
#include "support.h"

extern GtkWidget *win_main;

static GtkWidget *first_dialog;
static GtkWidget* create_dlg_first(void);
static void first_button_yes_activate(GtkButton *button, gpointer data);
static void first_button_no_activate(GtkButton *button, gpointer data);

/* first_create_or_raise() - Displays the dialog or raises the
 *                           current dialog
 *
 * Recieves:
 *
 * Returns:   
 */

void first_create_or_raise(void)
{
	ggzcore_conf_write_int("INIT", "FIRST", 1);
	ggzcore_conf_write_string("INIT", "VERSION", VERSION);
	if (!first_dialog) {
		first_dialog = create_dlg_first();
		gtk_widget_show(first_dialog);
	} else {
		gdk_window_show(first_dialog->window);
		gdk_window_raise(first_dialog->window);
	}

}

static void first_button_yes_activate(GtkButton *button, gpointer data)
{
	char *profiles, *curprofiles;
	int count;

	/* Morat.net */
	ggzcore_conf_write_string("Morat.net", "Host", 
		ggzcore_conf_read_string("Morat.net", "Host",
					 "ggz.morat.net"));
	ggzcore_conf_write_string("Morat.net", "Login", 
		ggzcore_conf_read_string("Morat.net", "Login",
					 "Guest"));
	ggzcore_conf_write_int("Morat.net", "Port",
		ggzcore_conf_read_int("Morat.net", "Port",
					 5688));
	ggzcore_conf_write_int("Morat.net", "Type",
		ggzcore_conf_read_int("Morat.net", "Type",
					 1));
	/* Justin's Server */
	ggzcore_conf_write_string("CVS Developer Server", "Host",
		ggzcore_conf_read_string("CVS Developer Server", "Host",
					 "jzaun.com"));
	ggzcore_conf_write_string("CVS Developer Server", "Login",
		ggzcore_conf_read_string("CVS Developer Server", "Login",
					 "Guest"));
	ggzcore_conf_write_int("CVS Developer Server", "Port",
		ggzcore_conf_read_int("CVS Developer Server", "Port",
					 5688));
	ggzcore_conf_write_int("CVS Developer Server", "Type",
		ggzcore_conf_read_int("CVS Developer Server", "Type",
					 1));

	/* Write-out list of servers */
	curprofiles = ggzcore_conf_read_string("Servers", "ProfileList", "");
	profiles = g_strdup_printf("%s Morat.net CVS\\ Developer\\ Server", curprofiles);
	ggzcore_conf_write_string("Servers", "ProfileList", profiles);

	ggzcore_conf_commit();
	gtk_widget_destroy(first_dialog);
	server_profiles_load();
	win_main = create_win_main();
	ggz_sensitivity_init();
	gtk_widget_show(win_main);
	login_create_or_raise();
}


static void first_button_no_activate(GtkButton *button, gpointer data)
{
	ggzcore_conf_commit();
	gtk_widget_destroy(first_dialog);
	win_main = create_win_main();
	ggz_sensitivity_init();
	gtk_widget_show(win_main);
	login_create_or_raise();
}



GtkWidget*
create_dlg_first (void)
{
  GtkWidget *dlg_first;
  GtkWidget *vbox3;
  GtkWidget *label5;
  GtkWidget *hbuttonbox2;
  GtkWidget *button_yes;
  GtkWidget *button_no;

  dlg_first = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_object_set_data (GTK_OBJECT (dlg_first), "dlg_first", dlg_first);
  gtk_container_set_border_width (GTK_CONTAINER (dlg_first), 21);
  gtk_window_set_title (GTK_WINDOW (dlg_first), _("First Time Configuration"));
  gtk_window_set_position (GTK_WINDOW (dlg_first), GTK_WIN_POS_CENTER);
  gtk_window_set_policy (GTK_WINDOW (dlg_first), FALSE, FALSE, FALSE);

  vbox3 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox3);
  gtk_object_set_data_full (GTK_OBJECT (dlg_first), "vbox3", vbox3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox3);
  gtk_container_add (GTK_CONTAINER (dlg_first), vbox3);

  label5 = gtk_label_new (_("This is the first time you are running the GTK+ GGZ Gaming Zone client. Would you like to create some default server profiles?"));
  gtk_widget_ref (label5);
  gtk_object_set_data_full (GTK_OBJECT (dlg_first), "label5", label5,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label5);
  gtk_box_pack_start (GTK_BOX (vbox3), label5, TRUE, TRUE, 0);
  gtk_label_set_line_wrap (GTK_LABEL (label5), TRUE);

  hbuttonbox2 = gtk_hbutton_box_new ();
  gtk_widget_ref (hbuttonbox2);
  gtk_object_set_data_full (GTK_OBJECT (dlg_first), "hbuttonbox2", hbuttonbox2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbuttonbox2);
  gtk_box_pack_start (GTK_BOX (vbox3), hbuttonbox2, FALSE, TRUE, 0);

  button_yes = gtk_button_new_with_label (_("Yes"));
  gtk_widget_ref (button_yes);
  gtk_object_set_data_full (GTK_OBJECT (dlg_first), "button_yes", button_yes,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button_yes);
  gtk_container_add (GTK_CONTAINER (hbuttonbox2), button_yes);
  GTK_WIDGET_SET_FLAGS (button_yes, GTK_CAN_DEFAULT);

  button_no = gtk_button_new_with_label (_("No"));
  gtk_widget_ref (button_no);
  gtk_object_set_data_full (GTK_OBJECT (dlg_first), "button_no", button_no,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button_no);
  gtk_container_add (GTK_CONTAINER (hbuttonbox2), button_no);
  GTK_WIDGET_SET_FLAGS (button_no, GTK_CAN_DEFAULT);

  gtk_signal_connect (GTK_OBJECT (button_yes), "clicked",
                      GTK_SIGNAL_FUNC (first_button_yes_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (button_no), "clicked",
                      GTK_SIGNAL_FUNC (first_button_no_activate),
                      NULL);

  return dlg_first;
}

