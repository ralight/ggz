/*
 * File: client.c
 * Author: Justin Zaun
 * Project: GGZ GTK Client
 * $Id: client.c 10939 2009-07-13 19:34:11Z josef $
 * 
 * This is the main program body for the GGZ client
 * 
 * Copyright (C) 2000-2002 Justin Zaun.
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

#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <ggz.h> /* For list functions */
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <ggzcore.h>

#include "about.h"
#include "client.h"
#include "chat.h"
#include "first.h"
#include "game.h"
#include "ggzclient.h"
#include "ggzutils.h"
#include "ggz-embed.h"
#include "ggz-gtk.h"
#include "playerlist.h"
#include "roominfo.h"
#include "launch.h"
#include "license.h"
#include "login.h"
#include "msgbox.h"
#include "props.h"
#include "roomlist.h"
#include "server.h"
#include "support.h"
#include "tablelist.h"
#include "types.h"
#include "xtext.h"

struct ggz_gtk ggz_gtk;

/* Maximum cache size for last entries */
#define CHAT_MAXIMUM_CACHE 5

/* Callbacks for main client window */
static void client_realize(GtkWidget *widget, gpointer data);
static void client_connect_activate(GtkMenuItem *menuitem, gpointer data);
static void client_disconnect_activate(GtkMenuItem *menuitem, gpointer data);
static void client_exit_activate(GtkMenuItem *menuitem, gpointer data);
static void client_launch_activate(GtkMenuItem *menuitem, gpointer data);
static void client_joinm_activate(GtkMenuItem *menuitem, gpointer data);
static void client_watchm_activate(GtkMenuItem *menuitem, gpointer data);
static void client_leave_activate(GtkMenuItem *menuitem, gpointer data);
static void client_properties_activate(GtkMenuItem *menuitem, gpointer data);
static void client_game_types_activate(GtkMenuItem *menuitem, gpointer data);
static void client_motd_activate(GtkMenuItem *menuitem, gpointer data);
static void client_about_activate(GtkMenuItem *menuitem, gpointer data);
static void client_license_activate(GtkMenuItem *menuitem, gpointer data);
static void client_ggz_help_activate(GtkMenuItem *menuitem, gpointer data);
static void client_game_help_activate(GtkMenuItem *menuitem, gpointer data);
static void client_goto_web1_activate(GtkMenuItem *menuitem, gpointer data);
static void client_launch_button_clicked(GtkButton *button, gpointer data);
static void client_join_button_clicked(GtkButton *button, gpointer data);
static void client_watch_button_clicked(GtkButton *button, gpointer data);
static void client_leave_button_clicked(GtkButton *button, gpointer data);
static void client_props_button_clicked(GtkButton *button, gpointer data);

static void client_chat_entry_activate(GtkEditable *editable, gpointer data);
static gboolean client_chat_entry_key_press_event(GtkWidget *widget, 
						  GdkEventKey *event,
						  gpointer data);
static void client_send_button_clicked(GtkButton *button, gpointer data);
static void client_start_table_watch(void);
static void client_tables_size_request(GtkWidget *widget, gpointer data);

/* Extra helper functions. */
static void try_to_quit(void);


static void
client_connect_activate			(GtkMenuItem	*menuitem,
					 gpointer	 data)
{
	main_activate();
}


static void
client_disconnect_activate		(GtkMenuItem	*menuitem,
					 gpointer	 data)
{
	if (ggzcore_server_logout(ggz_gtk.server) < 0)
		ggz_error_msg("Error logging out in "
		              "client_disconnect_activate");

	clear_room_list();
	clear_player_list();
	clear_table_list();

	/*ggz_sensitivity_init();*/
}


/* This is called by client_exit_activate and client_exit_button_clicked */
static void
try_to_quit			(void)
{
	if (ggz_connection_query() == 0
            || msgbox(_("Are you sure you want to quit?"), _("Quit?"),
                      MSGBOX_YESNO, MSGBOX_QUESTION, MSGBOX_MODAL) ==MSGBOX_YES)
	{
		gtk_main_quit();
	}
}


static void
client_exit_activate		(GtkMenuItem	*menuitem,
				 gpointer	 data)
{
	try_to_quit();
}

static void
client_launch_activate		(GtkMenuItem	*menuitem,
				 gpointer	 data)
{
	launch_create_or_raise();
}


static void
client_joinm_activate		(GtkMenuItem	*menuitem,
				 gpointer	 data)
{
	client_start_table_join();
}


static void
client_watchm_activate		(GtkMenuItem	*menuitem,
				 gpointer	 data)
{
	client_start_table_watch();
}


static void
client_leave_activate		(GtkMenuItem	*menuitem,
				 gpointer	 data)
{
	ggz_embed_leave_table();
}


static void
client_properties_activate	(GtkMenuItem	*menuitem,
				 gpointer	 data)
{
	props_raise();
}


static void widget_toggle_activate(GtkMenuItem *menuitem, gpointer data)
{
	GtkWidget *tmp = data;

	if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menuitem)))
		gtk_widget_show_all(tmp);
	else
		gtk_widget_hide(tmp);
}


#ifdef STATS
static void
client_server_stats_activate		(GtkMenuItem	*menuitem,
					 gpointer	 data)
{
	msgbox(_("Server stats are not implemented yet. If\n"
		 "you would like to help head over to\n"
		 "http://www.ggzgamingzone.org/"), _("Not Implemented"),
	       MSGBOX_OKONLY, MSGBOX_NONE, MSGBOX_NORMAL);
}


static void
client_player_stats_activate		(GtkMenuItem	*menuitem,
					 gpointer	 data)
{
	msgbox(_("Player stats are not implemented yet. If\n"
		 "you would like to help head over to\n"
		 "http://www.ggzgamingzone.org/"),
	       _("Not Implemented"),
	       MSGBOX_OKONLY, MSGBOX_NONE, MSGBOX_NORMAL);
}
#endif


static void
client_game_types_activate		(GtkMenuItem	*menuitem,
					 gpointer	 data)
{
	types_create_or_raise();
}


static void
client_motd_activate			(GtkMenuItem	*menuitem,
					 gpointer	 data)
{
	ggzcore_server_motd(ggz_gtk.server);
}


static void
client_about_activate			(GtkMenuItem	*menuitem,
					 gpointer	 data)
{
	about_create_or_raise();
}


static void
client_license_activate			(GtkMenuItem	*menuitem,
					 gpointer	 data)
{
	license_create_or_raise();
}


static void
client_ggz_help_activate		(GtkMenuItem	*menuitem,
					 gpointer	 data)
{
	if(!support_goto_url(GGZGTKDATADIR "/help/ggz-gtk-handbook.html")) {
		msgbox(_("GGZ Gaming Zone help needs a browser to be "
			 "configured.\n"
			 "The configuration dialog will be invoked now."),
		       "Help configuration",
		       MSGBOX_OKONLY, MSGBOX_NONE, MSGBOX_NORMAL);
		props_raise();
	}
}


static void
client_game_help_activate		(GtkMenuItem	*menuitem,
					 gpointer	 data)
{
	msgbox(_("Inline game help is not implemented yet. Help\n"
		 "is on our website. If\n"
		 "you would like to help head over to\n"
		 "http://www.ggzgamingzone.org/"), _("Not Implemented"),
	       MSGBOX_OKONLY, MSGBOX_NONE, MSGBOX_NORMAL);
}


static void
client_goto_web1_activate		(GtkMenuItem	*menuitem,
					 gpointer	 data)
{
	support_goto_url("http://www.ggzgamingzone.org/");
}

static GtkWidget*
main_xtext_chat_create			(gchar		*widget_name,
					 gchar		*string1,
					 gchar		*string2,
					 gint		 int1,
					 gint		 int2)
{
        GtkWidget *chat_text;

        chat_text = create_xtext();
        return chat_text;
}

static void
client_disconnect_button_clicked	(GtkButton	*button,
					 gpointer	 data)
{
	if (ggzcore_server_logout(ggz_gtk.server) < 0)
		ggz_error_msg("Error logging out in "
		              "client_disconnect_button_clicked");

	clear_room_list();
	clear_player_list();
	clear_table_list();

	/*ggz_sensitivity_init();*/
}


static void chat_line_entered(void)
{
	GtkEntry *tmp;
	GGZList *last_list; /* List for last entries */

	tmp = g_object_get_data(G_OBJECT(ggz_gtk.win_main), "chat_entry");
	last_list = g_object_get_data(G_OBJECT(tmp), "last_list");

	while (ggz_list_count(last_list) > CHAT_MAXIMUM_CACHE)
		ggz_list_delete_entry(last_list, ggz_list_head(last_list));

	if (strcmp(gtk_entry_get_text(GTK_ENTRY(tmp)),"")) {
		const char *text = gtk_entry_get_text(GTK_ENTRY(tmp));
		chat_send(text);
		ggz_list_insert(last_list, (char*)text);
		/* Clear the current entry */
		g_object_set_data(G_OBJECT(tmp), "current_entry", NULL);
	}

	/* Clear the entry box */
	gtk_entry_set_text(GTK_ENTRY(tmp), "");
}


static void
client_chat_entry_activate		(GtkEditable	*editable,
					 gpointer	 data)
{
	chat_line_entered();
}


gboolean
client_chat_entry_key_press_event	(GtkWidget	*widget,
					 GdkEventKey	*event,
					 gpointer	 data)
{
	GtkWidget *tmp;
	gint x, i, max, length, first = TRUE;
	const gchar *name = NULL, *text = NULL, *startname = NULL;
	GGZList *last_list;
	GGZListEntry *entry;

	if (event->keyval == GDK_Tab) {
		int match;

		/* Get start of name */
		tmp = g_object_get_data(G_OBJECT(ggz_gtk.win_main), "chat_entry");
		text = gtk_entry_get_text(GTK_ENTRY(tmp));
		length = strlen(text);

		/* Get last word of entry (up to 16 chars) */
		if (length < 16)
			max = length;
		else
			max = 16;

		/* Point to start of last 16 characters (or start) */
		startname = text + (length - max);
		for (i = 0; i < max; i++)
		{
			/* Check the ith char in from the right (skip \0) */
			x = length - (i + 1);
			if (text[x] == ' ')
			{
				startname = text + x + 1; /* skip the space */
				first = FALSE;
				break;
			}
		}
		if (strlen(startname) == 0)
			return TRUE;

		/* Check for matching name */
		/* FIXME: autocompletion will only work at the end of the
		   chatline. */
		name = chat_complete_name(startname, &match);
		if (name != NULL) {
			gchar *out;

			/* If it matches, copy the rest of the name */
			if (first && match)
				out = g_strdup_printf("%s%s: ", text, &name[strlen(startname)]);
			else
				out = g_strdup_printf("%s%s%s",
						      text,
						      &name[strlen(startname)],
						      match ? " " : "");
			tmp = g_object_get_data(G_OBJECT(ggz_gtk.win_main), "chat_entry");
			gtk_entry_set_text(GTK_ENTRY(tmp), out);

			/* Set the cursor to the end of the autocompleted
			   text. Since autocompletion will only work at the
			   end of the chatline, we can just set it to the
			   end of the line. */
			gtk_editable_set_position(GTK_EDITABLE(tmp), -1);
			g_free(out);
			ggz_free(name);
			return TRUE;
		}
		return TRUE;
	} else if (event->keyval == GDK_Up || event->keyval == GDK_Down) {
		gchar *out;
		tmp = g_object_get_data(G_OBJECT(ggz_gtk.win_main), "chat_entry");
		text = gtk_entry_get_text(GTK_ENTRY(tmp));
		last_list = g_object_get_data(G_OBJECT(tmp), "last_list");
		entry = g_object_get_data(G_OBJECT(tmp), "current_entry");
		if (!entry) {
			/* The text is not on the list!
			 * We will save it as "current" text */
			g_object_set_data(G_OBJECT(tmp), "current_text", ggz_strdup(text));
			if (event->keyval == GDK_Up)
				entry = ggz_list_tail(last_list);
			else
				entry = ggz_list_head(last_list);
		} else {
			if (event->keyval == GDK_Up)
				entry = ggz_list_prev(entry);
			else
				entry = ggz_list_next(entry);
		}
		
		out = ggz_list_get_data(entry);
		/* Set the new current entry */
		g_object_set_data(G_OBJECT(tmp), "current_entry", entry);
		if (out)
			gtk_entry_set_text(GTK_ENTRY(tmp), out);
		else {
			/* There isn't a entry in the cache for it
			 * Let's use the current entry then */
			out = g_object_get_data(G_OBJECT(tmp), "current_text");
			if (out) {
				gtk_entry_set_text(GTK_ENTRY(tmp), out);
				g_object_set_data(G_OBJECT(tmp), "current_text", NULL);
				ggz_free(out);
			} else
				gtk_entry_set_text(GTK_ENTRY(tmp), "");
		}
		return TRUE;
	}
	return TRUE;
}                                        

static void
client_send_button_clicked		(GtkButton	*button,
					 gpointer	 data)
{
	chat_line_entered();
}


static void
client_launch_button_clicked		(GtkButton	*button,
					 gpointer	 data)
{
	launch_create_or_raise();
}


static void
client_join_button_clicked		(GtkButton	*button,
					 gpointer	 data)
{
	client_start_table_join();
}

static void
client_watch_button_clicked (GtkButton *button, gpointer data)
{
	client_start_table_watch();
}


static void
client_leave_button_clicked		(GtkButton	*button,
					 gpointer	 data)
{
	/*game_quit();*/
	ggz_embed_leave_table();
}


static void
client_props_button_clicked		(GtkButton	*button,
					 gpointer	 data)
{
	props_raise();
}

#ifdef STATS
static void
client_stats_button_clicked		(GtkButton	*button,
					 gpointer	 data)
{
	msgbox(_("Player stats are not implemented yet. If\n"
		 "you would like to help head over to\n"
		 "http://www.ggzgamingzone.org/"), _("Not Implemented"),
	       MSGBOX_OKONLY, MSGBOX_NONE, MSGBOX_NORMAL);
}
#endif

static void
client_exit_button_clicked		(GtkButton	*button,
					 gpointer	 data)
{
	try_to_quit();
}


void client_start_table_join(void)
{
	GGZTable *table = get_selected_table();

	/* Make sure a table is selected */
	if (!table) {
		msgbox(_("You must highlight a table before you can join it."),
		       _("Error Joining"), MSGBOX_OKONLY, MSGBOX_INFO, 
		       MSGBOX_NORMAL);
		return;
	}

	/* Make sure we select a proper table */
	/* Make sure table has open seats */
	if (ggzcore_table_get_seat_count(table, GGZ_SEAT_OPEN)
	    + ggzcore_table_get_seat_count(table, GGZ_SEAT_RESERVED) == 0) {
		msgbox(_("That table is full."), _("Error Joining"),
		       MSGBOX_OKONLY, MSGBOX_INFO, MSGBOX_NORMAL);	
		return;
	}

	/* Initialize a game module */
	ggz_gtk.spectating = FALSE;
	if (game_initialize(0) == 0) {
		if (game_launch() < 0) {
			msgbox(_("Error launching game module."),
			       _("Game Error"),
			       MSGBOX_OKONLY, MSGBOX_INFO, MSGBOX_NORMAL);
			game_destroy();
		}
	}
}


static void client_start_table_watch(void)
{
	GGZTable *table = get_selected_table();

	/* Make sure a table is selected */
	if (!table) {
		msgbox(_("You must highlight a table before "
			 "you can watch it."), 
		       _("Error Spectating"), MSGBOX_OKONLY, MSGBOX_INFO, 
		       MSGBOX_NORMAL);
		return;
	}

#if 0 /* We don't check to see if there's an open place to stand around
	 the table.  Eventually we won't have to. */
	/* Make sure we select a proper table */
	if(tablerow <= numtables)
	{
		/* Make sure table has open seats */
		if (!client_get_table_open(tablerow)) {
		  msgbox(_("That table is full."),
			 _("Error Joining"),
			 MSGBOX_OKONLY, MSGBOX_INFO, MSGBOX_NORMAL);	
			return;
		}
	}
#endif

	/* Initialize a game module */
	ggz_gtk.spectating = TRUE;
	if (game_initialize(1) == 0) {
		if (game_launch() < 0) {
			msgbox(_("Error launching game module."),
			       _("Game Error"),
			       MSGBOX_OKONLY, MSGBOX_INFO, MSGBOX_NORMAL);
			game_destroy();
		}
	}
}


void client_join_table(void)
{
	GGZRoom *room;
        int status;
	GGZTable *table = get_selected_table();

	room = ggzcore_server_get_cur_room(ggz_gtk.server);
	status = ggzcore_room_join_table(room, ggzcore_table_get_id(table),
					 ggz_gtk.spectating);
	
	if (status < 0) {
		msgbox(_("Failed to join table.\nJoin aborted."),
		       _("Join Error"),
		       MSGBOX_OKONLY, MSGBOX_STOP, MSGBOX_NORMAL);
		game_destroy();
	}
}


static void tooltip(char *widget, char *tip)
{
	GtkWidget *ttmp = ggz_lookup_widget(ggz_gtk.win_main, widget);

	gtk_widget_set_tooltip_text(ttmp, tip);
}

static void
client_realize                    (GtkWidget       *widget,
				   gpointer         data)
{
#if !GTK_CHECK_VERSION(2, 12, 0)
	GtkTooltips *client_window_tips;
#define gtk_widget_set_tooltip_text(widget, text)			\
	gtk_tool_item_set_tooltip(GTK_TOOL_ITEM(widget),		\
				  client_window_tips,			\
				  (text), NULL)
#endif
	GtkXText *tmp, *tmp2;
	char *buf;
	char *font_str;

	colors_init();
	chat_init();

#if !GTK_CHECK_VERSION(2, 12, 0)
	/* setup Tooltips */
	client_window_tips = gtk_tooltips_new();
#endif

	tooltip("disconnect_button",
		_("Disconnect from the GGZ Gaming Zone server"));
	tooltip("launch_button",
		_("Start playing a game at a new table"));
	tooltip("join_button",
		_("Join an existing game"));
	tooltip("watch_button",
		_("Watch an existing game - "
		  "become a spectator of the table"));
	tooltip("leave_button",
		_("Leave the game you're currently playing"));
	tooltip("props_button",
		_("Show the properties dialog to change "
		  "the client settings"));
#ifdef STATS
	tooltip("stats_button",
		_("Show the game stats for the current room"));
#endif
	tooltip("exit_button",
		_("Exit the GGZ client application."));

	/* Set Properties */
	tmp = g_object_get_data(G_OBJECT(ggz_gtk.win_main), "table_vpaned");
	g_object_set(G_OBJECT(tmp), "user_data", 125, NULL);
	font_str = ggzcore_conf_read_string("CHAT", "FONT", DEFAULT_FONT);
	tmp = g_object_get_data(G_OBJECT(ggz_gtk.win_main), "xtext_custom");
	gtk_xtext_set_font(tmp, font_str);
	ggz_free(font_str);

	colors_activate(tmp);
	tmp->auto_indent = ggzcore_conf_read_int("CHAT", "AUTO_INDENT", TRUE);
	tmp->wordwrap = ggzcore_conf_read_int("CHAT", "WORD_WRAP", TRUE);
	tmp->max_auto_indent = 200;
	gtk_xtext_set_time_stamp(tmp->buffer, TRUE);

	/* URL Handiler */
	tmp->urlcheck_function = chat_checkurl;
	g_signal_connect (GTK_OBJECT (tmp), "word_click",
		G_CALLBACK (chat_word_clicked), NULL);

	gtk_xtext_refresh(tmp,0);

	/* Initialize the scroll bar */
	tmp2 = g_object_get_data(G_OBJECT(ggz_gtk.win_main), "chat_vscrollbar");
	gtk_range_set_adjustment(GTK_RANGE(tmp2), tmp->adj);

	gtk_xtext_refresh(tmp,0);

	/* Print out client information */
	buf = g_strdup_printf(_("Client Version:\00314 %s"), VERSION);
	chat_display_local(CHAT_LOCAL_NORMAL, NULL, buf); 
	g_free(buf);
	buf = g_strdup_printf(_("GTK+ Version:\00314 %d.%d.%d\n"),
		gtk_major_version, gtk_minor_version, gtk_micro_version);
	chat_display_local(CHAT_LOCAL_NORMAL, NULL, buf); 
	g_free(buf);

#ifdef DEBUG
	chat_display_local(CHAT_LOCAL_HIGH, NULL,
			   _("Compiled with debugging."));
#endif

}

static void client_tables_size_request(GtkWidget *widget, gpointer data)
{
	GtkWidget *tmp;
	GGZRoom *room;
	GGZGameType *gt;

	tmp =  ggz_lookup_widget(ggz_gtk.win_main, "table_vpaned");

	/* Check what the current game type is */
	room = ggzcore_server_get_cur_room(ggz_gtk.server);
	gt = ggzcore_room_get_gametype(room);

	if(ggzcore_gametype_get_name(gt) != NULL)
		g_object_set(G_OBJECT(tmp), "user_data",
			     GTK_PANED(tmp)->child1_size, NULL);
	if(ggzcore_gametype_get_name(gt) == NULL && GTK_PANED(tmp)->child1_size != 0 )
		gtk_paned_set_position(GTK_PANED(tmp), 0);
}


void ggz_embed_leave_table(void)
{
	ggzcore_room_leave_table(ggzcore_server_get_cur_room(ggz_gtk.server),
				 0);
}


GGZServer *ggz_embed_get_server(void)
{
	return ggz_gtk.server;
}

void ggz_embed_ensure_server(const char *profile_name,
			     const char *host, unsigned int port,
			     const char *username)
{
	if (!server_get(profile_name)) {
		Server *my_server = g_malloc(sizeof(*my_server));

		my_server->name = g_strdup(profile_name);
		my_server->host = g_strdup(host);
		my_server->port = port;
		my_server->type = GGZ_LOGIN_GUEST;
		my_server->login = g_strdup(username);
		my_server->password = NULL;

		server_list_add(my_server);
		server_profiles_save();
		ggzcore_conf_commit();
	}
}

/* Call this to load ggzcore configuration, and do other initializations. */
void ggz_gtk_initialize(gboolean reconnect,
			void (*connected)(GGZServer *server),
			void (*launched)(void),
			void (*ggz_closed)(void),
			const char *protocol_engine,
			const char *protocol_version,
			const char *default_profile)
{
	GGZOptions opt;
	char *global_conf, *user_conf;

	/*global_conf = "/etc/ggz/ggz.conf";*/
	/* We don't support this quite yet */
	global_conf = NULL;
#ifdef WIN32
	user_conf = g_strdup_printf("%s\\.ggz\\ggz-gtk.rc", getenv("APPDATA"));
#else
	user_conf = g_strdup_printf("%s/.ggz/ggz-gtk.rc", getenv("HOME"));
#endif
	ggzcore_conf_initialize(global_conf, user_conf);
	g_free(user_conf);

	opt.flags = GGZ_OPT_PARSER | GGZ_OPT_THREADED_IO;
#if 0
	/* Reconnect is disabled since the whole application locks up while
	 * trying to reconnect.  If you try to connect to a different
	 * server instead...you cannot. */
	if (reconnect) {
		opt.flags |= GGZ_OPT_RECONNECT;
	}
#endif
	if (protocol_engine && protocol_version) {
		/* If an engine+version are passed in, we use those as the
		 * embedded module. */
		opt.flags |= GGZ_OPT_EMBEDDED;
	} else {
		/* Modules are only used for non-embedded GGZ. */
		opt.flags |= GGZ_OPT_MODULES;
	}

	ggzcore_init(opt);

#ifdef ENABLE_NLS
	/* GTK2 always uses UTF-8 so we tell gettext to output its
	 * translations in that. */
	bindtextdomain("ggz-gtk", PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset("ggz-gtk", "UTF-8");
#endif

	server_profiles_load();	

	ggz_gtk.connected_cb = connected;
	ggz_gtk.launched_cb = launched;
	ggz_gtk.ggz_closed_cb = ggz_closed;
	if (ggz_gtk.embedded_protocol_engine) {
		ggz_free(ggz_gtk.embedded_protocol_engine);
	}
	if (ggz_gtk.embedded_protocol_version) {
		ggz_free(ggz_gtk.embedded_protocol_version);
	}
	if (ggz_gtk.embedded_default_profile) {
		ggz_free(ggz_gtk.embedded_default_profile);
	}
	ggz_gtk.embedded_protocol_engine = ggz_strdup(protocol_engine);
	ggz_gtk.embedded_protocol_version = ggz_strdup(protocol_version);
	ggz_gtk.embedded_default_profile = ggz_strdup(default_profile);
}

void ggz_gtk_exit(void)
{
	chat_save_lists();
	server_profiles_cleanup();
	chat_lists_cleanup();

	ggzcore_destroy();
}

static GtkWidget *create_main_dlg(GtkWidget *main_window)
{
  GtkWidget *main_vbox;
  GtkWidget *menubar;
  GtkWidget *ggz;
  GtkWidget *ggz_menu;
  GtkWidget *connect;
  GtkWidget *disconnect;
  GtkWidget *separator1;
  GtkWidget *exit_mnu;
  GtkWidget *game;
  GtkWidget *game_menu;
  GtkWidget *launch;
  GtkWidget *join;
  GtkWidget *watch;
  GtkWidget *separator2;
  GtkWidget *leave;
  GtkWidget *edit;
  GtkWidget *edit_menu;
  GtkWidget *properties;
  GtkWidget *view;
  GtkWidget *view_menu;
  GtkWidget *room_toggle;
  GtkWidget *player_toggle;
  GtkWidget *separator8;
#ifdef STATS
  GtkWidget *server_stats;
  GtkWidget *player_stats;
#endif
  GtkWidget *game_types;
  GtkWidget *separator3;
  GtkWidget *motd;
  GtkWidget *help;
  GtkWidget *help_menu;
  GtkWidget *about;
  GtkWidget *license;
  GtkWidget *separator6;
  GtkWidget *ggz_help;
  GtkWidget *game_help;
  GtkWidget *separator7;
  GtkWidget *goto_web1;
  GtkWidget *handlebox1;
  GtkWidget *toolbar;
  GtkToolItem *disconnect_button, *exit_button;
  GtkToolItem *launch_button, *join_button, *watch_button, *leave_button;
  GtkToolItem *props_button;
#ifdef STATS
  GtkTOolItem *stats_button;
#endif
  GtkWidget *Current_room_label;
  GtkWidget *client_hbox;
  GtkWidget *client_hpaned;
  GtkWidget *lists_vbox;
  GtkWidget *room_scrolledwindow;
  GtkWidget *room_list;
  GtkWidget *player_scrolledwindow;
  GtkWidget *player_tree;
  GtkWidget *table_vpaned;
  GtkWidget *scrolledwindow3;
  GtkWidget *table_list;
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
  GtkWidget *status_box;
  GtkWidget *serverbar;
  GtkWidget *statusbar;
  GtkWidget *statebar;
  GtkAccelGroup *accel_group;

  /* List for storing last messages */
  GGZList *last_list;

  accel_group = gtk_accel_group_new ();

  main_vbox = gtk_vbox_new (FALSE, 0);

  /* Set global value. */
  ggz_gtk.win_main = main_vbox;

  menubar = gtk_menu_bar_new ();
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "menubar", menubar);
  gtk_box_pack_start (GTK_BOX (main_vbox), menubar, FALSE, FALSE, 0);

  ggz = gtk_menu_item_new_with_label(_("GGZ"));
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "ggz", ggz);
  gtk_container_add (GTK_CONTAINER (menubar), ggz);

  ggz_menu = gtk_menu_new ();
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "ggz_menu", ggz_menu);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (ggz), ggz_menu);

  connect = gtk_menu_item_new_with_label(_("Connect"));
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "connect", connect);
  gtk_container_add (GTK_CONTAINER (ggz_menu), connect);
  gtk_widget_add_accelerator (connect, "activate", accel_group,
                              GDK_C, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  disconnect = gtk_menu_item_new_with_label(_("Disconnect"));
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "disconnect", disconnect);
  gtk_container_add (GTK_CONTAINER (ggz_menu), disconnect);
  gtk_widget_set_sensitive (disconnect, FALSE);
  gtk_widget_add_accelerator (disconnect, "activate", accel_group,
                              GDK_D, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  separator1 = gtk_menu_item_new ();
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "separator1", separator1);
  gtk_container_add (GTK_CONTAINER (ggz_menu), separator1);
  gtk_widget_set_sensitive (separator1, FALSE);

  exit_mnu = gtk_menu_item_new_with_label(_("Quit"));
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "exit", exit_mnu);
  gtk_container_add (GTK_CONTAINER (ggz_menu), exit_mnu);
  gtk_widget_add_accelerator (exit_mnu, "activate", accel_group,
                              GDK_X, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  game = gtk_menu_item_new_with_label(_("Game"));
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "game", game);
  gtk_container_add (GTK_CONTAINER (menubar), game);
  gtk_widget_set_sensitive (game, FALSE);

  game_menu = gtk_menu_new ();
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "game_menu", game_menu);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (game), game_menu);

  launch = gtk_menu_item_new_with_label(_("Launch"));
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "launch", launch);
  gtk_container_add (GTK_CONTAINER (game_menu), launch);
  gtk_widget_add_accelerator (launch, "activate", accel_group,
                              GDK_L, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  join = gtk_menu_item_new_with_label(_("Join"));
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "join", join);
  gtk_container_add (GTK_CONTAINER (game_menu), join);
  gtk_widget_add_accelerator (join, "activate", accel_group,
                              GDK_J, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  watch = gtk_menu_item_new_with_label(_("Watch"));
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "watch", watch);
  gtk_container_add (GTK_CONTAINER (game_menu), watch);
  gtk_widget_add_accelerator (watch, "activate", accel_group,
                              GDK_W, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  separator2 = gtk_menu_item_new ();
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "separator2", separator2);
  gtk_container_add (GTK_CONTAINER (game_menu), separator2);
  gtk_widget_set_sensitive (separator2, FALSE);

  leave = gtk_menu_item_new_with_label(_("Leave"));
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "leave", leave);
  gtk_container_add (GTK_CONTAINER (game_menu), leave);
  gtk_widget_add_accelerator (leave, "activate", accel_group,
                              GDK_V, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  edit = gtk_menu_item_new_with_label(_("Edit"));
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "edit", edit);
  gtk_container_add (GTK_CONTAINER (menubar), edit);

  edit_menu = gtk_menu_new ();
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "edit_menu", edit_menu);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (edit), edit_menu);

  properties = gtk_menu_item_new_with_label(_("Properties"));
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "properties", properties);
  gtk_container_add (GTK_CONTAINER (edit_menu), properties);
  gtk_widget_add_accelerator (properties, "activate", accel_group,
                              GDK_P, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  view = gtk_menu_item_new_with_label(_("View"));
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "view", view);
  gtk_container_add (GTK_CONTAINER (menubar), view);
  gtk_widget_set_sensitive (view, FALSE);

  view_menu = gtk_menu_new ();
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "view_menu", view_menu);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (view), view_menu);

  room_toggle = gtk_check_menu_item_new_with_label(_("Room List"));
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "room_toggle", room_toggle);
  gtk_container_add (GTK_CONTAINER (view_menu), room_toggle);
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (room_toggle),
				  TRUE);

  player_toggle = gtk_check_menu_item_new_with_label(_("Player List"));
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "player_toggle",
		    player_toggle);
  gtk_container_add (GTK_CONTAINER (view_menu), player_toggle);
  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM (player_toggle), TRUE);

  separator8 = gtk_menu_item_new ();
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "separator8", separator8);
  gtk_container_add (GTK_CONTAINER (view_menu), separator8);
  gtk_widget_set_sensitive (separator8, FALSE);

#ifdef STATS
  server_stats = gtk_menu_item_new_with_label(_("Server Stats"));
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "server_stats", server_stats);
  gtk_container_add (GTK_CONTAINER (view_menu), server_stats);

  player_stats = gtk_menu_item_new_with_label(_("Player Stats"));
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "player_stats", player_stats);
  gtk_container_add (GTK_CONTAINER (view_menu), player_stats);
  gtk_widget_add_accelerator (player_stats, "activate", accel_group,
                              GDK_S, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);
#endif

  game_types = gtk_menu_item_new_with_label(_("Game Types"));
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "game_types", game_types);
  gtk_container_add (GTK_CONTAINER (view_menu), game_types);

  separator3 = gtk_menu_item_new ();
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "separator3", separator3);
  gtk_container_add (GTK_CONTAINER (view_menu), separator3);
  gtk_widget_set_sensitive (separator3, FALSE);

  motd = gtk_menu_item_new_with_label(_("MOTD"));
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "motd", motd);
  gtk_container_add (GTK_CONTAINER (view_menu), motd);
  gtk_widget_add_accelerator (motd, "activate", accel_group,
                              GDK_M, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  help = gtk_menu_item_new_with_label(_("Help"));
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "help", help);
  gtk_container_add (GTK_CONTAINER (menubar), help);

  help_menu = gtk_menu_new ();
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "help_menu", help_menu);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (help), help_menu);

  about = gtk_menu_item_new_with_label(_("About"));
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "about", about);
  gtk_container_add (GTK_CONTAINER (help_menu), about);
  gtk_widget_add_accelerator (about, "activate", accel_group,
                              GDK_A, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  license = gtk_menu_item_new_with_label(_("Copyright"));
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "license", license);
  gtk_container_add (GTK_CONTAINER (help_menu), license);

  separator6 = gtk_menu_item_new ();
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "separator6", separator6);
  gtk_container_add (GTK_CONTAINER (help_menu), separator6);
  gtk_widget_set_sensitive (separator6, FALSE);

  ggz_help = gtk_menu_item_new_with_label(_("GGZ Help"));
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "ggz_help", ggz_help);
  gtk_container_add (GTK_CONTAINER (help_menu), ggz_help);
  gtk_widget_add_accelerator (ggz_help, "activate", accel_group,
                              GDK_F1, 0,
                              GTK_ACCEL_VISIBLE);

  game_help = gtk_menu_item_new_with_label(_("Game Help"));
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "game_help", game_help);
  gtk_container_add (GTK_CONTAINER (help_menu), game_help);

  separator7 = gtk_menu_item_new ();
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "separator7", separator7);
  gtk_container_add (GTK_CONTAINER (help_menu), separator7);
  gtk_widget_set_sensitive (separator7, FALSE);

  goto_web1 = gtk_menu_item_new_with_label(_("Go to Web"));
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "goto_web1", goto_web1);
  gtk_container_add (GTK_CONTAINER (help_menu), goto_web1);

  handlebox1 = gtk_handle_box_new ();
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "handlebox1", handlebox1);
  gtk_box_pack_start (GTK_BOX (main_vbox), handlebox1, FALSE, TRUE, 0);

  toolbar = gtk_toolbar_new();
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "toolbar", toolbar);
  gtk_container_add (GTK_CONTAINER (handlebox1), toolbar);

  launch_button = gtk_tool_button_new(NULL, _("Launch"));
  gtk_tool_button_set_stock_id(GTK_TOOL_BUTTON(launch_button),
			       GTK_STOCK_NEW);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar),
		     GTK_TOOL_ITEM(launch_button), -1);
  g_object_set_data(G_OBJECT(ggz_gtk.win_main), "launch_button", launch_button);
  gtk_widget_set_sensitive(GTK_WIDGET(launch_button), FALSE);

  join_button = gtk_tool_button_new(NULL, _("Join"));
  gtk_tool_button_set_stock_id(GTK_TOOL_BUTTON(join_button),
			       GTK_STOCK_OPEN);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar),
		     GTK_TOOL_ITEM(join_button), -1);
  g_object_set_data(G_OBJECT(ggz_gtk.win_main), "join_button", join_button);
  gtk_widget_set_sensitive(GTK_WIDGET(join_button), FALSE);

  watch_button = gtk_tool_button_new(NULL, _("Watch"));
  gtk_tool_button_set_stock_id(GTK_TOOL_BUTTON(watch_button),
			       GTK_STOCK_ZOOM_FIT);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar),
		     GTK_TOOL_ITEM(watch_button), -1);
  g_object_set_data(G_OBJECT(ggz_gtk.win_main), "watch_button", watch_button);
  gtk_widget_set_sensitive(GTK_WIDGET(watch_button), FALSE);

  leave_button = gtk_tool_button_new(NULL, _("Leave"));
  gtk_tool_button_set_stock_id(GTK_TOOL_BUTTON(leave_button),
			       GTK_STOCK_CLOSE);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar),
		     GTK_TOOL_ITEM(leave_button), -1);
  g_object_set_data(G_OBJECT(ggz_gtk.win_main), "leave_button", leave_button);
  gtk_widget_set_sensitive(GTK_WIDGET(leave_button), FALSE);

  props_button = gtk_tool_button_new_from_stock(GTK_STOCK_PREFERENCES);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar),
		     GTK_TOOL_ITEM(props_button), -1);
  g_object_set_data(G_OBJECT(ggz_gtk.win_main), "props_button", props_button);

#ifdef STATS
  stats_button = gtk_tool_button_new(NULL, _("Stats"));
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar),
		     GTK_TOOL_ITEM(stats_button), -1);
  g_object_set_data(G_OBJECT(ggz_gtk.win_main), "stats_button", stats_button);
  gtk_widget_set_sensitive(GTK_WIDGET(stats_button), FALSE);
#endif

  /* We should use gtk_tool_button_new_from_stock but for some reason
   * the connect and disconnect stock items don't have text included. */
  disconnect_button = gtk_tool_button_new(NULL, _("Disconnect"));
#ifdef GTK_STOCK_DISCONNECT
  gtk_tool_button_set_stock_id(GTK_TOOL_BUTTON(disconnect_button),
			       GTK_STOCK_DISCONNECT);
#endif
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar),
		     GTK_TOOL_ITEM(disconnect_button), -1);
  g_object_set_data(G_OBJECT(ggz_gtk.win_main), "disconnect_button",
		    disconnect_button);
  gtk_widget_set_sensitive(GTK_WIDGET(disconnect_button), FALSE);

  exit_button = gtk_tool_button_new_from_stock(GTK_STOCK_QUIT);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar),
		     GTK_TOOL_ITEM(exit_button), -1);
  g_object_set_data(G_OBJECT(ggz_gtk.win_main), "exit_button", exit_button);

  Current_room_label = gtk_label_new (_("Current Room:"));
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "Current_room_label",
		    Current_room_label);
  gtk_box_pack_start (GTK_BOX (main_vbox), Current_room_label, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (Current_room_label), 7.45058e-09, 0.5);
  gtk_misc_set_padding (GTK_MISC (Current_room_label), 8, 5);

  client_hbox = gtk_hbox_new (FALSE, 0);
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "client_hbox", client_hbox);
  gtk_box_pack_start (GTK_BOX (main_vbox), client_hbox, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (client_hbox), 3);

  client_hpaned = gtk_hpaned_new ();
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "client_hpaned", client_hpaned);
  gtk_box_pack_start (GTK_BOX (client_hbox), client_hpaned, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (client_hpaned), 3);
  gtk_paned_set_position (GTK_PANED (client_hpaned), 220);

  lists_vbox = gtk_vbox_new (FALSE, 0);
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "lists_vbox", lists_vbox);
  gtk_paned_pack1 (GTK_PANED (client_hpaned), lists_vbox, FALSE, TRUE);

  room_scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "room_scrolledwindow",
		    room_scrolledwindow);
  gtk_box_pack_start (GTK_BOX (lists_vbox), room_scrolledwindow, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (room_scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  room_list = create_room_list(main_window);
  gtk_container_add (GTK_CONTAINER (room_scrolledwindow), room_list);

  player_scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "player_scrolledwindow",
		    player_scrolledwindow);
  gtk_box_pack_start (GTK_BOX (lists_vbox), player_scrolledwindow, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (player_scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  player_tree = create_player_list(main_window);
  gtk_container_add(GTK_CONTAINER(player_scrolledwindow), player_tree);

  table_vpaned = gtk_vpaned_new ();
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "table_vpaned", table_vpaned);
  gtk_paned_pack2 (GTK_PANED (client_hpaned), table_vpaned, TRUE, TRUE);
  gtk_paned_set_position (GTK_PANED (table_vpaned), 125);

  scrolledwindow3 = gtk_scrolled_window_new (NULL, NULL);
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "scrolledwindow3", scrolledwindow3);
  gtk_paned_pack1 (GTK_PANED (table_vpaned), scrolledwindow3, FALSE, TRUE);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow3), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  table_list = create_table_list(main_window);
  gtk_container_add(GTK_CONTAINER(scrolledwindow3), table_list);

  chat_vbox = gtk_vbox_new (FALSE, 0);
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "chat_vbox", chat_vbox);
  gtk_paned_pack2 (GTK_PANED (table_vpaned), chat_vbox, TRUE, TRUE);

  chatdisplay_hbox = gtk_hbox_new (FALSE, 0);
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "chatdisplay_hbox",
		    chatdisplay_hbox);
  gtk_box_pack_start (GTK_BOX (chat_vbox), chatdisplay_hbox, TRUE, TRUE, 0);

  chat_frame = gtk_frame_new (NULL);
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "chat_frame", chat_frame);
  gtk_box_pack_start (GTK_BOX (chatdisplay_hbox), chat_frame, TRUE, TRUE, 0);
  gtk_frame_set_shadow_type (GTK_FRAME (chat_frame), GTK_SHADOW_IN);

  xtext_custom = main_xtext_chat_create ("xtext_custom", NULL, NULL, 0, 0);
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "xtext_custom", xtext_custom);
  gtk_container_add (GTK_CONTAINER (chat_frame), xtext_custom);
  GTK_WIDGET_UNSET_FLAGS (xtext_custom, GTK_CAN_FOCUS);
  GTK_WIDGET_UNSET_FLAGS (xtext_custom, GTK_CAN_DEFAULT);

  chat_vscrollbar = gtk_vscrollbar_new (GTK_ADJUSTMENT (gtk_adjustment_new (0, 0, 0, 0, 0, 0)));
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "chat_vscrollbar", chat_vscrollbar);
  gtk_box_pack_start (GTK_BOX (chatdisplay_hbox), chat_vscrollbar, FALSE, TRUE, 0);

  newchat_hbox = gtk_hbox_new (FALSE, 0);
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "newchat_hbox", newchat_hbox);
  gtk_box_pack_start (GTK_BOX (chat_vbox), newchat_hbox, FALSE, FALSE, 0);

  chat_label = gtk_label_new (_("Message:"));
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "chat_label", chat_label);
  gtk_box_pack_start (GTK_BOX (newchat_hbox), chat_label, FALSE, FALSE, 0);
  gtk_misc_set_padding (GTK_MISC (chat_label), 3, 0);

  chat_entry = gtk_entry_new ();
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "chat_entry", chat_entry);
  gtk_box_pack_start (GTK_BOX (newchat_hbox), chat_entry, TRUE, TRUE, 0);
  gtk_widget_set_sensitive (chat_entry, FALSE);

  /* List for last entries */
  last_list = ggz_list_create(NULL, ggz_list_create_str, ggz_list_destroy_str, GGZ_LIST_ALLOW_DUPS);
  g_object_set_data(G_OBJECT(chat_entry), "last_list", last_list);

  chat_hbuttonbox = gtk_hbutton_box_new ();
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "chat_hbuttonbox", chat_hbuttonbox);
  gtk_box_pack_start (GTK_BOX (newchat_hbox), chat_hbuttonbox, FALSE, FALSE, 0);

  send_button = stockbutton_new(GTK_STOCK_EXECUTE, _("Send"));
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "send_button", send_button);
  gtk_container_add (GTK_CONTAINER (chat_hbuttonbox), send_button);
  gtk_widget_set_sensitive (send_button, FALSE);
  GTK_WIDGET_UNSET_FLAGS (send_button, GTK_CAN_FOCUS);
  GTK_WIDGET_SET_FLAGS (send_button, GTK_CAN_DEFAULT);

  status_box = gtk_hbox_new (FALSE, 5);
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "status_box", status_box);
  gtk_box_pack_start (GTK_BOX (main_vbox), status_box, FALSE, FALSE, 0);

  serverbar = gtk_statusbar_new ();
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "serverbar", serverbar);
  gtk_statusbar_set_has_resize_grip(GTK_STATUSBAR (serverbar), FALSE);
  gtk_box_pack_start(GTK_BOX (status_box), serverbar, TRUE, TRUE, 0);

  statusbar = gtk_statusbar_new ();
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "statusbar", statusbar);
  gtk_statusbar_set_has_resize_grip(GTK_STATUSBAR (statusbar), FALSE);
  gtk_widget_set_size_request(GTK_WIDGET (statusbar), 150, -1);
  gtk_box_pack_start (GTK_BOX (status_box), statusbar, FALSE, TRUE, 0);

  statebar = gtk_statusbar_new ();
  g_object_set_data(G_OBJECT (ggz_gtk.win_main), "statebar", statebar);
  gtk_widget_set_size_request(GTK_WIDGET (statebar), 150, -1);
  gtk_box_pack_start (GTK_BOX (status_box), statebar, FALSE, TRUE, 0);

  g_signal_connect (GTK_OBJECT (connect), "activate",
                      G_CALLBACK (client_connect_activate),
                      NULL);
  g_signal_connect (GTK_OBJECT (disconnect), "activate",
                      G_CALLBACK (client_disconnect_activate),
                      NULL);
  g_signal_connect (GTK_OBJECT(exit_mnu), "activate",
                      G_CALLBACK (client_exit_activate),
                      NULL);
  g_signal_connect (GTK_OBJECT (launch), "activate",
                      G_CALLBACK (client_launch_activate),
                      NULL);
  g_signal_connect (GTK_OBJECT (join), "activate",
                      G_CALLBACK (client_joinm_activate),
                      NULL);
  g_signal_connect (GTK_OBJECT (watch), "activate",
                      G_CALLBACK (client_watchm_activate),
                      NULL);
  g_signal_connect (GTK_OBJECT (leave), "activate",
                      G_CALLBACK (client_leave_activate),
                      NULL);
  g_signal_connect (GTK_OBJECT (properties), "activate",
                      G_CALLBACK (client_properties_activate),
                      NULL);
  g_signal_connect (GTK_OBJECT (room_toggle), "toggled",
		    G_CALLBACK(widget_toggle_activate),
		    room_scrolledwindow);
  g_signal_connect (GTK_OBJECT(player_toggle), "toggled",
		    G_CALLBACK(widget_toggle_activate),
		    player_scrolledwindow);
#ifdef STATS
  g_signal_connect (GTK_OBJECT (server_stats), "activate",
                      G_CALLBACK (client_server_stats_activate),
                      NULL);
  g_signal_connect (GTK_OBJECT (player_stats), "activate",
                      G_CALLBACK (client_player_stats_activate),
                      NULL);
#endif
  g_signal_connect (GTK_OBJECT (game_types), "activate",
                      G_CALLBACK (client_game_types_activate),
                      NULL);
  g_signal_connect (GTK_OBJECT (motd), "activate",
                      G_CALLBACK (client_motd_activate),
                      NULL);
  g_signal_connect (GTK_OBJECT (about), "activate",
                      G_CALLBACK (client_about_activate),
                      NULL);
  g_signal_connect (GTK_OBJECT (license), "activate",
                      G_CALLBACK (client_license_activate),
                      NULL);
  g_signal_connect (GTK_OBJECT (ggz_help), "activate",
                      G_CALLBACK (client_ggz_help_activate),
                      NULL);
  g_signal_connect (GTK_OBJECT (game_help), "activate",
                      G_CALLBACK (client_game_help_activate),
                      NULL);
  g_signal_connect (GTK_OBJECT (goto_web1), "activate",
                      G_CALLBACK (client_goto_web1_activate),
                      NULL);
  g_signal_connect (GTK_OBJECT (disconnect_button), "clicked",
                      G_CALLBACK (client_disconnect_button_clicked),
                      NULL);
  g_signal_connect (GTK_OBJECT (launch_button), "clicked",
                      G_CALLBACK (client_launch_button_clicked),
                      NULL);
  g_signal_connect (GTK_OBJECT (join_button), "clicked",
                      G_CALLBACK (client_join_button_clicked),
                      NULL);
  g_signal_connect (GTK_OBJECT (watch_button), "clicked",
		      G_CALLBACK (client_watch_button_clicked),
		      NULL);
  g_signal_connect (GTK_OBJECT (leave_button), "clicked",
                      G_CALLBACK (client_leave_button_clicked),
                      NULL);
  g_signal_connect (GTK_OBJECT (props_button), "clicked",
                      G_CALLBACK (client_props_button_clicked),
                      NULL);
#ifdef STATS
  g_signal_connect (GTK_OBJECT (stats_button), "clicked",
                      G_CALLBACK (client_stats_button_clicked),
                      NULL);
#endif
  g_signal_connect (GTK_OBJECT (exit_button), "clicked",
                      G_CALLBACK (client_exit_button_clicked),
                      NULL);
  g_signal_connect (GTK_OBJECT (scrolledwindow3), "size_request",
                      G_CALLBACK (client_tables_size_request),
                      NULL);
  g_signal_connect (GTK_OBJECT (chat_entry), "activate",
                      G_CALLBACK (client_chat_entry_activate),
                      chat_entry);
  g_signal_connect_after (GTK_OBJECT (chat_entry), "key_press_event",
                            G_CALLBACK (client_chat_entry_key_press_event),
                            NULL);
  g_signal_connect (GTK_OBJECT (send_button), "clicked",
                      G_CALLBACK (client_send_button_clicked),
                      NULL);

  g_signal_connect(GTK_OBJECT(main_vbox), "realize",
		   G_CALLBACK (client_realize),
		   NULL);

  gtk_widget_grab_focus (chat_entry);
  gtk_window_add_accel_group (GTK_WINDOW(main_window), accel_group);

  return main_vbox;
}

GtkWidget *ggz_gtk_create_main_area(GtkWidget *main_win)
{
	ggz_gtk.main_window = main_win;
	ggz_gtk.notebook = gtk_notebook_new();

	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(ggz_gtk.notebook), FALSE);
	gtk_notebook_set_show_border(GTK_NOTEBOOK(ggz_gtk.notebook), FALSE);

	/* The order of these matches the enum ggz_page in client.h */
	gtk_notebook_append_page(GTK_NOTEBOOK(ggz_gtk.notebook),
				 create_dlg_first(), NULL);
	gtk_notebook_append_page(GTK_NOTEBOOK(ggz_gtk.notebook),
			create_dlg_login(ggz_gtk.embedded_default_profile),
			NULL);
	gtk_notebook_append_page(GTK_NOTEBOOK(ggz_gtk.notebook),
				 create_main_dlg(main_win), NULL);
	gtk_notebook_append_page(GTK_NOTEBOOK(ggz_gtk.notebook),
				 create_props_dlg(), NULL);

	gtk_widget_show_all(ggz_gtk.notebook);

	main_activate();

	return ggz_gtk.notebook;
}

void main_activate(void)
{
	if (first_is_raised()) {
		gtk_notebook_set_current_page(GTK_NOTEBOOK(ggz_gtk.notebook),
					      GGZ_PAGE_FIRSTLOGIN);
	} else if (props_is_raised()) {
		gtk_notebook_set_current_page(GTK_NOTEBOOK(ggz_gtk.notebook),
					      GGZ_PAGE_PROPS);
	} else if (ggz_gtk.server
		   && ggzcore_server_is_logged_in(ggz_gtk.server)) {
		gtk_notebook_set_current_page(GTK_NOTEBOOK(ggz_gtk.notebook),
					      GGZ_PAGE_MAIN);
	} else {
		gtk_notebook_set_current_page(GTK_NOTEBOOK(ggz_gtk.notebook),
					      GGZ_PAGE_LOGIN);
	}
}

GtkWidget *ggz_gtk_create_main_window(const char *option_log)
{
	GtkWidget *main_window, *main_vbox;
	const char *init_version;

	login_set_option_log(option_log);

	main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request(main_window, 620, 400);
	gtk_window_set_title(GTK_WINDOW (main_window), _("GGZ Gaming Zone"));
	gtk_window_set_resizable(GTK_WINDOW(main_window), TRUE);

	main_vbox = ggz_gtk_create_main_area(main_window);
	gtk_container_add (GTK_CONTAINER (main_window), main_vbox);

	g_signal_connect(GTK_OBJECT(main_window), "delete_event",
			 G_CALLBACK (client_exit_activate),
			 NULL);

	ggz_sensitivity_init();

	init_version = ggzcore_conf_read_string("INIT", "VERSION", VERSION);
	if (ggzcore_conf_read_int("INIT", "FIRST", 0) == 0
	    || strcmp(init_version, VERSION) != 0) {
		first_raise();
	}
	ggz_free(init_version);

	return main_window;
}
