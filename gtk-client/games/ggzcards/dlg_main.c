/* 
 * File: dlg_main.c
 * Author: Rich Gade, Jason Short
 * Project: GGZCards Client
 * Date: 08/14/2000
 * Desc: Creates the GGZCards main Gtk window
 * $Id: dlg_main.c 9053 2007-04-17 03:16:36Z jdorje $
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

#include "dlg_chat.h"
#include "dlg_players.h"
#include "ggzintl.h"
#include "menus.h"

#include "cb_main.h"
#include "dlg_main.h"
#include "game.h"
#include "layout.h"
#include "table.h"

static GtkWidget *create_menus(GtkWidget * window)
{
	GtkItemFactoryEntry items[] = {
		TABLE_MENU,
		{_("/_Game"), NULL, NULL, 0, "<Branch>"},
		{GAME_START_GAME, "<ctrl>N",
		 on_mnu_startgame_activate, 0, NULL},
#ifdef DEBUG
		{_("/Game/Force _redraw"), "<ctrl>R",
		 on_mnu_forceredraw_activate, 0, NULL},
#endif
		{_("/Game/Sc_ores"), "<ctrl>O",
		 on_mnu_scores_activate, 0, NULL},
		{_("/_Messages"), NULL, NULL, 0, "<Branch>"},
		{_("/_Options"), NULL, NULL, 0, "<Branch>"},
		{_("/Options/_Preferences"), "<ctrl>P",
		 on_mnu_preferences_activate, 0, NULL},
		{_("/Options/_Reveal hand"), NULL,
		 on_mnu_revealhand_activate, 0, NULL},
		HELP_MENU,
	};
	GtkWidget *menubar;

	menubar = ggz_create_menus(window,
				   items,
				   sizeof(items) / sizeof(items[0]));

	set_menu_sensitive(GAME_START_GAME, FALSE);
	set_menu_sensitive(TABLE_CHAT_WINDOW, TRUE);

	return menubar;
}

GtkWidget *create_dlg_main(void)
{
	GtkWidget *dlg_main;
	GtkWidget *vbox1;
	GtkWidget *menubar;
	GtkWidget *fixed1;
	GtkWidget *statusbar1;
	GtkWidget *messagebar;
	GtkWidget *chatline;

	dlg_main = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_name(dlg_main, "dlg_main");
	g_object_set_data(G_OBJECT(dlg_main), "dlg_main", dlg_main);
	gtk_window_set_title(GTK_WINDOW(dlg_main),
			     _("GGZ Gaming Zone - GGZ Cards"));

	/* Perhaps we should allow the user to resize the window, and scale
	   the table with it.  As long as that doesn't happen, though, there's
	   no need whatsoever to change the window size. */
	gtk_window_set_resizable(GTK_WINDOW(dlg_main), FALSE);

	vbox1 = gtk_vbox_new(FALSE, 0);
	gtk_widget_set_name(vbox1, "vbox1");
	gtk_widget_ref(vbox1);
	g_object_set_data_full(G_OBJECT(dlg_main), "vbox1", vbox1,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox1);
	gtk_container_add(GTK_CONTAINER(dlg_main), vbox1);

	menubar = create_menus(dlg_main);
	gtk_widget_set_name(menubar, "menubar");
	gtk_widget_ref(menubar);
	g_object_set_data_full(G_OBJECT(dlg_main), "menubar", menubar,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(menubar);
	gtk_box_pack_start(GTK_BOX(vbox1), menubar, FALSE, FALSE, 0);

	fixed1 = gtk_fixed_new();
	gtk_fixed_set_has_window(GTK_FIXED(fixed1), TRUE);
	gtk_widget_set_name(fixed1, "fixed1");
	gtk_widget_ref(fixed1);
	g_object_set_data_full(G_OBJECT(dlg_main), "fixed1", fixed1,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(fixed1);
	gtk_box_pack_start(GTK_BOX(vbox1), fixed1, TRUE, TRUE, 0);
	gtk_widget_set_size_request(fixed1,
				    get_table_width(), get_table_height());

	messagebar = gtk_statusbar_new();
	gtk_widget_set_name(messagebar, "messagebar");
	gtk_widget_ref(messagebar);
	g_object_set_data_full(G_OBJECT(dlg_main), "messagebar",
			       messagebar,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_statusbar_set_has_resize_grip(GTK_STATUSBAR(messagebar), FALSE);
	gtk_widget_show(messagebar);
	gtk_box_pack_start(GTK_BOX(vbox1), messagebar, FALSE, FALSE, 0);

	statusbar1 = gtk_statusbar_new();
	gtk_widget_set_name(statusbar1, "statusbar1");
	gtk_widget_ref(statusbar1);
	g_object_set_data_full(G_OBJECT(dlg_main), "statusbar1",
			       statusbar1,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_statusbar_set_has_resize_grip(GTK_STATUSBAR(statusbar1), FALSE);
	gtk_widget_show(statusbar1);
	gtk_box_pack_start(GTK_BOX(vbox1), statusbar1, FALSE, FALSE, 0);

	chatline = create_chat_widget();
	gtk_widget_set_name(chatline, "chat");
	gtk_widget_ref(chatline);
	g_object_set_data_full(G_OBJECT(dlg_main), "chat",
			       chatline,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show_all(chatline);
	gtk_box_pack_start(GTK_BOX(vbox1), chatline, FALSE, FALSE, 0);

	(void)g_signal_connect(GTK_OBJECT(dlg_main), "delete_event",
			       GTK_SIGNAL_FUNC(on_dlg_main_delete_event),
			       NULL);
	(void)g_signal_connect(GTK_OBJECT(fixed1), "button_press_event",
			       GTK_SIGNAL_FUNC
			       (on_fixed1_button_press_event), NULL);
	(void)g_signal_connect(GTK_OBJECT(fixed1), "style_set",
			       GTK_SIGNAL_FUNC(on_fixed1_redraw_event),
			       NULL);
#if 0	/* We need some kind of redraw, I think, but
	   this doesn't draw correctly. */
	(void)g_signal_connect(GTK_OBJECT(fixed1), "size_allocate",
			       GTK_SIGNAL_FUNC(on_fixed1_redraw_event),
			       NULL);
#endif

	return dlg_main;
}
