/*
 * File: roomlist.h
 * Author: GGZ Dev Team
 * Project: GGZ GTK Client
 * Date: 11/05/2004
 * $Id: roomlist.c 6277 2004-11-05 23:38:34Z jdorje $
 * 
 * List of rooms in the server
 * 
 * Copyright (C) 2000-2004 GGZ Development Team
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

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <ggz.h>
#include <ggzcore.h>

#include "client.h"
#include "msgbox.h"
#include "roominfo.h"
#include "roomlist.h"
#include "server.h"
#include "support.h"

static void client_join_room(guint room)
{
	gchar *err_msg = NULL;
	gint singleclick, status = -1;
	GGZRoom *ggzroom;

	switch (ggzcore_server_get_state(server)) {
	case GGZ_STATE_OFFLINE:
	case GGZ_STATE_CONNECTING:
	case GGZ_STATE_ONLINE:
	case GGZ_STATE_LOGGING_IN:
	case GGZ_STATE_LOGGING_OUT:
		err_msg = _("You can't join a room, you're not logged int");
		break;
	case GGZ_STATE_BETWEEN_ROOMS:
	case GGZ_STATE_ENTERING_ROOM:
		err_msg = _("You're already in between rooms");
		break;
	case GGZ_STATE_JOINING_TABLE:
	case GGZ_STATE_AT_TABLE:
	case GGZ_STATE_LEAVING_TABLE:
		err_msg = _("You can't switch rooms while playing a game");
		break;
	case GGZ_STATE_IN_ROOM:
	case GGZ_STATE_LOGGED_IN:
		status = 0;
		break;
	default:
		err_msg = _("Unknown error");
		status = -1;
	}

	if (status == 0) {
		ggzroom = ggzcore_server_get_cur_room(server);
			if (ggzcore_server_join_room(server, room) == 0) {
	
			/* Only desensitize with single click, dues to
	                some weird bug that freezes the mouse if we
        	        doubleclick and then desensitize */
			singleclick = ggzcore_conf_read_int("OPTIONS",
							    "ROOMENTRY",
							    FALSE);
			if (singleclick) {
				/* Set roomlist insensitive */
				sensitize_room_list(FALSE);
			}
			return;
		}
		else {
			err_msg = _("Error joining room");
		}
	}

	/* If we get here, there was an error */
	msgbox(err_msg, _("Error joining room"), MSGBOX_OKONLY, MSGBOX_STOP, 
	       MSGBOX_NORMAL);
}

static void client_room_join_activate(GtkMenuItem *menuitem, gpointer data)
{
	int room_num = GPOINTER_TO_INT(data);
	client_join_room(room_num);
}

static void client_room_info_activate(GtkMenuItem *menuitem, gpointer data)
{
	/* Display room's info in a nice dialog */
	int room_num = GPOINTER_TO_INT(data);
	GGZRoom *room = ggzcore_server_get_nth_room(server, room_num);
	room_info_create_or_raise(room);		
}

static GtkWidget *create_mnu_room(int room_num)
{
	GtkWidget *mnu_room;
	GtkWidget *info;
	GtkWidget *join;
	GtkAccelGroup *accel_group;

	accel_group = gtk_accel_group_new ();

	mnu_room = gtk_menu_new ();
	g_object_set_data(G_OBJECT (mnu_room), "mnu_room", mnu_room);

	info = gtk_menu_item_new_with_label (_("Info"));
	gtk_widget_ref (info);
	g_object_set_data_full(G_OBJECT (mnu_room), "info", info,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (info);
	gtk_container_add (GTK_CONTAINER (mnu_room), info);
	gtk_widget_add_accelerator (info, "activate", accel_group,
				    GDK_I, GDK_CONTROL_MASK,
				    GTK_ACCEL_VISIBLE);

	join = gtk_menu_item_new_with_label (_("Join"));
	gtk_widget_ref (join);
	g_object_set_data_full(G_OBJECT (mnu_room), "join", join,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (join);
	gtk_container_add (GTK_CONTAINER (mnu_room), join);
	gtk_widget_add_accelerator (join, "activate", accel_group,
				    GDK_J, GDK_CONTROL_MASK,
				    GTK_ACCEL_VISIBLE);

	g_signal_connect (GTK_OBJECT (info), "activate",
			  GTK_SIGNAL_FUNC (client_room_info_activate),
			  GINT_TO_POINTER(room_num));
	g_signal_connect (GTK_OBJECT (join), "activate",
			  GTK_SIGNAL_FUNC (client_room_join_activate),
			  GINT_TO_POINTER(room_num));

	gtk_menu_set_accel_group (GTK_MENU (mnu_room), accel_group);

	return mnu_room;
}

static gboolean client_room_clist_event(GtkWidget *widget, GdkEvent *event,
					gpointer data)
{
	gboolean single_join;
	gint row, column;
	GtkWidget *menu;
	GdkEventButton* buttonevent = (GdkEventButton*)event;
	GtkWidget *clist= lookup_widget(win_main, "room_clist");

	if (!gtk_clist_get_selection_info(GTK_CLIST(clist),
					  buttonevent->x,
					  buttonevent->y,
					  &row, &column)) {
		return FALSE;
	}

	single_join = ggzcore_conf_read_int("OPTIONS", "ROOMENTRY", FALSE);

	switch(event->type) { 
	case GDK_2BUTTON_PRESS:
		/* Double click */
		if (buttonevent->button == 1 && !single_join) {
			client_join_room(row);
			return TRUE;
		}
		break;
	case GDK_BUTTON_PRESS:
		/* Single click */
		if (buttonevent->button == 1 && single_join) {
			client_join_room(row);
			return TRUE;
		} else if (buttonevent->button == 3) {
			/* Right mouse button */
			/* Create and display the menu */
			gtk_clist_select_row(GTK_CLIST(clist), row, column);
			menu = create_mnu_room(row);
			gtk_menu_popup( GTK_MENU(menu), NULL, NULL, NULL,
					NULL, buttonevent->button, 0);
		}
		break;
	default:
		/* Some Other event */
		break;
	}

	return FALSE;
}

static void client_room_clist_select_row(GtkCList *clist,
                                         gint row, gint column,
                                         GdkEvent *event, gpointer data)
{

}

void sensitize_room_list(gboolean sensitive)
{
	GtkWidget *tmp = lookup_widget(win_main, "room_clist");
	gtk_widget_set_sensitive(tmp, sensitive);
}

void clear_room_list(void)
{
	/* Clear current list of rooms */
	GtkWidget *tmp;

	tmp = g_object_get_data(G_OBJECT(win_main), "room_clist");
        gtk_clist_clear(GTK_CLIST(tmp));
}

void select_room(GGZRoom *room)
{
	GtkWidget *tmp = lookup_widget(win_main, "room_clist");

	gtk_clist_select_row(GTK_CLIST(tmp), ggzcore_room_get_id(room), 0);
}

void update_one_room(GGZRoom *room)
{
	int nplayers, id;
	char *name, *players;
	GtkWidget *tmp;

	tmp = lookup_widget(win_main, "room_clist");

	name = ggzcore_room_get_name(room);
	id = ggzcore_room_get_id(room);
	nplayers = ggzcore_room_get_num_players(room);

	if (nplayers >= 0) {
		players = g_strdup_printf("%d", nplayers);
	} else {
		players = NULL;
	}

	gtk_clist_freeze(GTK_CLIST(tmp));
	gtk_clist_set_text(GTK_CLIST(tmp), id, 0, name);
	gtk_clist_set_text(GTK_CLIST(tmp), id, 1, players);
	gtk_clist_thaw(GTK_CLIST(tmp));

	if (players) g_free(players);
}

void update_room_list(void)
{
	GtkWidget *tmp = lookup_widget(win_main, "room_clist");
	int i;
	const int numrooms = ggzcore_server_get_num_rooms(server);

	/* Clear current list of rooms */
	gtk_clist_freeze(GTK_CLIST(tmp));
	gtk_clist_clear(GTK_CLIST(tmp));

	for (i = 0; i < numrooms; i++) {
		gchar *table[3];
		GGZRoom *room = ggzcore_server_get_nth_room(server, i);
		gchar *name = ggzcore_room_get_name(room);
		int players = ggzcore_room_get_num_players(room);

		table[0] = g_strdup_printf("%s", name);
		if (players >= 0) {
			table[1] = g_strdup_printf("%d", players);
		} else {
			table[1] = NULL;
		}
		table[2] = NULL;

		gtk_clist_append(GTK_CLIST(tmp), table);
	}

	gtk_clist_thaw(GTK_CLIST(tmp));
}

GtkWidget *create_room_list(GtkWidget *window)
{
	GtkWidget *room_clist, *room_label;

	room_clist = gtk_clist_new(2);
	gtk_widget_ref(room_clist);
	g_object_set_data_full(G_OBJECT(window), "room_clist", room_clist,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(room_clist);

	gtk_widget_set_sensitive (room_clist, FALSE);
	GTK_WIDGET_UNSET_FLAGS (room_clist, GTK_CAN_FOCUS);
	gtk_widget_set_events (room_clist, GDK_BUTTON_PRESS_MASK);
	gtk_clist_set_column_width(GTK_CLIST(room_clist), 0, 130);
	gtk_clist_set_column_width(GTK_CLIST(room_clist), 1, 15);
	gtk_clist_column_titles_show (GTK_CLIST (room_clist));

	room_label = gtk_label_new (_("Rooms"));
	gtk_widget_ref (room_label);
	g_object_set_data_full(G_OBJECT(window), "room_label", room_label,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (room_label);
	gtk_clist_set_column_widget (GTK_CLIST (room_clist), 0, room_label);

	room_label = gtk_label_new("#");
	gtk_widget_ref(room_label);
	g_object_set_data_full(G_OBJECT (window), "room_players_label",
			       room_label,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(room_label);
	gtk_clist_set_column_widget(GTK_CLIST(room_clist), 1, room_label);

	g_signal_connect(room_clist, "select_row",
			 GTK_SIGNAL_FUNC(client_room_clist_select_row),
			 NULL);
	g_signal_connect(room_clist, "button-press-event",
			 GTK_SIGNAL_FUNC(client_room_clist_event),
			 NULL);

	return room_clist;
}
