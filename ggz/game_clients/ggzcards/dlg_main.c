/*
 * File: dlg_main.c
 * Author: Rich Gade
 * Project: GGZCards Client
 * Date: 08/14/2000
 * Desc: Creates the GGZCards main Gtk window
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

#include "cb_main.h"
#include "dlg_main.h"
#include "support.h"
#include "layout.h"
#include "table.h"

GtkWidget*
create_dlg_main (void)
{
  GtkWidget *dlg_main;
  GtkWidget *vbox1;
  GtkWidget *menubar1;
  GtkWidget *mnu_game;
  GtkWidget *mnu_game_menu;
  GtkAccelGroup *mnu_game_menu_accels;
  GtkWidget *mnu_exit;
  GtkWidget *mnu_edit;
  GtkWidget *mnu_edit_menu;
  GtkAccelGroup *mnu_edit_menu_accels;
  GtkWidget *mnu_preferences;
  GtkWidget *mnu_help;
  GtkWidget *mnu_help_menu;
  GtkAccelGroup *mnu_help_menu_accels;
  GtkWidget *mnu_about;
  GtkWidget *fixed1;
  GtkWidget *statusbar1;

  dlg_main = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_name (dlg_main, "dlg_main");
  gtk_object_set_data (GTK_OBJECT (dlg_main), "dlg_main", dlg_main);
  gtk_window_set_title (GTK_WINDOW (dlg_main), "GGZ Gaming Zone - GGZ Cards");

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_set_name (vbox1, "vbox1");
  gtk_widget_ref (vbox1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "vbox1", vbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (dlg_main), vbox1);

  menubar1 = gtk_menu_bar_new ();
  gtk_widget_set_name (menubar1, "menubar1");
  gtk_widget_ref (menubar1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "menubar1", menubar1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (menubar1);
  gtk_box_pack_start (GTK_BOX (vbox1), menubar1, FALSE, FALSE, 0);

  mnu_game = gtk_menu_item_new_with_label ("Game");
  gtk_widget_set_name (mnu_game, "mnu_game");
  gtk_widget_ref (mnu_game);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "mnu_game", mnu_game,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (mnu_game);
  gtk_container_add (GTK_CONTAINER (menubar1), mnu_game);

  mnu_game_menu = gtk_menu_new ();
  gtk_widget_set_name (mnu_game_menu, "mnu_game_menu");
  gtk_widget_ref (mnu_game_menu);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "mnu_game_menu", mnu_game_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (mnu_game), mnu_game_menu);
  mnu_game_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (mnu_game_menu));

  mnu_exit = gtk_menu_item_new_with_label ("Exit");
  gtk_widget_set_name (mnu_exit, "mnu_exit");
  gtk_widget_ref (mnu_exit);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "mnu_exit", mnu_exit,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (mnu_exit);
  gtk_container_add (GTK_CONTAINER (mnu_game_menu), mnu_exit);

  mnu_edit = gtk_menu_item_new_with_label ("Edit");
  gtk_widget_set_name (mnu_edit, "mnu_edit");
  gtk_widget_ref (mnu_edit);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "mnu_edit", mnu_edit,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (mnu_edit);
  gtk_container_add (GTK_CONTAINER (menubar1), mnu_edit);

  mnu_edit_menu = gtk_menu_new ();
  gtk_widget_set_name (mnu_edit_menu, "mnu_edit_menu");
  gtk_widget_ref (mnu_edit_menu);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "mnu_edit_menu", mnu_edit_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (mnu_edit), mnu_edit_menu);
  mnu_edit_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (mnu_edit_menu));

  mnu_preferences = gtk_menu_item_new_with_label ("Preferences");
  gtk_widget_set_name (mnu_preferences, "mnu_preferences");
  gtk_widget_ref (mnu_preferences);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "mnu_preferences", mnu_preferences,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (mnu_preferences);
  gtk_container_add (GTK_CONTAINER (mnu_edit_menu), mnu_preferences);

  mnu_help = gtk_menu_item_new_with_label ("Help");
  gtk_widget_set_name (mnu_help, "mnu_help");
  gtk_widget_ref (mnu_help);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "mnu_help", mnu_help,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (mnu_help);
  gtk_container_add (GTK_CONTAINER (menubar1), mnu_help);

  mnu_help_menu = gtk_menu_new ();
  gtk_widget_set_name (mnu_help_menu, "mnu_help_menu");
  gtk_widget_ref (mnu_help_menu);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "mnu_help_menu", mnu_help_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (mnu_help), mnu_help_menu);
  mnu_help_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (mnu_help_menu));

  mnu_about = gtk_menu_item_new_with_label ("About");
  gtk_widget_set_name (mnu_about, "mnu_about");
  gtk_widget_ref (mnu_about);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "mnu_about", mnu_about,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (mnu_about);
  gtk_container_add (GTK_CONTAINER (mnu_help_menu), mnu_about);

  fixed1 = gtk_fixed_new ();
  gtk_widget_set_name (fixed1, "fixed1");
  gtk_widget_ref (fixed1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "fixed1", fixed1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (fixed1);
  gtk_box_pack_start (GTK_BOX (vbox1), fixed1, TRUE, TRUE, 0);
  gtk_widget_set_usize (fixed1, get_window_width(), get_window_height());

  statusbar1 = gtk_statusbar_new ();
  gtk_widget_set_name (statusbar1, "statusbar1");
  gtk_widget_ref (statusbar1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "statusbar1", statusbar1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (statusbar1);
  gtk_box_pack_start (GTK_BOX (vbox1), statusbar1, FALSE, FALSE, 0);

  gtk_signal_connect (GTK_OBJECT (dlg_main), "delete_event",
                      GTK_SIGNAL_FUNC (on_dlg_main_delete_event),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (mnu_exit), "activate",
                      GTK_SIGNAL_FUNC (on_mnu_exit_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (mnu_preferences), "activate",
                      GTK_SIGNAL_FUNC (on_mnu_preferences_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (mnu_about), "activate",
                      GTK_SIGNAL_FUNC (on_mnu_about_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (fixed1), "button_press_event",
                      GTK_SIGNAL_FUNC (on_fixed1_button_press_event),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (fixed1), "expose_event",
                      GTK_SIGNAL_FUNC (on_fixed1_expose_event),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (fixed1), "style_set",
		      GTK_SIGNAL_FUNC (on_fixed1_style_set),
		      NULL);

  return dlg_main;
}

