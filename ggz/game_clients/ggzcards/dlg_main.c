/* 
 * File: dlg_main.c
 * Author: Rich Gade, Jason Short
 * Project: GGZCards Client
 * Date: 08/14/2000
 * Desc: Creates the GGZCards main Gtk window
 * $Id: dlg_main.c 4917 2002-10-14 22:27:43Z jdorje $
 *
 * Copyright (C) 2000-2002 Brent Hendricks.
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

#include "dlg_players.h"
#include "ggzintl.h"

#include "cb_main.h"
#include "dlg_main.h"
#include "game.h"
#include "layout.h"
#include "table.h"

static GtkItemFactory *menu;

static GtkWidget *create_menus(GtkWidget *window)
{
	GtkAccelGroup *accel_group;
	GtkItemFactoryEntry items[] = {
	  {_("/_Table"), NULL, NULL, 0, "<Branch>"},
	  {_("/Table/Player _list"), "<ctrl>L",
	   create_or_raise_dlg_players, 0, NULL},
	  {_("/Table/_Sync with server"), "<ctrl>S",
	   on_mnu_sync_activate, 0, NULL},
	  {_("/Table/E_xit"), "<ctrl>X", on_mnu_exit_activate, 0, NULL},
	  {_("/_Game"), NULL, NULL, 0, "<Branch>"},
	  {_("/Game/Start game"), "<ctrl>N",
	   on_mnu_startgame_activate, 0, NULL},
#ifdef DEBUG
	  {_("/Game/Force _redraw"), "<ctrl>R",
	   on_mnu_forceredraw_activate, 0, NULL},
#endif
	  {_("/_Messages"), NULL, NULL, 0, "<Branch>"},
	  {_("/_Options"), NULL, NULL, 0, "<Branch>"},
	  {_("/Options/_Preferences"), "<ctrl>P",
	   on_mnu_preferences_activate, 0, NULL},
	  {_("/_Help"), NULL, NULL, 0, "<LastBranch>"},
	  {_("/Help/_About"), "<ctrl>A", on_mnu_about_activate, 0, NULL}
	};
	const int num = sizeof(items) / sizeof(items[0]);

	accel_group = gtk_accel_group_new();

	menu = gtk_item_factory_new(GTK_TYPE_MENU_BAR, "<main>", accel_group);
	gtk_item_factory_create_items(menu, num, items, NULL);
	gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

	gtk_object_set_data(GTK_OBJECT(window), "mbar", menu);

	return gtk_item_factory_get_widget(menu, "<main>");
}

GtkWidget *get_menu_item(const char *item)
{
	return gtk_item_factory_get_widget(menu, item);
}

GtkWidget *create_dlg_main(void)
{
	GtkWidget *dlg_main;
	GtkWidget *vbox1;
	GtkWidget *menubar;
	GtkWidget *fixed1;
	GtkWidget *statusbar1;
	GtkWidget *messagebar;

	dlg_main = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_name(dlg_main, "dlg_main");
	gtk_object_set_data(GTK_OBJECT(dlg_main), "dlg_main", dlg_main);
	gtk_window_set_title(GTK_WINDOW(dlg_main),
			     _("GGZ Gaming Zone - GGZ Cards"));

	vbox1 = gtk_vbox_new(FALSE, 0);
	gtk_widget_set_name(vbox1, "vbox1");
	gtk_widget_ref(vbox1);
	gtk_object_set_data_full(GTK_OBJECT(dlg_main), "vbox1", vbox1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox1);
	gtk_container_add(GTK_CONTAINER(dlg_main), vbox1);

	menubar = create_menus(dlg_main);
	gtk_widget_set_name(menubar, "menubar");
	gtk_widget_ref(menubar);
	gtk_object_set_data_full(GTK_OBJECT(dlg_main), "menubar", menubar,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(menubar);
	gtk_box_pack_start(GTK_BOX(vbox1), menubar, FALSE, FALSE, 0);

	fixed1 = gtk_fixed_new();
	gtk_widget_set_name(fixed1, "fixed1");
	gtk_widget_ref(fixed1);
	gtk_object_set_data_full(GTK_OBJECT(dlg_main), "fixed1", fixed1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(fixed1);
	gtk_box_pack_start(GTK_BOX(vbox1), fixed1, TRUE, TRUE, 0);
	gtk_widget_set_usize(fixed1, get_table_width(), get_table_height());

	messagebar = gtk_statusbar_new();
	gtk_widget_set_name(messagebar, "messagebar");
	gtk_widget_ref(messagebar);
	gtk_object_set_data_full(GTK_OBJECT(dlg_main), "messagebar",
				 messagebar,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(messagebar);
	gtk_box_pack_start(GTK_BOX(vbox1), messagebar, FALSE, FALSE, 0);

	statusbar1 = gtk_statusbar_new();
	gtk_widget_set_name(statusbar1, "statusbar1");
	gtk_widget_ref(statusbar1);
	gtk_object_set_data_full(GTK_OBJECT(dlg_main), "statusbar1",
				 statusbar1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(statusbar1);
	gtk_box_pack_start(GTK_BOX(vbox1), statusbar1, FALSE, FALSE, 0);

	(void) gtk_signal_connect(GTK_OBJECT(dlg_main), "delete_event",
				  GTK_SIGNAL_FUNC(on_dlg_main_delete_event),
				  NULL);
	(void) gtk_signal_connect(GTK_OBJECT(fixed1), "button_press_event",
				  GTK_SIGNAL_FUNC
				  (on_fixed1_button_press_event), NULL);
	(void) gtk_signal_connect(GTK_OBJECT(fixed1), "style_set",
				  GTK_SIGNAL_FUNC(on_fixed1_redraw_event),
				  NULL);
#if 0				/* We need some kind of redraw, I think, but
				   this doesn't draw correctly. */
	(void) gtk_signal_connect(GTK_OBJECT(fixed1), "size_allocate",
				  GTK_SIGNAL_FUNC(on_fixed1_redraw_event),
				  NULL);
#endif

	return dlg_main;
}
