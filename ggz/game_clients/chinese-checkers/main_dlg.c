/*
 * File: main_dlg.c
 * Author: Rich Gade
 * Project: GGZ Chinese Checkers Client
 * Date: 01/01/2001
 * Desc: Create the main dialog window
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

#include "main_cb.h"
#include "main_dlg.h"
#include "support.h"

GtkWidget*
create_dlg_main (void)
{
  GtkWidget *dlg_main;
  GtkWidget *vbox1;
  GtkWidget *menubar1;
  guint tmp_key;
  GtkWidget *game_menu;
  GtkWidget *game_menu_menu;
  GtkAccelGroup *game_menu_menu_accels;
  GtkWidget *exit;
  GtkWidget *preferences_menu;
  GtkWidget *help_menu;
  GtkWidget *help_menu_menu;
  GtkAccelGroup *help_menu_menu_accels;
  GtkWidget *about;
  GtkWidget *hbox3;
  GtkWidget *p3_label;
  GtkWidget *p4_label;
  GtkWidget *p5_label;
  GtkWidget *draw_area;
  GtkWidget *hbox2;
  GtkWidget *p2_label;
  GtkWidget *p1_label;
  GtkWidget *p6_label;
  GtkWidget *statusbar1;
  GtkAccelGroup *accel_group;

  accel_group = gtk_accel_group_new ();

  dlg_main = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_name (dlg_main, "dlg_main");
  gtk_object_set_data (GTK_OBJECT (dlg_main), "dlg_main", dlg_main);
  gtk_window_set_title (GTK_WINDOW (dlg_main), "Chinese Checkers for GGZ");
  gtk_window_set_policy (GTK_WINDOW (dlg_main), FALSE, FALSE, FALSE);

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

  game_menu = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (game_menu)->child),
                                   "_Game");
  gtk_widget_add_accelerator (game_menu, "activate_item", accel_group,
                              tmp_key, GDK_MOD1_MASK, 0);
  gtk_widget_set_name (game_menu, "game_menu");
  gtk_widget_ref (game_menu);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "game_menu", game_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (game_menu);
  gtk_container_add (GTK_CONTAINER (menubar1), game_menu);

  game_menu_menu = gtk_menu_new ();
  gtk_widget_set_name (game_menu_menu, "game_menu_menu");
  gtk_widget_ref (game_menu_menu);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "game_menu_menu", game_menu_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (game_menu), game_menu_menu);
  game_menu_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (game_menu_menu));

  exit = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (exit)->child),
                                   "E_xit");
  gtk_widget_add_accelerator (exit, "activate_item", game_menu_menu_accels,
                              tmp_key, 0, 0);
  gtk_widget_set_name (exit, "exit");
  gtk_widget_ref (exit);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "exit", exit,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (exit);
  gtk_container_add (GTK_CONTAINER (game_menu_menu), exit);

  preferences_menu = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (preferences_menu)->child),
                                   "_Preferences");
  gtk_widget_add_accelerator (preferences_menu, "activate_item", accel_group,
                              tmp_key, GDK_MOD1_MASK, 0);
  gtk_widget_set_name (preferences_menu, "preferences_menu");
  gtk_widget_ref (preferences_menu);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "preferences_menu", preferences_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (preferences_menu);
  gtk_container_add (GTK_CONTAINER (menubar1), preferences_menu);

  help_menu = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (help_menu)->child),
                                   "_Help");
  gtk_widget_add_accelerator (help_menu, "activate_item", accel_group,
                              tmp_key, GDK_MOD1_MASK, 0);
  gtk_widget_set_name (help_menu, "help_menu");
  gtk_widget_ref (help_menu);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "help_menu", help_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (help_menu);
  gtk_container_add (GTK_CONTAINER (menubar1), help_menu);
  gtk_menu_item_right_justify (GTK_MENU_ITEM (help_menu));

  help_menu_menu = gtk_menu_new ();
  gtk_widget_set_name (help_menu_menu, "help_menu_menu");
  gtk_widget_ref (help_menu_menu);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "help_menu_menu", help_menu_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (help_menu), help_menu_menu);
  help_menu_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (help_menu_menu));

  about = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (about)->child),
                                   "_About");
  gtk_widget_add_accelerator (about, "activate_item", help_menu_menu_accels,
                              tmp_key, 0, 0);
  gtk_widget_set_name (about, "about");
  gtk_widget_ref (about);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "about", about,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (about);
  gtk_container_add (GTK_CONTAINER (help_menu_menu), about);

  hbox3 = gtk_hbox_new (TRUE, 0);
  gtk_widget_set_name (hbox3, "hbox3");
  gtk_widget_ref (hbox3);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "hbox3", hbox3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox3);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox3, TRUE, TRUE, 0);

  p3_label = gtk_label_new ("");
  gtk_widget_set_name (p3_label, "p3_label");
  gtk_widget_ref (p3_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "p3_label", p3_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (p3_label);
  gtk_box_pack_start (GTK_BOX (hbox3), p3_label, FALSE, FALSE, 0);

  p4_label = gtk_label_new ("");
  gtk_widget_set_name (p4_label, "p4_label");
  gtk_widget_ref (p4_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "p4_label", p4_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (p4_label);
  gtk_box_pack_start (GTK_BOX (hbox3), p4_label, FALSE, FALSE, 0);

  p5_label = gtk_label_new ("");
  gtk_widget_set_name (p5_label, "p5_label");
  gtk_widget_ref (p5_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "p5_label", p5_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (p5_label);
  gtk_box_pack_start (GTK_BOX (hbox3), p5_label, FALSE, FALSE, 0);

  draw_area = gtk_drawing_area_new ();
  gtk_widget_set_name (draw_area, "draw_area");
  gtk_widget_ref (draw_area);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "draw_area", draw_area,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (draw_area);
  gtk_box_pack_start (GTK_BOX (vbox1), draw_area, FALSE, TRUE, 0);
  gtk_widget_set_usize (draw_area, 400, 400);
  gtk_widget_set_events (draw_area, GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK);

  hbox2 = gtk_hbox_new (TRUE, 0);
  gtk_widget_set_name (hbox2, "hbox2");
  gtk_widget_ref (hbox2);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "hbox2", hbox2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox2);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox2, TRUE, TRUE, 0);

  p2_label = gtk_label_new ("");
  gtk_widget_set_name (p2_label, "p2_label");
  gtk_widget_ref (p2_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "p2_label", p2_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (p2_label);
  gtk_box_pack_start (GTK_BOX (hbox2), p2_label, FALSE, FALSE, 0);

  p1_label = gtk_label_new ("");
  gtk_widget_set_name (p1_label, "p1_label");
  gtk_widget_ref (p1_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "p1_label", p1_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (p1_label);
  gtk_box_pack_start (GTK_BOX (hbox2), p1_label, FALSE, FALSE, 0);

  p6_label = gtk_label_new ("");
  gtk_widget_set_name (p6_label, "p6_label");
  gtk_widget_ref (p6_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_main), "p6_label", p6_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (p6_label);
  gtk_box_pack_start (GTK_BOX (hbox2), p6_label, FALSE, FALSE, 0);

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
  gtk_signal_connect (GTK_OBJECT (exit), "activate",
                      GTK_SIGNAL_FUNC (on_exit_menu_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (about), "activate",
                      GTK_SIGNAL_FUNC (on_about_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (draw_area), "expose_event",
                      GTK_SIGNAL_FUNC (on_draw_area_expose_event),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (draw_area), "button_press_event",
                      GTK_SIGNAL_FUNC (on_draw_area_button_press_event),
                      NULL);

  gtk_window_add_accel_group (GTK_WINDOW (dlg_main), accel_group);

  return dlg_main;
}

