/*
 * File: dlg_main.c
 * Author: Brent Hendricks
 * Project: GGZ Client
 * Date: 3/27/00
 *
 * Main GGZ Client window and associated callbacks
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

#include <dlg_main.h>
#include <datatypes.h>
#include <err_func.h>
#include <easysock.h>
#include <chat.h>
#include <connect.h>
#include <ggzrc.h>
#include <protocols.h>
#include <dlg_about.h>
#include <dlg_error.h>
#include <dlg_exit.h>
#include <dlg_launch.h>
#include <dlg_login.h>
#include <dlg_props.h>
#include <xtext.h>

#include "support.h"

/* Globals neaded by this dialog */
extern GdkColor colors[];
extern GtkWidget *dlg_about;
extern GtkWidget *dlg_launch;
extern GtkWidget *dlg_login;
extern GtkWidget *dlg_props;
extern GtkWidget *mnu_players;
extern GtkWidget *mnu_tables;
extern gint selected_table;
extern gint selected_type;
extern struct ConnectInfo connection;
extern struct GameTypes game_types;
extern struct Rooms room_info;

/* Global GtkWidget for this dialog */
GtkWidget *main_win;

/* Local callbacks which no other file will call */
static void ggz_join_game(GtkButton * button, gpointer user_data);
static void ggz_leave_game(GtkWidget* widget, gpointer data);
static void ggz_input_chat_msg(GtkWidget * widget, gpointer user_data);
static void ggz_props_clicked(GtkWidget * widget, gpointer user_data);
static void ggz_table_select_row_callback(GtkWidget *widget, gint row, 
					  gint column, GdkEventButton *event, 
					  gpointer data);
static void ggz_get_game_options(GtkButton * button, gpointer user_data);
static gint ggz_event_tables( GtkWidget *widget, GdkEvent *event );
static gint ggz_event_players( GtkWidget *widget, GdkEvent *event );
static void ggz_disconnect(void);
static void ggz_connect(void);
static void ggz_motd(void);
static void ggz_realize(GtkWidget* widget, gpointer data);
static void ggz_room_changed(GtkWidget* widget, gpointer data);
static void ggz_about(GtkWidget* widget, gpointer data);
GtkWidget* ggz_xtext_new (gchar *widget_name, gchar *string1, gchar *string2, gint int1, gint int2);


/*
 *Main window callbacks 
 *
 */
static void ggz_join_game(GtkButton * button, gpointer user_data)
{
	
        dbg_msg("joining game");
        es_write_int(connection.sock, REQ_TABLE_JOIN);
        es_write_int(connection.sock, selected_table);

}

static void ggz_leave_game(GtkWidget* widget, gpointer data)
{
	GtkWidget *tmp;

        dbg_msg("leaving game");
        es_write_int(connection.sock, REQ_TABLE_LEAVE);

	tmp = gtk_object_get_data(GTK_OBJECT(main_win), "msg_entry");
	gtk_widget_grab_focus(tmp);
}

void ggz_get_types(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *tmp;
        gchar verbose = 1;
                 
        es_write_int(connection.sock, REQ_LIST_TYPES);
        write(connection.sock, &verbose, 1);

	tmp = gtk_object_get_data(GTK_OBJECT(main_win), "msg_entry");
	gtk_widget_grab_focus(tmp);
}
                                
 
void ggz_get_players(GtkMenuItem * menuitem, gpointer user_data)
{
        es_write_int(connection.sock, REQ_LIST_PLAYERS);
}

        
void ggz_get_tables(GtkMenuItem * menuitem, gpointer user_data)
{       
        es_write_int(connection.sock, REQ_LIST_TABLES);
        es_write_int(connection.sock, -1);
	es_write_char(connection.sock, 0);
}

static void ggz_input_chat_msg(GtkWidget * widget, gpointer user_data)
{
	gchar *name = NULL;
	gint i;

        if (!connection.connected) {
                err_dlg(_("Not Connected"));
                return;
        }
                        
	if(strncasecmp(gtk_entry_get_text(GTK_ENTRY(user_data)), "/help", 5))
	{
	        if (strcmp(gtk_entry_get_text(GTK_ENTRY(user_data)), "") != 0
		  && es_write_int(connection.sock, REQ_CHAT) == 0)
		{
			if (!strncasecmp(gtk_entry_get_text(GTK_ENTRY(user_data)), "/msg ", 5))
			{
					name = g_strdup(gtk_entry_get_text(GTK_ENTRY(user_data))+5);
					for(i=0;i<strlen(name);i++)
					{
						if(name[i]==' ')
						{
							name[i]='\0';
							es_write_char(connection.sock, GGZ_CHAT_PERSONAL);
			        	        	es_write_string(connection.sock, name);
			        	        	es_write_string(connection.sock,
        	        	        	        	gtk_entry_get_text(GTK_ENTRY(user_data))+6+i);

							chat_print(CHAT_COLOR_SERVER, "---", "Personal message sent");
							i = strlen(name)+1;
						}
					}
			} else if (!strncasecmp(gtk_entry_get_text(GTK_ENTRY(user_data)), "/beep ", 6))
			{
				if (es_write_char(connection.sock, GGZ_CHAT_BEEP) == 0)
	        		        es_write_string(connection.sock,
                        		        gtk_entry_get_text(GTK_ENTRY(user_data))+6);
				chat_print(CHAT_COLOR_SERVER, "---", "Beep sent");
			} else if (!strncasecmp(gtk_entry_get_text(GTK_ENTRY(user_data)), "/announce ", 10))
			{
				if (es_write_char(connection.sock, GGZ_CHAT_ANNOUNCE) == 0)
	        		        es_write_string(connection.sock,
                        		        gtk_entry_get_text(GTK_ENTRY(user_data))+10);
			}else{
				if (es_write_char(connection.sock, GGZ_CHAT_NORMAL) == 0)
	        		        es_write_string(connection.sock,
                        		        gtk_entry_get_text(GTK_ENTRY(user_data)));
			}
		}
        }else{
		chat_print(CHAT_COLOR_SERVER, "---", "GNU Gaming Zone");
		chat_print(CHAT_COLOR_SERVER, "---", "   Chat Help");
		chat_print(CHAT_COLOR_SERVER, "---", " ");
		chat_print(CHAT_COLOR_SERVER, "---", "/help");
		chat_print(CHAT_COLOR_SERVER, "---", "   Display help on a given commad.");
		chat_print(CHAT_COLOR_SERVER, "---", "/me <action>");
		chat_print(CHAT_COLOR_SERVER, "---", "   Displays an action.");
		chat_print(CHAT_COLOR_SERVER, "---", "/beep <user>");
		chat_print(CHAT_COLOR_SERVER, "---", "   Makes <user>'s computer beep.");
	}
        gtk_entry_set_text(GTK_ENTRY(user_data), "");
}
                        
static void ggz_table_select_row_callback(GtkWidget *widget, gint row, gint column,
                               GdkEventButton *event, gpointer data)
{
        gchar *text;
        gint i;
        
        gtk_clist_get_text(GTK_CLIST(widget), row, 1, &text);
        selected_table = atoi(text);
        gtk_clist_get_text(GTK_CLIST(widget), row, 2, &text);
        for(i=0;i<game_types.count;i++)
        {
                if(!strcmp(text,game_types.info[i].name))
                        selected_type=i;
        }
}


static void ggz_get_game_options(GtkButton * button, gpointer user_data)
{

        if (!connection.connected)
                warn_dlg(_("Not connected!"));
        else {
                dlg_launch = create_dlg_launch();
                gtk_widget_show(dlg_launch);
        }

}

gint ggz_event_tables( GtkWidget *widget, GdkEvent *event )
{
	GtkWidget *tmp;
	int row,col;

	if (event->type == GDK_BUTTON_PRESS && event->button.button == 3)
	{
		tmp = gtk_object_get_data(GTK_OBJECT(mnu_tables), "launch1");
		gtk_widget_show(GTK_WIDGET(tmp));
		tmp = gtk_object_get_data(GTK_OBJECT(mnu_tables), "join1");
		gtk_widget_hide(GTK_WIDGET(tmp));

		tmp = gtk_object_get_data(GTK_OBJECT(main_win), "table_tree");
		if (gtk_clist_get_selection_info(GTK_CLIST(tmp), event->button.x, event->button.y, &row, &col) > 0)
		{
			tmp = gtk_object_get_data(GTK_OBJECT(mnu_tables), "launch1");
			gtk_widget_hide(GTK_WIDGET(tmp));
			tmp = gtk_object_get_data(GTK_OBJECT(mnu_tables), "join1");
			gtk_widget_show(GTK_WIDGET(tmp));

			tmp = gtk_object_get_data(GTK_OBJECT(main_win), "table_tree");
			gtk_clist_unselect_all (GTK_CLIST (tmp));
			gtk_clist_select_row (GTK_CLIST (tmp), row, 0);
		}
		gtk_menu_popup (GTK_MENU (mnu_tables), NULL, NULL, NULL, NULL,
			event->button.button, event->button.time);
        }
	if (event->type == GDK_2BUTTON_PRESS)
	{
        	es_write_int(connection.sock, REQ_TABLE_JOIN);
	        es_write_int(connection.sock, selected_table);
	}


	return 0;
}

gint ggz_event_players( GtkWidget *widget, GdkEvent *event )
{
	if (event->type == GDK_BUTTON_PRESS && event->button.button == 3)
	{
		GdkEventButton *bevent = (GdkEventButton *) event;
		gtk_menu_popup (GTK_MENU (mnu_players), NULL, NULL, NULL, NULL,
			bevent->button, bevent->time);
        }

	return 0;
}

static void ggz_disconnect(void)
{
	GtkWidget *tmp;
        dbg_msg("Logging out");
        es_write_int(connection.sock, REQ_LOGOUT);

        dbg_msg("Disconnecting");
	disconnect(NULL,NULL);
	
	login_disconnect();

	tmp = gtk_object_get_data(GTK_OBJECT(main_win), "msg_entry");
	gtk_widget_grab_focus(tmp);
}

static void ggz_connect(void)
{
	GtkWidget *tmp;

	disconnect(NULL,NULL);
	dlg_login = create_dlg_login();
	gtk_widget_show(dlg_login);

	tmp = gtk_object_get_data(GTK_OBJECT(main_win), "msg_entry");
	gtk_widget_grab_focus(tmp);
}

static void ggz_motd(void)
{
	GtkWidget *tmp;

        dbg_msg("Requestiong the MOTD");
        es_write_int(connection.sock, REQ_MOTD);

	tmp = gtk_object_get_data(GTK_OBJECT(main_win), "msg_entry");
	gtk_widget_grab_focus(tmp);
}

static void ggz_realize(GtkWidget* widget, gpointer data)
{
	GtkXText *tmp;
	char *buf;

	tmp = gtk_object_get_data(GTK_OBJECT(main_win), "chat_text");
	gtk_xtext_set_font(GTK_XTEXT(tmp), NULL, 0);
	gtk_xtext_set_palette (GTK_XTEXT(tmp), colors);
	tmp->auto_indent = ggzrc_read_int("CHAT","AutoIndent",TRUE);
	tmp->wordwrap = ggzrc_read_int("CHAT","WordWrap",TRUE);
	tmp->max_auto_indent = 50;
	tmp->time_stamp = ggzrc_read_int("CHAT","Timestamp",FALSE);
	gtk_xtext_refresh(tmp);

	buf = g_strdup_printf("Client Version:\00314 %s",VERSION);
	gtk_xtext_append_indent(tmp,"---",3,buf,strlen(buf));
	g_free(buf);
	buf = g_strdup_printf("GTK+ Version:\00314 %d.%d.%d\n",
		gtk_major_version, gtk_minor_version, gtk_micro_version);
	gtk_xtext_append_indent(tmp,"---",3,buf,strlen(buf));
	g_free(buf);

	gtk_xtext_append_indent(tmp,"---",3,"Options:", 8);
#ifdef DEBUG
	gtk_xtext_append_indent(tmp,"---",3,"  Debug", 7);
#else
	gtk_xtext_append_indent(tmp,"---",3,"  No Debug", 10);
#endif
#ifdef DEBUG_SOCKET
	gtk_xtext_append_indent(tmp,"---",3,"  Socket Debug", 14);
#else
	gtk_xtext_append_indent(tmp,"---",3,"  No Socket Debug", 17);
#endif
}

void ggz_room_changed(GtkWidget* widget, gpointer data)
{
	GtkWidget *tmp;
	gint i;

	tmp = gtk_object_get_data(GTK_OBJECT(main_win), "room_entry");
	if (connection.cur_room == connection.new_room)
	{
		connection.new_room=1;
		for(i=0;i<room_info.count;i++)
		{
			if(!strcmp(room_info.info[i].name, gtk_entry_get_text(GTK_ENTRY(tmp))))
				connection.new_room=i;
		}
		es_write_int(connection.sock, REQ_ROOM_JOIN);
		es_write_int(connection.sock, connection.new_room);
	}else if (connection.cur_room == -3){
		/* Insert Rooms From Server */
		connection.cur_room++;
	}else if (connection.cur_room == -2){
		/* Blank out room to start */
		connection.cur_room++;
	}else{
		connection.new_room=connection.cur_room;
	}	

	tmp = gtk_object_get_data(GTK_OBJECT(main_win), "msg_entry");
	gtk_widget_grab_focus(tmp);
}

void ggz_about(GtkWidget* widget, gpointer data)
{
	GtkWidget *dlg_about;
	GtkWidget *tmp;
	
	tmp = gtk_object_get_data(GTK_OBJECT(main_win), "about");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);

	dlg_about = create_dlg_about();
	gtk_widget_show(dlg_about);
}

static void ggz_props_clicked(GtkWidget * widget, gpointer user_data)
{
	GtkWidget *tmp;

	dlg_props = create_dlg_props();
	gtk_widget_show(dlg_props);

	tmp = gtk_object_get_data(GTK_OBJECT(main_win), "props_button");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
	tmp = gtk_object_get_data(GTK_OBJECT(main_win), "properties");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
}

GtkWidget* ggz_xtext_new (gchar *widget_name, gchar *string1, gchar *string2, gint int1, gint int2)
{
	GtkWidget *chat_text;
	chat_text = gtk_xtext_new (TRUE, TRUE);
	return chat_text;
}


GtkWidget*
create_main_win (void)
{
  GtkWidget *main_win;
  GtkWidget *main_box;
  GtkWidget *menu_bar;
  guint tmp_key;
  GtkWidget *ggz_state;
  GtkWidget *ggz_state_menu;
  GtkAccelGroup *ggz_state_menu_accels;
  GtkWidget *connect;
  GtkWidget *disconnect;
  GtkWidget *separator2;
  GtkWidget *exit;
  GtkWidget *game;
  GtkWidget *game_menu;
  GtkAccelGroup *game_menu_accels;
  GtkWidget *launch;
  GtkWidget *join;
  GtkWidget *leave;
  GtkWidget *edit;
  GtkWidget *edit_menu;
  GtkAccelGroup *edit_menu_accels;
  GtkWidget *properties;
  GtkWidget *view;
  GtkWidget *view_menu;
  GtkAccelGroup *view_menu_accels;
  GtkWidget *types;
  GtkWidget *motd;
  GtkWidget *help;
  GtkWidget *help_menu;
  GtkAccelGroup *help_menu_accels;
  GtkWidget *about;
  GtkWidget *toolbar;
  GtkWidget *connect_button;
  GtkWidget *disconnect_button;
  GtkWidget *launch_button;
  GtkWidget *join_button;
  GtkWidget *props_button;
  GtkWidget *stats_button;
  GtkWidget *exit_button;
  GtkWidget *v_pane;
  GtkWidget *table_box;
  GtkWidget *table_label;
  GtkWidget *table_scroll;
  GtkWidget *table_tree;
  GtkWidget *blank_label;
  GtkWidget *table_num_label;
  GtkWidget *game_type_label;
  GtkWidget *seats_label;
  GtkWidget *open_seats_label;
  GtkWidget *humans_label;
  GtkWidget *desc_label;
  GtkWidget *h_pane;
  GtkWidget *player_box;
  GtkWidget *player_label;
  GtkWidget *player_scroll;
  GtkWidget *player_list;
  GtkWidget *p_name_label;
  GtkWidget *p_table_label;
  GtkWidget *chat_box;
  GtkWidget *room_box;
  GtkWidget *room_label;
  GtkWidget *room_combo;
  GtkWidget *room_entry;
  GtkWidget *hbox1;
  GtkWidget *chat_text;
  GtkWidget *chat_vscroll;
  GtkWidget *chat_separator;
  GtkWidget *msg_box;
  GtkWidget *msg_label;
  GtkWidget *msg_entry;
  GtkWidget *msg_button;
  GtkWidget *statusbar;
  GtkAccelGroup *accel_group;

  accel_group = gtk_accel_group_new ();

  main_win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_object_set_data (GTK_OBJECT (main_win), "main_win", main_win);
  gtk_widget_set_usize (main_win, 602, -2);
  gtk_window_set_title (GTK_WINDOW (main_win), _("Gnu Gaming Zone"));
  gtk_window_set_default_size (GTK_WINDOW (main_win), 500, 450);

  main_box = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (main_box);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "main_box", main_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (main_box);
  gtk_container_add (GTK_CONTAINER (main_win), main_box);

  menu_bar = gtk_menu_bar_new ();
  gtk_widget_ref (menu_bar);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "menu_bar", menu_bar,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (menu_bar);
  gtk_box_pack_start (GTK_BOX (main_box), menu_bar, FALSE, FALSE, 0);

  ggz_state = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (ggz_state)->child),
                                   _("_GGZ"));
  gtk_widget_add_accelerator (ggz_state, "activate_item", accel_group,
                              tmp_key, GDK_MOD1_MASK, 0);
  gtk_widget_ref (ggz_state);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "ggz_state", ggz_state,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (ggz_state);
  gtk_container_add (GTK_CONTAINER (menu_bar), ggz_state);

  ggz_state_menu = gtk_menu_new ();
  gtk_widget_ref (ggz_state_menu);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "ggz_state_menu", ggz_state_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (ggz_state), ggz_state_menu);
  ggz_state_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (ggz_state_menu));

  connect = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (connect)->child),
                                   _("_Connect"));
  gtk_widget_add_accelerator (connect, "activate_item", ggz_state_menu_accels,
                              tmp_key, 0, 0);
  gtk_widget_ref (connect);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "connect", connect,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (connect);
  gtk_container_add (GTK_CONTAINER (ggz_state_menu), connect);
  gtk_widget_add_accelerator (connect, "activate", accel_group,
                              GDK_C, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  disconnect = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (disconnect)->child),
                                   _("_Disconnect"));
  gtk_widget_add_accelerator (disconnect, "activate_item", ggz_state_menu_accels,
                              tmp_key, 0, 0);
  gtk_widget_ref (disconnect);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "disconnect", disconnect,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (disconnect);
  gtk_container_add (GTK_CONTAINER (ggz_state_menu), disconnect);
  gtk_widget_add_accelerator (disconnect, "activate", accel_group,
                              GDK_D, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  separator2 = gtk_menu_item_new ();
  gtk_widget_ref (separator2);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "separator2", separator2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (separator2);
  gtk_container_add (GTK_CONTAINER (ggz_state_menu), separator2);
  gtk_widget_set_sensitive (separator2, FALSE);

  exit = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (exit)->child),
                                   _("E_xit"));
  gtk_widget_add_accelerator (exit, "activate_item", ggz_state_menu_accels,
                              tmp_key, 0, 0);
  gtk_widget_ref (exit);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "exit", exit,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (exit);
  gtk_container_add (GTK_CONTAINER (ggz_state_menu), exit);
  gtk_widget_add_accelerator (exit, "activate", accel_group,
                              GDK_X, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  game = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (game)->child),
                                   _("G_ame"));
  gtk_widget_add_accelerator (game, "activate_item", accel_group,
                              tmp_key, GDK_MOD1_MASK, 0);
  gtk_widget_ref (game);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "game", game,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (game);
  gtk_container_add (GTK_CONTAINER (menu_bar), game);

  game_menu = gtk_menu_new ();
  gtk_widget_ref (game_menu);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "game_menu", game_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (game), game_menu);
  game_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (game_menu));

  launch = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (launch)->child),
                                   _("_Launch Game"));
  gtk_widget_add_accelerator (launch, "activate_item", game_menu_accels,
                              tmp_key, 0, 0);
  gtk_widget_ref (launch);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "launch", launch,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (launch);
  gtk_container_add (GTK_CONTAINER (game_menu), launch);
  gtk_widget_add_accelerator (launch, "activate", accel_group,
                              GDK_L, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  join = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (join)->child),
                                   _("_Join Game"));
  gtk_widget_add_accelerator (join, "activate_item", game_menu_accels,
                              tmp_key, 0, 0);
  gtk_widget_ref (join);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "join", join,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (join);
  gtk_container_add (GTK_CONTAINER (game_menu), join);

  leave = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (leave)->child),
                                   _("Lea_ve Game"));
  gtk_widget_add_accelerator (leave, "activate_item", game_menu_accels,
                              tmp_key, 0, 0);
  gtk_widget_ref (leave);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "leave", leave,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (leave);
  gtk_container_add (GTK_CONTAINER (game_menu), leave);

  edit = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (edit)->child),
                                   _("_Edit"));
  gtk_widget_add_accelerator (edit, "activate_item", accel_group,
                              tmp_key, GDK_MOD1_MASK, 0);
  gtk_widget_ref (edit);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "edit", edit,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (edit);
  gtk_container_add (GTK_CONTAINER (menu_bar), edit);

  edit_menu = gtk_menu_new ();
  gtk_widget_ref (edit_menu);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "edit_menu", edit_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (edit), edit_menu);
  edit_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (edit_menu));

  properties = gtk_menu_item_new_with_label (_("Properties"));
  gtk_widget_ref (properties);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "properties", properties,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (properties);
  gtk_container_add (GTK_CONTAINER (edit_menu), properties);

  view = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (view)->child),
                                   _("_View"));
  gtk_widget_add_accelerator (view, "activate_item", accel_group,
                              tmp_key, GDK_MOD1_MASK, 0);
  gtk_widget_ref (view);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "view", view,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (view);
  gtk_container_add (GTK_CONTAINER (menu_bar), view);

  view_menu = gtk_menu_new ();
  gtk_widget_ref (view_menu);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "view_menu", view_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (view), view_menu);
  view_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (view_menu));

  types = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (types)->child),
                                   _("Game _Types"));
  gtk_widget_add_accelerator (types, "activate_item", view_menu_accels,
                              tmp_key, 0, 0);
  gtk_widget_ref (types);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "types", types,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (types);
  gtk_container_add (GTK_CONTAINER (view_menu), types);

  motd = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (motd)->child),
                                   _("_MOTD"));
  gtk_widget_add_accelerator (motd, "activate_item", view_menu_accels,
                              tmp_key, 0, 0);
  gtk_widget_ref (motd);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "motd", motd,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (motd);
  gtk_container_add (GTK_CONTAINER (view_menu), motd);

  help = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (help)->child),
                                   _("_Help"));
  gtk_widget_add_accelerator (help, "activate_item", accel_group,
                              tmp_key, GDK_MOD1_MASK, 0);
  gtk_widget_ref (help);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "help", help,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (help);
  gtk_container_add (GTK_CONTAINER (menu_bar), help);
  gtk_menu_item_right_justify (GTK_MENU_ITEM (help));

  help_menu = gtk_menu_new ();
  gtk_widget_ref (help_menu);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "help_menu", help_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (help), help_menu);
  help_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (help_menu));

  about = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (about)->child),
                                   _("_About"));
  gtk_widget_add_accelerator (about, "activate_item", help_menu_accels,
                              tmp_key, 0, 0);
  gtk_widget_ref (about);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "about", about,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (about);
  gtk_container_add (GTK_CONTAINER (help_menu), about);

  toolbar = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_TEXT);
  gtk_widget_ref (toolbar);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "toolbar", toolbar,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (toolbar);
  gtk_box_pack_start (GTK_BOX (main_box), toolbar, FALSE, FALSE, 0);

  connect_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("Connect"),
                                NULL, NULL,
                                NULL, NULL, NULL);
  gtk_widget_ref (connect_button);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "connect_button", connect_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (connect_button);

  disconnect_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("Disconnect"),
                                NULL, NULL,
                                NULL, NULL, NULL);
  gtk_widget_ref (disconnect_button);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "disconnect_button", disconnect_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (disconnect_button);

  launch_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("Launch Game"),
                                NULL, NULL,
                                NULL, NULL, NULL);
  gtk_widget_ref (launch_button);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "launch_button", launch_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (launch_button);

  join_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("Join Game"),
                                NULL, NULL,
                                NULL, NULL, NULL);
  gtk_widget_ref (join_button);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "join_button", join_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (join_button);

  props_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("Change Props"),
                                NULL, NULL,
                                NULL, NULL, NULL);
  gtk_widget_ref (props_button);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "props_button", props_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (props_button);

  stats_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("Stats"),
                                NULL, NULL,
                                NULL, NULL, NULL);
  gtk_widget_ref (stats_button);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "stats_button", stats_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (stats_button);

  exit_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("Exit"),
                                NULL, NULL,
                                NULL, NULL, NULL);
  gtk_widget_ref (exit_button);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "exit_button", exit_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (exit_button);

  v_pane = gtk_vpaned_new ();
  gtk_widget_ref (v_pane);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "v_pane", v_pane,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (v_pane);
  gtk_box_pack_start (GTK_BOX (main_box), v_pane, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (v_pane), 10);
  gtk_paned_set_gutter_size (GTK_PANED (v_pane), 15);
  gtk_paned_set_position (GTK_PANED (v_pane), 160);

  table_box = gtk_vbox_new (FALSE, 5);
  gtk_widget_ref (table_box);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "table_box", table_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (table_box);
  gtk_paned_pack1 (GTK_PANED (v_pane), table_box, FALSE, TRUE);

  table_label = gtk_label_new (_("Game Tables"));
  gtk_widget_ref (table_label);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "table_label", table_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (table_label);
  gtk_box_pack_start (GTK_BOX (table_box), table_label, FALSE, FALSE, 0);

  table_scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (table_scroll);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "table_scroll", table_scroll,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (table_scroll);
  gtk_box_pack_start (GTK_BOX (table_box), table_scroll, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (table_scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  table_tree = gtk_clist_new (7);
  gtk_widget_ref (table_tree);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "table_tree", table_tree,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (table_tree);
  gtk_container_add (GTK_CONTAINER (table_scroll), table_tree);
  gtk_clist_set_column_width (GTK_CLIST (table_tree), 0, 12);
  gtk_clist_set_column_width (GTK_CLIST (table_tree), 1, 64);
  gtk_clist_set_column_width (GTK_CLIST (table_tree), 2, 80);
  gtk_clist_set_column_width (GTK_CLIST (table_tree), 3, 52);
  gtk_clist_set_column_width (GTK_CLIST (table_tree), 4, 72);
  gtk_clist_set_column_width (GTK_CLIST (table_tree), 5, 60);
  gtk_clist_set_column_width (GTK_CLIST (table_tree), 6, 162);
  gtk_clist_column_titles_show (GTK_CLIST (table_tree));

  blank_label = gtk_label_new (_(" "));
  gtk_widget_ref (blank_label);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "blank_label", blank_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (blank_label);
  gtk_clist_set_column_widget (GTK_CLIST (table_tree), 0, blank_label);

  table_num_label = gtk_label_new (_("Table No."));
  gtk_widget_ref (table_num_label);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "table_num_label", table_num_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (table_num_label);
  gtk_clist_set_column_widget (GTK_CLIST (table_tree), 1, table_num_label);

  game_type_label = gtk_label_new (_("Game Type"));
  gtk_widget_ref (game_type_label);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "game_type_label", game_type_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (game_type_label);
  gtk_clist_set_column_widget (GTK_CLIST (table_tree), 2, game_type_label);

  seats_label = gtk_label_new (_("Seats"));
  gtk_widget_ref (seats_label);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "seats_label", seats_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seats_label);
  gtk_clist_set_column_widget (GTK_CLIST (table_tree), 3, seats_label);

  open_seats_label = gtk_label_new (_("Open Seats"));
  gtk_widget_ref (open_seats_label);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "open_seats_label", open_seats_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (open_seats_label);
  gtk_clist_set_column_widget (GTK_CLIST (table_tree), 4, open_seats_label);

  humans_label = gtk_label_new (_("Humans"));
  gtk_widget_ref (humans_label);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "humans_label", humans_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (humans_label);
  gtk_clist_set_column_widget (GTK_CLIST (table_tree), 5, humans_label);

  desc_label = gtk_label_new (_("Description"));
  gtk_widget_ref (desc_label);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "desc_label", desc_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (desc_label);
  gtk_clist_set_column_widget (GTK_CLIST (table_tree), 6, desc_label);

  h_pane = gtk_hpaned_new ();
  gtk_widget_ref (h_pane);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "h_pane", h_pane,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (h_pane);
  gtk_paned_pack2 (GTK_PANED (v_pane), h_pane, TRUE, TRUE);
  gtk_container_set_border_width (GTK_CONTAINER (h_pane), 5);
  gtk_paned_set_gutter_size (GTK_PANED (h_pane), 10);
  gtk_paned_set_position (GTK_PANED (h_pane), 160);

  player_box = gtk_vbox_new (FALSE, 5);
  gtk_widget_ref (player_box);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "player_box", player_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (player_box);
  gtk_paned_pack1 (GTK_PANED (h_pane), player_box, FALSE, TRUE);

  player_label = gtk_label_new (_("Players"));
  gtk_widget_ref (player_label);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "player_label", player_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (player_label);
  gtk_box_pack_start (GTK_BOX (player_box), player_label, FALSE, FALSE, 0);

  player_scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (player_scroll);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "player_scroll", player_scroll,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (player_scroll);
  gtk_box_pack_start (GTK_BOX (player_box), player_scroll, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (player_scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  player_list = gtk_clist_new (2);
  gtk_widget_ref (player_list);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "player_list", player_list,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (player_list);
  gtk_container_add (GTK_CONTAINER (player_scroll), player_list);
  gtk_clist_set_column_width (GTK_CLIST (player_list), 0, 80);
  gtk_clist_set_column_width (GTK_CLIST (player_list), 1, 48);
  gtk_clist_column_titles_show (GTK_CLIST (player_list));

  p_name_label = gtk_label_new (_("Name"));
  gtk_widget_ref (p_name_label);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "p_name_label", p_name_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (p_name_label);
  gtk_clist_set_column_widget (GTK_CLIST (player_list), 0, p_name_label);

  p_table_label = gtk_label_new (_("Table"));
  gtk_widget_ref (p_table_label);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "p_table_label", p_table_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (p_table_label);
  gtk_clist_set_column_widget (GTK_CLIST (player_list), 1, p_table_label);

  chat_box = gtk_vbox_new (FALSE, 5);
  gtk_widget_ref (chat_box);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "chat_box", chat_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chat_box);
  gtk_paned_pack2 (GTK_PANED (h_pane), chat_box, TRUE, FALSE);
  gtk_widget_set_usize (chat_box, -2, 280);
  gtk_container_set_border_width (GTK_CONTAINER (chat_box), 5);

  room_box = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (room_box);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "room_box", room_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (room_box);
  gtk_box_pack_start (GTK_BOX (chat_box), room_box, FALSE, FALSE, 0);

  room_label = gtk_label_new (_("Current Room  "));
  gtk_widget_ref (room_label);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "room_label", room_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (room_label);
  gtk_box_pack_start (GTK_BOX (room_box), room_label, FALSE, FALSE, 0);

  room_combo = gtk_combo_new ();
  gtk_widget_ref (room_combo);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "room_combo", room_combo,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (room_combo);
  gtk_box_pack_start (GTK_BOX (room_box), room_combo, TRUE, TRUE, 0);

  room_entry = GTK_COMBO (room_combo)->entry;
  gtk_widget_ref (room_entry);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "room_entry", room_entry,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (room_entry);
  gtk_entry_set_editable (GTK_ENTRY (room_entry), FALSE);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox1);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "hbox1", hbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (chat_box), hbox1, TRUE, TRUE, 0);

  chat_text = ggz_xtext_new ("chat_text", NULL, NULL, 0, 0);
  gtk_widget_ref (chat_text);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "chat_text", chat_text,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chat_text);
  gtk_box_pack_start (GTK_BOX (hbox1), chat_text, TRUE, TRUE, 0);
  GTK_WIDGET_UNSET_FLAGS (chat_text, GTK_CAN_FOCUS);
  GTK_WIDGET_UNSET_FLAGS (chat_text, GTK_CAN_DEFAULT);

  chat_vscroll = gtk_vscrollbar_new (GTK_XTEXT(chat_text)->adj);
  gtk_widget_ref (chat_vscroll);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "chat_vscroll", chat_vscroll,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chat_vscroll);
  gtk_box_pack_start (GTK_BOX (hbox1), chat_vscroll, FALSE, TRUE, 0);

  chat_separator = gtk_hseparator_new ();
  gtk_widget_ref (chat_separator);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "chat_separator", chat_separator,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chat_separator);
  gtk_box_pack_start (GTK_BOX (chat_box), chat_separator, FALSE, FALSE, 5);

  msg_box = gtk_hbox_new (FALSE, 5);
  gtk_widget_ref (msg_box);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "msg_box", msg_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (msg_box);
  gtk_box_pack_start (GTK_BOX (chat_box), msg_box, FALSE, FALSE, 0);

  msg_label = gtk_label_new (_("Message: "));
  gtk_widget_ref (msg_label);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "msg_label", msg_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (msg_label);
  gtk_box_pack_start (GTK_BOX (msg_box), msg_label, FALSE, FALSE, 0);

  msg_entry = gtk_entry_new_with_max_length (512);
  gtk_widget_ref (msg_entry);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "msg_entry", msg_entry,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (msg_entry);
  gtk_box_pack_start (GTK_BOX (msg_box), msg_entry, TRUE, TRUE, 0);

  msg_button = gtk_button_new_with_label (_("Send"));
  gtk_widget_ref (msg_button);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "msg_button", msg_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (msg_button);
  gtk_box_pack_start (GTK_BOX (msg_box), msg_button, FALSE, FALSE, 0);

  statusbar = gtk_statusbar_new ();
  gtk_widget_ref (statusbar);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "statusbar", statusbar,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (statusbar);
  gtk_box_pack_end (GTK_BOX (main_box), statusbar, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (statusbar), 5);

  gtk_signal_connect (GTK_OBJECT (main_win), "delete_event",
                      GTK_SIGNAL_FUNC (exit_dlg),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (main_win), "destroy",
                      GTK_SIGNAL_FUNC (gtk_main_quit),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (main_win), "realize",
                      GTK_SIGNAL_FUNC (ggz_realize),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (connect), "activate",
                      GTK_SIGNAL_FUNC (ggz_connect),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (disconnect), "activate",
                      GTK_SIGNAL_FUNC (ggz_disconnect),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (exit), "activate",
                      GTK_SIGNAL_FUNC (exit_dlg),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (launch), "activate",
                      GTK_SIGNAL_FUNC (ggz_get_game_options),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (join), "activate",
                      GTK_SIGNAL_FUNC (ggz_join_game),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (leave), "activate",
                      GTK_SIGNAL_FUNC (ggz_leave_game),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (properties), "activate",
                      GTK_SIGNAL_FUNC (ggz_props_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (types), "activate",
                      GTK_SIGNAL_FUNC (ggz_get_types),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (motd), "activate",
                      GTK_SIGNAL_FUNC (ggz_motd),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (about), "activate",
                      GTK_SIGNAL_FUNC (ggz_about),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (connect_button), "clicked",
                      GTK_SIGNAL_FUNC (ggz_connect),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (disconnect_button), "clicked",
                      GTK_SIGNAL_FUNC (ggz_disconnect),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (launch_button), "clicked",
                      GTK_SIGNAL_FUNC (ggz_get_game_options),
                      main_win);
  gtk_signal_connect (GTK_OBJECT (join_button), "clicked",
                      GTK_SIGNAL_FUNC (ggz_join_game),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (props_button), "clicked",
                      GTK_SIGNAL_FUNC (ggz_props_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (exit_button), "clicked",
                      GTK_SIGNAL_FUNC (exit_dlg),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (table_tree), "select_row",
                      GTK_SIGNAL_FUNC (ggz_table_select_row_callback),
                      NULL);
  gtk_signal_connect_object (GTK_OBJECT (table_tree), "event",
                             GTK_SIGNAL_FUNC (ggz_event_tables),
                             GTK_OBJECT (mnu_tables));
  gtk_signal_connect_object (GTK_OBJECT (player_list), "event",
                             GTK_SIGNAL_FUNC (ggz_event_players),
                             GTK_OBJECT (mnu_players));
  gtk_signal_connect (GTK_OBJECT (room_entry), "changed",
                      GTK_SIGNAL_FUNC (ggz_room_changed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (msg_entry), "activate",
                      GTK_SIGNAL_FUNC (ggz_input_chat_msg),
                      msg_entry);
  gtk_signal_connect (GTK_OBJECT (msg_button), "clicked",
                      GTK_SIGNAL_FUNC (ggz_input_chat_msg),
                      msg_entry);

  gtk_window_add_accel_group (GTK_WINDOW (main_win), accel_group);

  return main_win;
}
