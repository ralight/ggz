/*
 * File: client.c
 * Author: Justin Zaun
 * Project: GGZ GTK Client
 * 
 * This is the main program body for the GGZ client
 * 
 * Copyright (C) 2000 Justin Zaun.
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

#include <config.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <ggzcore.h>

#include "client.h"
#include "chat.h"
#include "login.h"
#include "xtext.h"

extern GdkColor colors[];
extern GtkWidget *win_main;
extern GtkWidget *dlg_login;


void
on_connect_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	if(dlg_login != NULL)
		dlg_login = create_dlg_login();
	gtk_widget_show(dlg_login);
}


void
on_disconnect_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkCList *tmp;

	ggzcore_event_trigger(GGZ_USER_LOGOUT, NULL, NULL);

	/* Clear current list of rooms */
        tmp = gtk_object_get_data(GTK_OBJECT(win_main), "room_clist");
        gtk_clist_clear(GTK_CLIST(tmp));

	/* Clear current list of players */
        tmp = gtk_object_get_data(GTK_OBJECT(win_main), "player_clist");
        gtk_clist_clear(GTK_CLIST(tmp));
}


void
on_exit_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	gtk_main_quit();
}


void
on_launch_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_join_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_leave_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_properties_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_room_list_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkWidget *tmp;
	static gboolean room_view = TRUE;

	tmp = gtk_object_get_data(GTK_OBJECT(win_main), "room_scrolledwindow");
	if(room_view == TRUE)
	{
		gtk_widget_hide(tmp);
		room_view = FALSE;
	} else {
		gtk_widget_show(tmp);
		room_view = TRUE;
	}
}


void
on_player_list_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkWidget *tmp;
	static gboolean player_view = TRUE;

	tmp = gtk_object_get_data(GTK_OBJECT(win_main), "player_scrolledwindow");
	if(player_view == TRUE)
	{
		gtk_widget_hide(tmp);
		player_view = FALSE;
	} else {
		gtk_widget_show(tmp);
		player_view = TRUE;
	}
}


void
on_server_stats_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_player_stats_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_game_types_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_motd_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_about_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_gpl_license_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_ggz_help_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_game_help_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_goto_web1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


GtkWidget*
main_xtext_chat_create (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2)
{
        GtkWidget *chat_text;
        chat_text = gtk_xtext_new (TRUE, TRUE);
        return chat_text;
}


void
on_enter_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_info_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_connect_button_clicked              (GtkButton         *button,
                                        gpointer         user_data)
{
	if(dlg_login != NULL)
		dlg_login = create_dlg_login();
	gtk_widget_show(dlg_login);
}


void
on_disconnect_button_clicked           (GtkButton         *button,
                                        gpointer         user_data)
{
	GtkCList *tmp;

	ggzcore_event_trigger(GGZ_USER_LOGOUT, NULL, NULL);

	/* Clear current list of rooms */
        tmp = gtk_object_get_data(GTK_OBJECT(win_main), "room_clist");
        gtk_clist_clear(GTK_CLIST(tmp));

	/* Clear current list of players */
        tmp = gtk_object_get_data(GTK_OBJECT(win_main), "player_clist");
        gtk_clist_clear(GTK_CLIST(tmp));
}


void
on_chat_entry_activate                 (GtkEditable     *editable,
                                        gpointer         user_data)
{
	GtkEntry *tmp;

	tmp = gtk_object_get_data(GTK_OBJECT(win_main), "chat_entry");

	if(strncasecmp(gtk_entry_get_text(GTK_ENTRY(user_data)), "/help", 5))
	{
		if (strcmp(gtk_entry_get_text(GTK_ENTRY(tmp)),""))
		{
			if(!strncasecmp(gtk_entry_get_text(GTK_ENTRY(user_data)), "/msg", 3))
			{
				/* A private message */
			}
			else if(!strncasecmp(gtk_entry_get_text(GTK_ENTRY(user_data)), "/beep", 4))
			{
				/* A beep message */
				chat_send_beep();
			}
			else
			{
				/* Just a normal message */
				chat_send_msg();
			}
		}
	} else {
		/* Display Help */
	}

	/* Clear the entry box */
	gtk_entry_set_text(GTK_ENTRY(tmp), "");
}
 
 
gboolean
on_chat_entry_key_press_event          (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data)
{   
	return TRUE;
}
                                        

void
on_send_button_clicked                 (GtkButton       *button,   
                                        gpointer         user_data)
{
	chat_send_msg();
}


void
on_table_clist_select_row              (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{

}


void
on_table_clist_click_column            (GtkCList        *clist,
                                        gint             column,
                                        gpointer         user_data)
{

}


void
on_launch_button_clicked               (GtkButton         *button,
                                        gpointer         user_data)
{

}


void
on_join_button_clicked                 (GtkButton         *button,
                                        gpointer         user_data)
{

}


void
on_props_button_clicked                (GtkButton         *button,
                                        gpointer         user_data)
{

}


void
on_stats_button_clicked                (GtkButton         *button,
                                        gpointer         user_data)
{

}


void
on_exit_button_clicked                 (GtkButton         *button,
                                        gpointer         user_data)
{
	gtk_main_quit();
}


void
on_room_clist_select_row               (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	ggzcore_event_trigger(GGZ_USER_JOIN_ROOM, (void*)row, NULL); 
	gtk_clist_unselect_row(GTK_CLIST(clist), row, column);
}


void
on_win_main_realize                    (GtkWidget       *widget,
                                        gpointer         user_data)
{
	GdkFont* fixed;
	GtkXText *tmp;
	char *buf;


	fixed = gdk_font_load( "-*-fixed-medium-r-semicondensed--*-120-*-*-c-*-iso8859-8" );

	tmp = gtk_object_get_data(GTK_OBJECT(win_main), "xtext_custom");
	gtk_xtext_set_font(GTK_XTEXT(tmp), fixed, 0);

	gtk_xtext_set_palette (GTK_XTEXT(tmp), colors); 
	tmp->auto_indent = TRUE;
	tmp->wordwrap = TRUE;
	tmp->max_auto_indent = 200;
	tmp->time_stamp = FALSE;
	gtk_xtext_refresh(tmp,0);

	buf = g_strdup_printf("Client Version:\00314 %s",VERSION);
	gtk_xtext_append_indent(tmp,"---",3,buf,strlen(buf)); 
	g_free(buf);
	buf = g_strdup_printf("GTK+ Version:\00314 %d.%d.%d\n",
		gtk_major_version, gtk_minor_version, gtk_micro_version);
	gtk_xtext_append_indent(tmp,"---",3,buf,strlen(buf));
	g_free(buf);

#ifdef DEBUG
	gtk_xtext_append_indent(tmp,"---",3,"Compiled with debugging.", 24);
#endif

}




GtkWidget*
create_win_main (void)
{
  GtkWidget *win_main;
  GtkWidget *main_vbox;
  GtkWidget *menubar;
  GtkWidget *ggz;
  GtkWidget *ggz_menu;
  GtkAccelGroup *ggz_menu_accels;
  GtkWidget *connect;
  GtkWidget *disconnect;
  GtkWidget *separator1;
  GtkWidget *exit;
  GtkWidget *game;
  GtkWidget *game_menu;
  GtkAccelGroup *game_menu_accels;
  GtkWidget *launch;
  GtkWidget *join;
  GtkWidget *separator2;
  GtkWidget *leave;
  GtkWidget *edit;
  GtkWidget *edit_menu;
  GtkAccelGroup *edit_menu_accels;
  GtkWidget *properties;
  GtkWidget *view;
  GtkWidget *view_menu;
  GtkAccelGroup *view_menu_accels;
  GtkWidget *room_list;
  GtkWidget *player_list;
  GtkWidget *separator8;
  GtkWidget *server_stats;
  GtkWidget *player_stats;
  GtkWidget *game_types;
  GtkWidget *separator3;
  GtkWidget *motd;
  GtkWidget *help;
  GtkWidget *help_menu;
  GtkAccelGroup *help_menu_accels;
  GtkWidget *about;
  GtkWidget *gpl_license;
  GtkWidget *separator6;
  GtkWidget *ggz_help;
  GtkWidget *game_help;
  GtkWidget *separator7;
  GtkWidget *goto_web1;
  GtkWidget *handlebox1;
  GtkWidget *toolbar;
  GtkWidget *connect_button;
  GtkWidget *disconnect_button;
  GtkWidget *launch_button;
  GtkWidget *join_button;
  GtkWidget *props_button;
  GtkWidget *stats_button;
  GtkWidget *exit_button;
  GtkWidget *Current_room_label;
  GtkWidget *client_hbox;
  GtkWidget *client_hpaned;
  GtkWidget *lists_vbox;
  GtkWidget *room_scrolledwindow;
  GtkWidget *room_clist;
  GtkWidget *room_label;
  GtkWidget *player_scrolledwindow;
  GtkWidget *player_clist;
  GtkWidget *player_lag_label;
  GtkWidget *player_table_label;
  GtkWidget *player_name_label;
  GtkWidget *table_vpaned;
  GtkWidget *scrolledwindow3;
  GtkWidget *table_clist;
  GtkWidget *table_no_label;
  GtkWidget *table_steats_label;
  GtkWidget *tabel_desc_label;
  GtkWidget *chat_vbox;
  GtkWidget *chatdisplay_hbox;
  GtkWidget *chat_frame;
  GtkWidget *xtext_custom;
  GtkWidget *chat_vscrollbar;
  GtkWidget *newchat_hbox;
  GtkWidget *chat_label;
  GtkWidget *chat_entry;
  GtkWidget *chat_hbuttonbox;
  GtkWidget *send_button;
  GtkWidget *statusbar;
  GtkAccelGroup *accel_group;

  accel_group = gtk_accel_group_new ();

  win_main = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_object_set_data (GTK_OBJECT (win_main), "win_main", win_main);
  gtk_widget_set_usize (win_main, 620, 400);
  gtk_window_set_title (GTK_WINDOW (win_main), "GNU Gaming Zone");
  gtk_window_set_policy (GTK_WINDOW (win_main), FALSE, TRUE, TRUE);

  main_vbox = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (main_vbox);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "main_vbox", main_vbox,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (main_vbox);
  gtk_container_add (GTK_CONTAINER (win_main), main_vbox);

  menubar = gtk_menu_bar_new ();
  gtk_widget_ref (menubar);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "menubar", menubar,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (menubar);
  gtk_box_pack_start (GTK_BOX (main_vbox), menubar, FALSE, FALSE, 0);

  ggz = gtk_menu_item_new_with_label ("GGZ");
  gtk_widget_ref (ggz);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "ggz", ggz,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (ggz);
  gtk_container_add (GTK_CONTAINER (menubar), ggz);

  ggz_menu = gtk_menu_new ();
  gtk_widget_ref (ggz_menu);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "ggz_menu", ggz_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (ggz), ggz_menu);
  ggz_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (ggz_menu));

  connect = gtk_menu_item_new_with_label ("Connect");
  gtk_widget_ref (connect);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "connect", connect,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (connect);
  gtk_container_add (GTK_CONTAINER (ggz_menu), connect);
  gtk_widget_add_accelerator (connect, "activate", accel_group,
                              GDK_C, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  disconnect = gtk_menu_item_new_with_label ("Disconnect");
  gtk_widget_ref (disconnect);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "disconnect", disconnect,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (disconnect);
  gtk_container_add (GTK_CONTAINER (ggz_menu), disconnect);
  gtk_widget_add_accelerator (disconnect, "activate", accel_group,
                              GDK_D, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  separator1 = gtk_menu_item_new ();
  gtk_widget_ref (separator1);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "separator1", separator1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (separator1);
  gtk_container_add (GTK_CONTAINER (ggz_menu), separator1);
  gtk_widget_set_sensitive (separator1, FALSE);

  exit = gtk_menu_item_new_with_label ("Exit");
  gtk_widget_ref (exit);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "exit", exit,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (exit);
  gtk_container_add (GTK_CONTAINER (ggz_menu), exit);
  gtk_widget_add_accelerator (exit, "activate", accel_group,
                              GDK_X, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  game = gtk_menu_item_new_with_label ("Game");
  gtk_widget_ref (game);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "game", game,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (game);
  gtk_container_add (GTK_CONTAINER (menubar), game);

  game_menu = gtk_menu_new ();
  gtk_widget_ref (game_menu);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "game_menu", game_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (game), game_menu);
  game_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (game_menu));

  launch = gtk_menu_item_new_with_label ("Launch");
  gtk_widget_ref (launch);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "launch", launch,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (launch);
  gtk_container_add (GTK_CONTAINER (game_menu), launch);
  gtk_widget_add_accelerator (launch, "activate", accel_group,
                              GDK_L, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  join = gtk_menu_item_new_with_label ("Join");
  gtk_widget_ref (join);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "join", join,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (join);
  gtk_container_add (GTK_CONTAINER (game_menu), join);

  separator2 = gtk_menu_item_new ();
  gtk_widget_ref (separator2);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "separator2", separator2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (separator2);
  gtk_container_add (GTK_CONTAINER (game_menu), separator2);
  gtk_widget_set_sensitive (separator2, FALSE);

  leave = gtk_menu_item_new_with_label ("Leave");
  gtk_widget_ref (leave);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "leave", leave,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (leave);
  gtk_container_add (GTK_CONTAINER (game_menu), leave);

  edit = gtk_menu_item_new_with_label ("Edit");
  gtk_widget_ref (edit);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "edit", edit,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (edit);
  gtk_container_add (GTK_CONTAINER (menubar), edit);

  edit_menu = gtk_menu_new ();
  gtk_widget_ref (edit_menu);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "edit_menu", edit_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (edit), edit_menu);
  edit_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (edit_menu));

  properties = gtk_menu_item_new_with_label ("Properties");
  gtk_widget_ref (properties);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "properties", properties,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (properties);
  gtk_container_add (GTK_CONTAINER (edit_menu), properties);
  gtk_widget_add_accelerator (properties, "activate", accel_group,
                              GDK_P, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  view = gtk_menu_item_new_with_label ("View");
  gtk_widget_ref (view);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "view", view,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (view);
  gtk_container_add (GTK_CONTAINER (menubar), view);

  view_menu = gtk_menu_new ();
  gtk_widget_ref (view_menu);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "view_menu", view_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (view), view_menu);
  view_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (view_menu));

  room_list = gtk_check_menu_item_new_with_label ("Room List");
  gtk_widget_ref (room_list);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "room_list", room_list,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (room_list);
  gtk_container_add (GTK_CONTAINER (view_menu), room_list);
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (room_list), TRUE);

  player_list = gtk_check_menu_item_new_with_label ("Player List");
  gtk_widget_ref (player_list);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "player_list", player_list,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (player_list);
  gtk_container_add (GTK_CONTAINER (view_menu), player_list);
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (player_list), TRUE);

  separator8 = gtk_menu_item_new ();
  gtk_widget_ref (separator8);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "separator8", separator8,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (separator8);
  gtk_container_add (GTK_CONTAINER (view_menu), separator8);
  gtk_widget_set_sensitive (separator8, FALSE);

  server_stats = gtk_menu_item_new_with_label ("Server Stats");
  gtk_widget_ref (server_stats);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "server_stats", server_stats,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (server_stats);
  gtk_container_add (GTK_CONTAINER (view_menu), server_stats);

  player_stats = gtk_menu_item_new_with_label ("Player Stats");
  gtk_widget_ref (player_stats);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "player_stats", player_stats,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (player_stats);
  gtk_container_add (GTK_CONTAINER (view_menu), player_stats);
  gtk_widget_add_accelerator (player_stats, "activate", accel_group,
                              GDK_S, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  game_types = gtk_menu_item_new_with_label ("Game Types");
  gtk_widget_ref (game_types);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "game_types", game_types,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (game_types);
  gtk_container_add (GTK_CONTAINER (view_menu), game_types);

  separator3 = gtk_menu_item_new ();
  gtk_widget_ref (separator3);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "separator3", separator3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (separator3);
  gtk_container_add (GTK_CONTAINER (view_menu), separator3);
  gtk_widget_set_sensitive (separator3, FALSE);

  motd = gtk_menu_item_new_with_label ("MOTD");
  gtk_widget_ref (motd);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "motd", motd,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (motd);
  gtk_container_add (GTK_CONTAINER (view_menu), motd);
  gtk_widget_add_accelerator (motd, "activate", accel_group,
                              GDK_M, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  help = gtk_menu_item_new_with_label ("Help");
  gtk_widget_ref (help);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "help", help,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (help);
  gtk_container_add (GTK_CONTAINER (menubar), help);

  help_menu = gtk_menu_new ();
  gtk_widget_ref (help_menu);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "help_menu", help_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (help), help_menu);
  help_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (help_menu));

  about = gtk_menu_item_new_with_label ("About");
  gtk_widget_ref (about);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "about", about,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (about);
  gtk_container_add (GTK_CONTAINER (help_menu), about);
  gtk_widget_add_accelerator (about, "activate", accel_group,
                              GDK_A, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  gpl_license = gtk_menu_item_new_with_label ("GPL License");
  gtk_widget_ref (gpl_license);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "gpl_license", gpl_license,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (gpl_license);
  gtk_container_add (GTK_CONTAINER (help_menu), gpl_license);

  separator6 = gtk_menu_item_new ();
  gtk_widget_ref (separator6);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "separator6", separator6,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (separator6);
  gtk_container_add (GTK_CONTAINER (help_menu), separator6);
  gtk_widget_set_sensitive (separator6, FALSE);

  ggz_help = gtk_menu_item_new_with_label ("GGZ Help");
  gtk_widget_ref (ggz_help);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "ggz_help", ggz_help,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (ggz_help);
  gtk_container_add (GTK_CONTAINER (help_menu), ggz_help);
  gtk_widget_add_accelerator (ggz_help, "activate", accel_group,
                              GDK_F1, 0,
                              GTK_ACCEL_VISIBLE);

  game_help = gtk_menu_item_new_with_label ("Game Help");
  gtk_widget_ref (game_help);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "game_help", game_help,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (game_help);
  gtk_container_add (GTK_CONTAINER (help_menu), game_help);

  separator7 = gtk_menu_item_new ();
  gtk_widget_ref (separator7);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "separator7", separator7,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (separator7);
  gtk_container_add (GTK_CONTAINER (help_menu), separator7);
  gtk_widget_set_sensitive (separator7, FALSE);

  goto_web1 = gtk_menu_item_new_with_label ("Goto Web");
  gtk_widget_ref (goto_web1);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "goto_web1", goto_web1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (goto_web1);
  gtk_container_add (GTK_CONTAINER (help_menu), goto_web1);

  handlebox1 = gtk_handle_box_new ();
  gtk_widget_ref (handlebox1);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "handlebox1", handlebox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (handlebox1);
  gtk_box_pack_start (GTK_BOX (main_vbox), handlebox1, FALSE, TRUE, 0);

  toolbar = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_TEXT);
  gtk_widget_ref (toolbar);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "toolbar", toolbar,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (toolbar);
  gtk_container_add (GTK_CONTAINER (handlebox1), toolbar);

  connect_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "Connect",
                                NULL, NULL,
                                NULL, NULL, NULL);
  gtk_widget_ref (connect_button);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "connect_button", connect_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (connect_button);

  disconnect_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "Disconnect",
                                NULL, NULL,
                                NULL, NULL, NULL);
  gtk_widget_ref (disconnect_button);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "disconnect_button", disconnect_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (disconnect_button);

  launch_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "Launch",
                                NULL, NULL,
                                NULL, NULL, NULL);
  gtk_widget_ref (launch_button);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "launch_button", launch_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (launch_button);

  join_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "Join",
                                NULL, NULL,
                                NULL, NULL, NULL);
  gtk_widget_ref (join_button);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "join_button", join_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (join_button);

  props_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "Props",
                                NULL, NULL,
                                NULL, NULL, NULL);
  gtk_widget_ref (props_button);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "props_button", props_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (props_button);

  stats_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "Stats",
                                NULL, NULL,
                                NULL, NULL, NULL);
  gtk_widget_ref (stats_button);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "stats_button", stats_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (stats_button);

  exit_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "Exit",
                                NULL, NULL,
                                NULL, NULL, NULL);
  gtk_widget_ref (exit_button);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "exit_button", exit_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (exit_button);

  Current_room_label = gtk_label_new ("Please select a room.");
  gtk_widget_ref (Current_room_label);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "Current_room_label", Current_room_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (Current_room_label);
  gtk_box_pack_start (GTK_BOX (main_vbox), Current_room_label, FALSE, FALSE, 0);

  client_hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (client_hbox);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "client_hbox", client_hbox,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (client_hbox);
  gtk_box_pack_start (GTK_BOX (main_vbox), client_hbox, TRUE, TRUE, 0);

  client_hpaned = gtk_hpaned_new ();
  gtk_widget_ref (client_hpaned);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "client_hpaned", client_hpaned,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (client_hpaned);
  gtk_box_pack_start (GTK_BOX (client_hbox), client_hpaned, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (client_hpaned), 3);
  gtk_paned_set_handle_size (GTK_PANED (client_hpaned), 9);
  gtk_paned_set_gutter_size (GTK_PANED (client_hpaned), 10);
  gtk_paned_set_position (GTK_PANED (client_hpaned), 180);

  lists_vbox = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (lists_vbox);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "lists_vbox", lists_vbox,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (lists_vbox);
  gtk_paned_pack1 (GTK_PANED (client_hpaned), lists_vbox, FALSE, TRUE);

  room_scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (room_scrolledwindow);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "room_scrolledwindow", room_scrolledwindow,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (room_scrolledwindow);
  gtk_box_pack_start (GTK_BOX (lists_vbox), room_scrolledwindow, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (room_scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  room_clist = gtk_clist_new (1);
  gtk_widget_ref (room_clist);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "room_clist", room_clist,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (room_clist);
  gtk_container_add (GTK_CONTAINER (room_scrolledwindow), room_clist);
  GTK_WIDGET_UNSET_FLAGS (room_clist, GTK_CAN_FOCUS);
  gtk_clist_set_column_width (GTK_CLIST (room_clist), 0, 80);
  gtk_clist_column_titles_show (GTK_CLIST (room_clist));

  room_label = gtk_label_new ("Rooms");
  gtk_widget_ref (room_label);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "room_label", room_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (room_label);
  gtk_clist_set_column_widget (GTK_CLIST (room_clist), 0, room_label);

  player_scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (player_scrolledwindow);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "player_scrolledwindow", player_scrolledwindow,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (player_scrolledwindow);
  gtk_box_pack_start (GTK_BOX (lists_vbox), player_scrolledwindow, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (player_scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  player_clist = gtk_clist_new (3);
  gtk_widget_ref (player_clist);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "player_clist", player_clist,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (player_clist);
  gtk_container_add (GTK_CONTAINER (player_scrolledwindow), player_clist);
  GTK_WIDGET_UNSET_FLAGS (player_clist, GTK_CAN_FOCUS);
  gtk_clist_set_column_width (GTK_CLIST (player_clist), 0, 10);
  gtk_clist_set_column_width (GTK_CLIST (player_clist), 1, 10);
  gtk_clist_set_column_width (GTK_CLIST (player_clist), 2, 80);
  gtk_clist_column_titles_show (GTK_CLIST (player_clist));

  player_lag_label = gtk_label_new ("L");
  gtk_widget_ref (player_lag_label);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "player_lag_label", player_lag_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (player_lag_label);
  gtk_clist_set_column_widget (GTK_CLIST (player_clist), 0, player_lag_label);

  player_table_label = gtk_label_new ("T");
  gtk_widget_ref (player_table_label);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "player_table_label", player_table_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (player_table_label);
  gtk_clist_set_column_widget (GTK_CLIST (player_clist), 1, player_table_label);

  player_name_label = gtk_label_new ("Player");
  gtk_widget_ref (player_name_label);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "player_name_label", player_name_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (player_name_label);
  gtk_clist_set_column_widget (GTK_CLIST (player_clist), 2, player_name_label);

  table_vpaned = gtk_vpaned_new ();
  gtk_widget_ref (table_vpaned);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "table_vpaned", table_vpaned,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (table_vpaned);
  gtk_paned_pack2 (GTK_PANED (client_hpaned), table_vpaned, TRUE, TRUE);
  gtk_paned_set_handle_size (GTK_PANED (table_vpaned), 9);
  gtk_paned_set_gutter_size (GTK_PANED (table_vpaned), 10);
  gtk_paned_set_position (GTK_PANED (table_vpaned), 125);

  scrolledwindow3 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow3);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "scrolledwindow3", scrolledwindow3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow3);
  gtk_paned_pack1 (GTK_PANED (table_vpaned), scrolledwindow3, FALSE, TRUE);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow3), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  table_clist = gtk_clist_new (3);
  gtk_widget_ref (table_clist);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "table_clist", table_clist,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (table_clist);
  gtk_container_add (GTK_CONTAINER (scrolledwindow3), table_clist);
  GTK_WIDGET_UNSET_FLAGS (table_clist, GTK_CAN_FOCUS);
  gtk_clist_set_column_width (GTK_CLIST (table_clist), 0, 26);
  gtk_clist_set_column_width (GTK_CLIST (table_clist), 1, 38);
  gtk_clist_set_column_width (GTK_CLIST (table_clist), 2, 80);
  gtk_clist_column_titles_show (GTK_CLIST (table_clist));

  table_no_label = gtk_label_new ("No.");
  gtk_widget_ref (table_no_label);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "table_no_label", table_no_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (table_no_label);
  gtk_clist_set_column_widget (GTK_CLIST (table_clist), 0, table_no_label);

  table_steats_label = gtk_label_new ("Seats");
  gtk_widget_ref (table_steats_label);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "table_steats_label", table_steats_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (table_steats_label);
  gtk_clist_set_column_widget (GTK_CLIST (table_clist), 1, table_steats_label);

  tabel_desc_label = gtk_label_new ("Description");
  gtk_widget_ref (tabel_desc_label);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "tabel_desc_label", tabel_desc_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (tabel_desc_label);
  gtk_clist_set_column_widget (GTK_CLIST (table_clist), 2, tabel_desc_label);

  chat_vbox = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (chat_vbox);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "chat_vbox", chat_vbox,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chat_vbox);
  gtk_paned_pack2 (GTK_PANED (table_vpaned), chat_vbox, TRUE, TRUE);

  chatdisplay_hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (chatdisplay_hbox);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "chatdisplay_hbox", chatdisplay_hbox,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chatdisplay_hbox);
  gtk_box_pack_start (GTK_BOX (chat_vbox), chatdisplay_hbox, TRUE, TRUE, 0);

  chat_frame = gtk_frame_new (NULL);
  gtk_widget_ref (chat_frame);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "chat_frame", chat_frame,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chat_frame);
  gtk_box_pack_start (GTK_BOX (chatdisplay_hbox), chat_frame, TRUE, TRUE, 0);
  gtk_frame_set_shadow_type (GTK_FRAME (chat_frame), GTK_SHADOW_IN);

  xtext_custom = main_xtext_chat_create ("xtext_custom", NULL, NULL, 0, 0);
  gtk_widget_ref (xtext_custom);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "xtext_custom", xtext_custom,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (xtext_custom);
  gtk_container_add (GTK_CONTAINER (chat_frame), xtext_custom);
  GTK_WIDGET_UNSET_FLAGS (xtext_custom, GTK_CAN_FOCUS);
  GTK_WIDGET_UNSET_FLAGS (xtext_custom, GTK_CAN_DEFAULT);

  chat_vscrollbar = gtk_vscrollbar_new (GTK_ADJUSTMENT (gtk_adjustment_new (0, 0, 0, 0, 0, 0)));
  gtk_widget_ref (chat_vscrollbar);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "chat_vscrollbar", chat_vscrollbar,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chat_vscrollbar);
  gtk_box_pack_start (GTK_BOX (chatdisplay_hbox), chat_vscrollbar, FALSE, TRUE, 0);

  newchat_hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (newchat_hbox);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "newchat_hbox", newchat_hbox,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (newchat_hbox);
  gtk_box_pack_start (GTK_BOX (chat_vbox), newchat_hbox, FALSE, FALSE, 0);

  chat_label = gtk_label_new ("Message:");
  gtk_widget_ref (chat_label);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "chat_label", chat_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chat_label);
  gtk_box_pack_start (GTK_BOX (newchat_hbox), chat_label, FALSE, FALSE, 0);
  gtk_misc_set_padding (GTK_MISC (chat_label), 3, 0);

  chat_entry = gtk_entry_new ();
  gtk_widget_ref (chat_entry);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "chat_entry", chat_entry,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chat_entry);
  gtk_box_pack_start (GTK_BOX (newchat_hbox), chat_entry, TRUE, TRUE, 0);

  chat_hbuttonbox = gtk_hbutton_box_new ();
  gtk_widget_ref (chat_hbuttonbox);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "chat_hbuttonbox", chat_hbuttonbox,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chat_hbuttonbox);
  gtk_box_pack_start (GTK_BOX (newchat_hbox), chat_hbuttonbox, FALSE, FALSE, 0);
  gtk_button_box_set_spacing (GTK_BUTTON_BOX (chat_hbuttonbox), 0);

  send_button = gtk_button_new_with_label ("Send");
  gtk_widget_ref (send_button);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "send_button", send_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (send_button);
  gtk_container_add (GTK_CONTAINER (chat_hbuttonbox), send_button);
  GTK_WIDGET_UNSET_FLAGS (send_button, GTK_CAN_FOCUS);
  GTK_WIDGET_SET_FLAGS (send_button, GTK_CAN_DEFAULT);

  statusbar = gtk_statusbar_new ();
  gtk_widget_ref (statusbar);
  gtk_object_set_data_full (GTK_OBJECT (win_main), "statusbar", statusbar,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (statusbar);
  gtk_box_pack_start (GTK_BOX (main_vbox), statusbar, FALSE, FALSE, 0);

  gtk_signal_connect (GTK_OBJECT (win_main), "realize",
                      GTK_SIGNAL_FUNC (on_win_main_realize),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (win_main), "delete_event",
                      GTK_SIGNAL_FUNC (gtk_main_quit),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (connect), "activate",
                      GTK_SIGNAL_FUNC (on_connect_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (disconnect), "activate",
                      GTK_SIGNAL_FUNC (on_disconnect_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (exit), "activate",
                      GTK_SIGNAL_FUNC (on_exit_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (launch), "activate",
                      GTK_SIGNAL_FUNC (on_launch_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (join), "activate",
                      GTK_SIGNAL_FUNC (on_join_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (leave), "activate",
                      GTK_SIGNAL_FUNC (on_leave_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (properties), "activate",
                      GTK_SIGNAL_FUNC (on_properties_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (room_list), "activate",
                      GTK_SIGNAL_FUNC (on_room_list_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (player_list), "activate",
                      GTK_SIGNAL_FUNC (on_player_list_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (server_stats), "activate",
                      GTK_SIGNAL_FUNC (on_server_stats_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (player_stats), "activate",
                      GTK_SIGNAL_FUNC (on_player_stats_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (game_types), "activate",
                      GTK_SIGNAL_FUNC (on_game_types_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (motd), "activate",
                      GTK_SIGNAL_FUNC (on_motd_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (about), "activate",
                      GTK_SIGNAL_FUNC (on_about_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (gpl_license), "activate",
                      GTK_SIGNAL_FUNC (on_gpl_license_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (ggz_help), "activate",
                      GTK_SIGNAL_FUNC (on_ggz_help_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (game_help), "activate",
                      GTK_SIGNAL_FUNC (on_game_help_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (goto_web1), "activate",
                      GTK_SIGNAL_FUNC (on_goto_web1_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (connect_button), "clicked",
                      GTK_SIGNAL_FUNC (on_connect_button_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (disconnect_button), "clicked",
                      GTK_SIGNAL_FUNC (on_disconnect_button_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (launch_button), "clicked",
                      GTK_SIGNAL_FUNC (on_launch_button_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (join_button), "clicked",
                      GTK_SIGNAL_FUNC (on_join_button_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (props_button), "clicked",
                      GTK_SIGNAL_FUNC (on_props_button_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (stats_button), "clicked",
                      GTK_SIGNAL_FUNC (on_stats_button_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (exit_button), "clicked",
                      GTK_SIGNAL_FUNC (on_exit_button_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (room_clist), "select_row",
                      GTK_SIGNAL_FUNC (on_room_clist_select_row),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (table_clist), "select_row",
                      GTK_SIGNAL_FUNC (on_table_clist_select_row),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (table_clist), "click_column",
                      GTK_SIGNAL_FUNC (on_table_clist_click_column),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (chat_entry), "activate",
                      GTK_SIGNAL_FUNC (on_chat_entry_activate),
                      chat_entry);
  gtk_signal_connect_after (GTK_OBJECT (chat_entry), "key_press_event",
                            GTK_SIGNAL_FUNC (on_chat_entry_key_press_event),
                            NULL);
  gtk_signal_connect (GTK_OBJECT (send_button), "clicked",
                      GTK_SIGNAL_FUNC (on_send_button_clicked),
                      NULL);

  gtk_widget_grab_focus (chat_entry);
  gtk_window_add_accel_group (GTK_WINDOW (win_main), accel_group);

  return win_main;
}

GtkWidget*
create_mnu_room (void)
{
  GtkWidget *mnu_room;
  GtkAccelGroup *mnu_room_accels;
  GtkWidget *enter;
  GtkWidget *info;

  mnu_room = gtk_menu_new ();
  gtk_object_set_data (GTK_OBJECT (mnu_room), "mnu_room", mnu_room);
  mnu_room_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (mnu_room));

  enter = gtk_menu_item_new_with_label ("Enter");
  gtk_widget_ref (enter);
  gtk_object_set_data_full (GTK_OBJECT (mnu_room), "enter", enter,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (enter);
  gtk_container_add (GTK_CONTAINER (mnu_room), enter);

  info = gtk_menu_item_new_with_label ("Info");
  gtk_widget_ref (info);
  gtk_object_set_data_full (GTK_OBJECT (mnu_room), "info", info,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (info);
  gtk_container_add (GTK_CONTAINER (mnu_room), info);

  gtk_signal_connect (GTK_OBJECT (enter), "activate",
                      GTK_SIGNAL_FUNC (on_enter_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (info), "activate",
                      GTK_SIGNAL_FUNC (on_info_activate),
                      NULL);

  return mnu_room;
}

GtkWidget*
create_mnu_table (void)
{
  GtkWidget *mnu_table;
  GtkAccelGroup *mnu_table_accels;
  GtkWidget *join;
  GtkWidget *leave;
  GtkWidget *menuitem3;
  GtkWidget *info;

  mnu_table = gtk_menu_new ();
  gtk_object_set_data (GTK_OBJECT (mnu_table), "mnu_table", mnu_table);
  mnu_table_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (mnu_table));

  join = gtk_menu_item_new_with_label ("Join");
  gtk_widget_ref (join);
  gtk_object_set_data_full (GTK_OBJECT (mnu_table), "join", join,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (join);
  gtk_container_add (GTK_CONTAINER (mnu_table), join);

  leave = gtk_menu_item_new_with_label ("Leave");
  gtk_widget_ref (leave);
  gtk_object_set_data_full (GTK_OBJECT (mnu_table), "leave", leave,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (leave);
  gtk_container_add (GTK_CONTAINER (mnu_table), leave);

  menuitem3 = gtk_menu_item_new ();
  gtk_widget_ref (menuitem3);
  gtk_object_set_data_full (GTK_OBJECT (mnu_table), "menuitem3", menuitem3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (menuitem3);
  gtk_container_add (GTK_CONTAINER (mnu_table), menuitem3);
  gtk_widget_set_sensitive (menuitem3, FALSE);

  info = gtk_menu_item_new_with_label ("Info");
  gtk_widget_ref (info);
  gtk_object_set_data_full (GTK_OBJECT (mnu_table), "info", info,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (info);
  gtk_container_add (GTK_CONTAINER (mnu_table), info);

  gtk_signal_connect (GTK_OBJECT (join), "activate",
                      GTK_SIGNAL_FUNC (on_join_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (leave), "activate",
                      GTK_SIGNAL_FUNC (on_leave_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (info), "activate",
                      GTK_SIGNAL_FUNC (on_info_activate),
                      NULL);

  return mnu_table;
}

