/*
 * File: interface.c
 * Author: GGZ Development Team
 * Project: GGZ Combat Client
 * Date: 2001?
 * Desc: Interface code
 * $Id: interface.c 6284 2004-11-06 06:21:54Z jdorje $
 *
 * Copyright (C) 2001-2004 GGZ Development Team
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

#include "menus.h"

#include "callbacks.h"
#include "interface.h"
#include "support.h"

static GtkWidget *create_menus(GtkWidget *window)
{
  GtkItemFactoryEntry items[] = {
	  TABLE_MENU,
	  {_("/_Game"), NULL, NULL, 0, "<Branch>"},
	  {_("/Game/S_ave current map"), NULL,
	   on_save_map_menu_activate, 0, NULL},
	  {_("/Game/_Remember enemy units"), NULL,
	   on_remember_enemy_units_toggled, 0, "<ToggleItem>"},
	  {_("/Game/_Show game options"), NULL,
	   on_show_game_options_activate, 0, NULL},

	  HELP_MENU,
  };
  GtkWidget *menubar;

  menubar = ggz_create_menus(window,
			     items,
			     sizeof(items) / sizeof(items[0]));

  set_menu_sensitive(TABLE_PLAYER_LIST, FALSE);
  set_menu_sensitive(TABLE_SYNC, FALSE);
  set_menu_sensitive(TABLE_SIT, FALSE);
  set_menu_sensitive(TABLE_STAND, FALSE);

  return menubar;
}

GtkWidget*
create_main_window (void)
{
  GtkWidget *main_window;
  GtkWidget *vbox;
  GtkWidget *menubar;
  GtkWidget *hbox;
  GtkWidget *mainarea;
  GtkWidget *vseparator1;
  GtkWidget *player_box;
  guint send_setup_key;
  GtkWidget *send_setup;
  GtkWidget *hbox2;
  GtkWidget *statusbar;
  GtkWidget *current_turn;
  GtkAccelGroup *accel_group;
  GtkTooltips *tooltips;

  tooltips = gtk_tooltips_new ();

  accel_group = gtk_accel_group_new ();

  main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_name (main_window, "main_window");
  g_object_set_data(G_OBJECT (main_window), "main_window", main_window);
  gtk_window_set_title (GTK_WINDOW (main_window), _("Combat"));

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_widget_set_name (vbox, "vbox");
  gtk_widget_ref (vbox);
  g_object_set_data_full(G_OBJECT (main_window), "vbox", vbox,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox);
  gtk_container_add (GTK_CONTAINER (main_window), vbox);

  menubar = create_menus(main_window);
  gtk_widget_set_name (menubar, "menubar");
  gtk_widget_ref (menubar);
  g_object_set_data_full(G_OBJECT (main_window), "menubar", menubar,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (menubar);
  gtk_box_pack_start (GTK_BOX (vbox), menubar, FALSE, FALSE, 0);
  
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_set_name (hbox, "hbox");
  gtk_widget_ref (hbox);
  g_object_set_data_full(G_OBJECT (main_window), "hbox", hbox,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);

  mainarea = gtk_drawing_area_new ();
  gtk_widget_set_name (mainarea, "mainarea");
  gtk_widget_ref (mainarea);
  g_object_set_data_full(G_OBJECT (main_window), "mainarea", mainarea,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (mainarea);
  gtk_box_pack_start (GTK_BOX (hbox), mainarea, FALSE, FALSE, 0);
  gtk_widget_set_usize (mainarea, 491, 500);
  gtk_widget_set_events (mainarea, GDK_BUTTON_PRESS_MASK);

  vseparator1 = gtk_vseparator_new ();
  gtk_widget_set_name (vseparator1, "vseparator1");
  gtk_widget_ref (vseparator1);
  g_object_set_data_full(G_OBJECT (main_window), "vseparator1", vseparator1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vseparator1);
  gtk_box_pack_start (GTK_BOX (hbox), vseparator1, FALSE, FALSE, 7);
  gtk_widget_set_usize (vseparator1, 2, -2);

  player_box = gtk_vbox_new (FALSE, 0);
  gtk_widget_set_name (player_box, "player_box");
  gtk_widget_ref (player_box);
  g_object_set_data_full(G_OBJECT (main_window), "player_box", player_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (player_box);
  gtk_box_pack_start (GTK_BOX (hbox), player_box, TRUE, TRUE, 0);
  gtk_widget_set_usize (player_box, 180, -2);

  send_setup = gtk_button_new_with_label ("");
  send_setup_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (send_setup)->child),
                                   _("_Send Setup"));
  gtk_widget_add_accelerator (send_setup, "clicked", accel_group,
                              send_setup_key, GDK_MOD1_MASK, 0);
  gtk_widget_set_name (send_setup, "send_setup");
  gtk_widget_ref (send_setup);
  g_object_set_data_full(G_OBJECT (main_window), "send_setup", send_setup,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_box_pack_start (GTK_BOX (player_box), send_setup, FALSE, FALSE, 3);
  gtk_widget_set_sensitive (send_setup, FALSE);

  hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_set_name (hbox2, "hbox2");
  gtk_widget_ref (hbox2);
  g_object_set_data_full(G_OBJECT (main_window), "hbox2", hbox2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox2, FALSE, FALSE, 0);

  statusbar = gtk_statusbar_new ();
  gtk_widget_set_name (statusbar, "statusbar");
  gtk_widget_ref (statusbar);
  g_object_set_data_full(G_OBJECT (main_window), "statusbar", statusbar,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (statusbar);
  gtk_box_pack_start (GTK_BOX (hbox2), statusbar, TRUE, TRUE, 0);

  current_turn = gtk_statusbar_new ();
  gtk_widget_set_name (current_turn, "current_turn");
  gtk_widget_ref (current_turn);
  g_object_set_data_full(G_OBJECT (main_window), "current_turn", current_turn,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (current_turn);
  gtk_box_pack_start (GTK_BOX (hbox2), current_turn, FALSE, FALSE, 0);
  gtk_widget_set_usize (current_turn, 120, -2);

  g_signal_connect (GTK_OBJECT (main_window), "delete_event",
                      GTK_SIGNAL_FUNC (main_window_exit),
                      NULL);
  g_signal_connect (GTK_OBJECT (main_window), "destroy_event",
                      GTK_SIGNAL_FUNC (main_window_exit),
                      NULL);
  g_signal_connect (GTK_OBJECT (mainarea), "expose_event",
                      GTK_SIGNAL_FUNC (on_mainarea_expose_event),
                      NULL);
  g_signal_connect (GTK_OBJECT (mainarea), "configure_event",
                      GTK_SIGNAL_FUNC (on_mainarea_configure_event),
                      NULL);
  g_signal_connect (GTK_OBJECT (mainarea), "button_press_event",
                      GTK_SIGNAL_FUNC (on_mainarea_button_press_event),
                      NULL);
  g_signal_connect (GTK_OBJECT (send_setup), "clicked",
                      GTK_SIGNAL_FUNC (on_send_setup_clicked),
                      NULL);

  g_object_set_data(G_OBJECT (main_window), "tooltips", tooltips);

  gtk_window_add_accel_group (GTK_WINDOW (main_window), accel_group);

  return main_window;
}

