/*
 * File: dlg_main.c
 * Author: Rich Gade
 * Project: GGZ Connect the Dots Client
 * Date: 08/14/2000
 * Desc: Creates the main Gtk window
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

#include "dlg_about.h"
#include "dlg_players.h"

#include "cb_main.h"
#include "dlg_main.h"
#include "support.h"
#include "ggzintl.h"

GtkWidget *dlg_main;

static GtkWidget *create_menus(GtkWidget * window)
{
	GtkAccelGroup *accel_group;
	GtkItemFactory *menu;
	GtkWidget *menu_item;
	GtkItemFactoryEntry items[] = {
		{_("/_Table"), NULL, NULL, 0, "<Branch>"},
		{_("/Table/Player _list"), "<ctrl>L",
		 create_or_raise_dlg_players, 0, NULL},
		{_("/Table/_Sync with server"), "<ctrl>S", NULL, 0, NULL},
		{_("/Table/E_xit"), "<ctrl>X", on_mnu_exit_activate, 0,
		 NULL},
		{_("/_Options"), NULL, NULL, 0, "<Branch>"},
		{_("/Options/_Preferences"), "<ctrl>P",
		 on_mnu_preferences_activate,
		 0, NULL},
		{_("/_Help"), NULL, NULL, 0, "<LastBranch>"},
		{_("/Help/_About"), "<ctrl>A", create_or_raise_dlg_about,
		 0, NULL}
	};
	const int num = sizeof(items) / sizeof(items[0]);

	accel_group = gtk_accel_group_new();

	menu =
	    gtk_item_factory_new(GTK_TYPE_MENU_BAR, "<main>", accel_group);
	gtk_item_factory_create_items(menu, num, items, NULL);
	gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

	menu_item = gtk_item_factory_get_widget(menu,
						_("<main>/Table/"
						  "Sync with server"));
	gtk_widget_set_sensitive(menu_item, FALSE);

	return gtk_item_factory_get_widget(menu, "<main>");
}

GtkWidget *create_dlg_main(void)
{
	GtkWidget *vbox;
	GtkWidget *menubar;
	GtkWidget *board;
	GtkWidget *hbox1;
	GtkWidget *frame_left;
	GtkWidget *hbox2;
	GtkWidget *p1b;
	GtkWidget *lbl_score0;
	GtkWidget *frame_right;
	GtkWidget *hbox3;
	GtkWidget *p2b;
	GtkWidget *lbl_score1;
	GtkWidget *statusbar;
	GtkAccelGroup *accel_group;

	accel_group = gtk_accel_group_new();

	dlg_main = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_object_set_data(G_OBJECT(dlg_main), "dlg_main", dlg_main);
	gtk_window_set_title(GTK_WINDOW(dlg_main), "Connect the Dots");
	gtk_window_set_resizable(GTK_WINDOW(dlg_main), FALSE);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox);
	g_object_set_data_full(G_OBJECT(dlg_main), "vbox", vbox,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox);
	gtk_container_add(GTK_CONTAINER(dlg_main), vbox);

	menubar = create_menus(dlg_main);
	gtk_widget_ref(menubar);
	g_object_set_data_full(G_OBJECT(dlg_main), "menubar", menubar,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(menubar);
	gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);

#if 0
	mnu_game = gtk_menu_item_new_with_label("");
	tmp_key =
	    gtk_label_parse_uline(GTK_LABEL(GTK_BIN(mnu_game)->child),
				  _("_Game"));
	gtk_widget_add_accelerator(mnu_game, "activate_item", accel_group,
				   tmp_key, GDK_MOD1_MASK, 0);
	gtk_widget_ref(mnu_game);
	g_object_set_data_full(G_OBJECT(dlg_main), "mnu_game", mnu_game,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(mnu_game);
	gtk_container_add(GTK_CONTAINER(menubar), mnu_game);

	mnu_game_menu = gtk_menu_new();
	gtk_widget_ref(mnu_game_menu);
	g_object_set_data_full(G_OBJECT(dlg_main), "mnu_game_menu",
			       mnu_game_menu,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(mnu_game), mnu_game_menu);
	mnu_game_menu_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU(mnu_game_menu));

	mnu_exit = gtk_menu_item_new_with_label("");
	tmp_key =
	    gtk_label_parse_uline(GTK_LABEL(GTK_BIN(mnu_exit)->child),
				  _("E_xit"));
	gtk_widget_add_accelerator(mnu_exit, "activate_item",
				   mnu_game_menu_accels, tmp_key, 0, 0);
	gtk_widget_ref(mnu_exit);
	g_object_set_data_full(G_OBJECT(dlg_main), "mnu_exit", mnu_exit,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(mnu_exit);
	gtk_container_add(GTK_CONTAINER(mnu_game_menu), mnu_exit);

	mnu_settings = gtk_menu_item_new_with_label("");
	tmp_key =
	    gtk_label_parse_uline(GTK_LABEL(GTK_BIN(mnu_settings)->child),
				  _("_Settings"));
	gtk_widget_add_accelerator(mnu_settings, "activate_item",
				   accel_group, tmp_key, GDK_MOD1_MASK, 0);
	gtk_widget_ref(mnu_settings);
	g_object_set_data_full(G_OBJECT(dlg_main), "mnu_settings",
			       mnu_settings,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(mnu_settings);
	gtk_container_add(GTK_CONTAINER(menubar), mnu_settings);

	mnu_settings_menu = gtk_menu_new();
	gtk_widget_ref(mnu_settings_menu);
	g_object_set_data_full(G_OBJECT(dlg_main), "mnu_settings_menu",
			       mnu_settings_menu,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(mnu_settings),
				  mnu_settings_menu);
	mnu_settings_menu_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU(mnu_settings_menu));

	mnu_preferences = gtk_menu_item_new_with_label("");
	tmp_key =
	    gtk_label_parse_uline(GTK_LABEL
				  (GTK_BIN(mnu_preferences)->child),
				  _("_Preferences"));
	gtk_widget_add_accelerator(mnu_preferences, "activate_item",
				   mnu_settings_menu_accels, tmp_key, 0,
				   0);
	gtk_widget_ref(mnu_preferences);
	g_object_set_data_full(G_OBJECT(dlg_main), "mnu_preferences",
			       mnu_preferences,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(mnu_preferences);
	gtk_container_add(GTK_CONTAINER(mnu_settings_menu),
			  mnu_preferences);

	mnu_help = gtk_menu_item_new_with_label("");
	tmp_key =
	    gtk_label_parse_uline(GTK_LABEL(GTK_BIN(mnu_help)->child),
				  _("_Help"));
	gtk_widget_add_accelerator(mnu_help, "activate_item", accel_group,
				   tmp_key, GDK_MOD1_MASK, 0);
	gtk_widget_ref(mnu_help);
	g_object_set_data_full(G_OBJECT(dlg_main), "mnu_help", mnu_help,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(mnu_help);
	gtk_container_add(GTK_CONTAINER(menubar), mnu_help);
	gtk_menu_item_right_justify(GTK_MENU_ITEM(mnu_help));

	mnu_help_menu = gtk_menu_new();
	gtk_widget_ref(mnu_help_menu);
	g_object_set_data_full(G_OBJECT(dlg_main), "mnu_help_menu",
			       mnu_help_menu,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(mnu_help), mnu_help_menu);
	mnu_help_menu_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU(mnu_help_menu));

	mnu_about = gtk_menu_item_new_with_label("");
	tmp_key =
	    gtk_label_parse_uline(GTK_LABEL(GTK_BIN(mnu_about)->child),
				  _("_About"));
	gtk_widget_add_accelerator(mnu_about, "activate_item",
				   mnu_help_menu_accels, tmp_key, 0, 0);
	gtk_widget_ref(mnu_about);
	g_object_set_data_full(G_OBJECT(dlg_main), "mnu_about", mnu_about,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(mnu_about);
	gtk_container_add(GTK_CONTAINER(mnu_help_menu), mnu_about);
#endif

	board = gtk_drawing_area_new();
	gtk_widget_ref(board);
	g_object_set_data_full(G_OBJECT(dlg_main), "board", board,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(board);
	gtk_box_pack_start(GTK_BOX(vbox), board, FALSE, FALSE, 1);
	gtk_widget_set_size_request(board, 350, 350);
	gtk_widget_set_events(board,
			      GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK);

	hbox1 = gtk_hbox_new(TRUE, 0);
	gtk_widget_ref(hbox1);
	g_object_set_data_full(G_OBJECT(dlg_main), "hbox1", hbox1,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox1);
	gtk_box_pack_start(GTK_BOX(vbox), hbox1, TRUE, TRUE, 0);

	frame_left = gtk_frame_new("You");
	gtk_widget_ref(frame_left);
	g_object_set_data_full(G_OBJECT(dlg_main), "frame_left",
			       frame_left,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame_left);
	gtk_box_pack_start(GTK_BOX(hbox1), frame_left, TRUE, TRUE, 0);
	gtk_frame_set_shadow_type(GTK_FRAME(frame_left),
				  GTK_SHADOW_ETCHED_OUT);

	hbox2 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox2);
	g_object_set_data_full(G_OBJECT(dlg_main), "hbox2", hbox2,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox2);
	gtk_container_add(GTK_CONTAINER(frame_left), hbox2);

	p1b = gtk_drawing_area_new();
	gtk_widget_ref(p1b);
	g_object_set_data_full(G_OBJECT(dlg_main), "p1b", p1b,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(p1b);
	gtk_box_pack_start(GTK_BOX(hbox2), p1b, FALSE, TRUE, 5);
	gtk_widget_set_size_request(p1b, 15, 15);

	lbl_score0 = gtk_label_new(_("No Score"));
	gtk_widget_ref(lbl_score0);
	g_object_set_data_full(G_OBJECT(dlg_main), "lbl_score0",
			       lbl_score0,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(lbl_score0);
	gtk_box_pack_start(GTK_BOX(hbox2), lbl_score0, FALSE, FALSE, 0);

	frame_right = gtk_frame_new(_("Opponent"));
	gtk_widget_ref(frame_right);
	g_object_set_data_full(G_OBJECT(dlg_main), "frame_right",
			       frame_right,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame_right);
	gtk_box_pack_start(GTK_BOX(hbox1), frame_right, TRUE, TRUE, 0);
	gtk_frame_set_shadow_type(GTK_FRAME(frame_right),
				  GTK_SHADOW_ETCHED_OUT);

	hbox3 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox3);
	g_object_set_data_full(G_OBJECT(dlg_main), "hbox3", hbox3,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox3);
	gtk_container_add(GTK_CONTAINER(frame_right), hbox3);

	p2b = gtk_drawing_area_new();
	gtk_widget_ref(p2b);
	g_object_set_data_full(G_OBJECT(dlg_main), "p2b", p2b,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(p2b);
	gtk_box_pack_start(GTK_BOX(hbox3), p2b, FALSE, TRUE, 5);
	gtk_widget_set_size_request(p2b, 15, 15);

	lbl_score1 = gtk_label_new(_("No Score"));
	gtk_widget_ref(lbl_score1);
	g_object_set_data_full(G_OBJECT(dlg_main), "lbl_score1",
			       lbl_score1,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(lbl_score1);
	gtk_box_pack_start(GTK_BOX(hbox3), lbl_score1, FALSE, FALSE, 0);

	statusbar = gtk_statusbar_new();
	gtk_widget_ref(statusbar);
	g_object_set_data_full(G_OBJECT(dlg_main), "statusbar", statusbar,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(statusbar);
	gtk_box_pack_start(GTK_BOX(vbox), statusbar, FALSE, FALSE, 0);

	g_signal_connect(GTK_OBJECT(dlg_main), "delete_event",
			 GTK_SIGNAL_FUNC(on_window_delete_event), NULL);
#if 0
	g_signal_connect(GTK_OBJECT(mnu_exit), "activate",
			 GTK_SIGNAL_FUNC(on_mnu_exit_activate), NULL);
	g_signal_connect(GTK_OBJECT(mnu_preferences), "activate",
			 GTK_SIGNAL_FUNC(on_mnu_preferences_activate),
			 NULL);
	g_signal_connect(GTK_OBJECT(mnu_about), "activate",
			 GTK_SIGNAL_FUNC(on_mnu_about_activate), NULL);
#endif
	g_signal_connect(GTK_OBJECT(board), "expose_event",
			 GTK_SIGNAL_FUNC(on_board_expose_event), NULL);
	g_signal_connect(GTK_OBJECT(board), "button_press_event",
			 GTK_SIGNAL_FUNC(on_board_button_press_event),
			 NULL);
	g_signal_connect(GTK_OBJECT(p1b), "expose_event",
			 GTK_SIGNAL_FUNC(on_p1b_expose_event), NULL);
	g_signal_connect(GTK_OBJECT(p2b), "expose_event",
			 GTK_SIGNAL_FUNC(on_p2b_expose_event), NULL);

	gtk_window_add_accel_group(GTK_WINDOW(dlg_main), accel_group);

	return dlg_main;
}
