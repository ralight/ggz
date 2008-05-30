/*
 * File: playerinfo.c
 * Author: Justin Zaun
 * Project: GGZ GTK Client
 * $Id: playerinfo.c 10013 2008-05-30 20:01:43Z jdorje $
 *
 * This dialog is used to display information about a selected player to
 * the user. 
 *
 * Copyright (C) 2002 GGZ Development Team.
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
#include <config.h>
#endif

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <ggzcore.h>

#include "chat.h"
#include "client.h"
#include "ggzutils.h"
#include "playerinfo.h"
#include "support.h"


static GtkWidget *dialog;
static char *player_name;


static void chat_activate(GtkEditable * editable, gpointer data)
{
	GtkEntry *entry;
	GtkLabel *handle;
	const char *text;
	gchar *name;
	GGZServer *server = data;
	GGZRoom *room = ggzcore_server_get_cur_room(server);

	entry = g_object_get_data(G_OBJECT(dialog), "chat");
	handle = g_object_get_data(G_OBJECT(dialog), "handle");

	text = gtk_entry_get_text(entry);
	if (strcmp(text, "") == 0)
		return;

	name = player_name;
	if (!name)
		return;

	ggzcore_room_chat(room, GGZ_CHAT_PERSONAL, name, text);
	chat_display_local(CHAT_SEND_PERSONAL, name, text);

	/* Clear the entry box */
	gtk_entry_set_text(entry, "");
}


static GtkWidget *create_dlg_info(GGZServer *server)
{
	GtkWidget *dlg_info;
	GtkWidget *dialog_vbox;
	GtkWidget *display_table;
	GtkWidget *game_pixmap;
	GtkWidget *handle_label;
	GtkWidget *handle;
	GtkWidget *table_label;
	GtkWidget *table;
	GtkWidget *type_label;
	GtkWidget *type;
	GtkWidget *perms_label;
	GtkWidget *perms;
	GtkWidget *record_label;
	GtkWidget *record;
	GtkWidget *rating_label;
	GtkWidget *rating;
	GtkWidget *ranking_label;
	GtkWidget *ranking;
	GtkWidget *highscore_label;
	GtkWidget *highscore;
	GtkWidget *chat_label;
	GtkWidget *chat;

	dlg_info = gtk_dialog_new_with_buttons(_("Player Information"),
					       GTK_WINDOW(main_window), 0,
					       GTK_STOCK_CLOSE,
					       GTK_RESPONSE_CLOSE, NULL);
	g_object_set_data(G_OBJECT(dlg_info), "dlg_info", dlg_info);

	dialog_vbox = GTK_DIALOG(dlg_info)->vbox;
	g_object_set_data(G_OBJECT(dlg_info), "dialog_vbox", dialog_vbox);
	gtk_widget_show(dialog_vbox);

	display_table = gtk_table_new(9, 3, FALSE);
	g_object_set_data(G_OBJECT(dlg_info), "display_table", display_table);
	gtk_widget_show(display_table);
	gtk_box_pack_start(GTK_BOX(dialog_vbox), display_table,
			   FALSE, TRUE, 0);

	game_pixmap = gtk_image_new_from_stock(GTK_STOCK_DIALOG_QUESTION,
					       GTK_ICON_SIZE_DIALOG);
	g_object_set_data(G_OBJECT(dlg_info), "game_pixmap", game_pixmap);
	gtk_widget_show(game_pixmap);
	gtk_table_attach(GTK_TABLE(display_table), game_pixmap,
			 0, 1, 0, 8, 0, 0, 0, 0);

	/* Add 'handle' label */
	handle_label = gtk_label_new(_("Player Handle:"));
	g_object_set_data(G_OBJECT(dlg_info), "handle_label", handle_label);
	gtk_widget_show(handle_label);
	gtk_table_attach(GTK_TABLE(display_table), handle_label,
			 1, 2, 0, 1, 0, 0, 0, 0);

	handle = gtk_label_new("");
	g_object_set_data(G_OBJECT(dlg_info), "handle", handle);
	gtk_widget_show(handle);
	gtk_table_attach(GTK_TABLE(display_table), handle,
			 2, 3, 0, 1, GTK_EXPAND, 0, 0, 0);

	/* Add "table" label */
	table_label = gtk_label_new(_("Table:"));
	g_object_set_data(G_OBJECT(dlg_info), "table_label", table_label);
	gtk_widget_show(table_label);
	gtk_table_attach(GTK_TABLE(display_table), table_label,
			 1, 2, 1, 2, 0, 0, 0, 0);

	table = gtk_label_new("-");
	g_object_set_data(G_OBJECT(dlg_info), "table", table);
	gtk_widget_show(table);
	gtk_table_attach(GTK_TABLE(display_table), table,
			 2, 3, 1, 2, GTK_EXPAND, 0, 0, 0);

	/* Add "type" label */
	type_label = gtk_label_new(_("Account:"));
	g_object_set_data(G_OBJECT(dlg_info), "type_label", type_label);
	gtk_widget_show(type_label);
	gtk_table_attach(GTK_TABLE(display_table), type_label,
			 1, 2, 2, 3, 0, 0, 0, 0);

	type = gtk_label_new("");
	g_object_set_data(G_OBJECT(dlg_info), "type", type);
	gtk_widget_show(type);
	gtk_table_attach(GTK_TABLE(display_table), type,
			 2, 3, 2, 3, GTK_EXPAND, 0, 0, 0);
	gtk_label_set_line_wrap(GTK_LABEL(type), TRUE);

	/* Add permissions label */
	perms_label = gtk_label_new(_("Permissions:"));
	gtk_widget_show(perms_label);
	gtk_table_attach(GTK_TABLE(display_table), perms_label,
			 1, 2, 3, 4, 0, 0, 0, 0);
	perms = gtk_label_new("");
	gtk_widget_show(perms);
	gtk_table_attach(GTK_TABLE(display_table), perms,
			 2, 3, 3, 4, GTK_EXPAND, 0, 0, 0);
	g_object_set_data(G_OBJECT(dlg_info), "perms", perms);
	gtk_label_set_line_wrap(GTK_LABEL(perms), TRUE);

	/* Add "Record" label */
	record_label = gtk_label_new(_("Record:"));
	g_object_set_data(G_OBJECT(dlg_info), "record_label", record_label);
	/* gtk_widget_show is called later. */
	gtk_table_attach(GTK_TABLE(display_table), record_label,
			 1, 2, 4, 5, 0, 0, 0, 0);

	record = gtk_label_new("");
	g_object_set_data(G_OBJECT(dlg_info), "record", record);
	/* gtk_widget_show is called later. */
	gtk_table_attach(GTK_TABLE(display_table), record,
			 2, 3, 4, 5, GTK_EXPAND, 0, 0, 0);

	/* Add "Rating" label */
	rating_label = gtk_label_new(_("Rating:"));
	g_object_set_data(G_OBJECT(dlg_info), "rating_label", rating_label);
	/* gtk_widget_show is called later. */
	gtk_table_attach(GTK_TABLE(display_table), rating_label,
			 1, 2, 5, 6, 0, 0, 0, 0);

	rating = gtk_label_new("");
	g_object_set_data(G_OBJECT(dlg_info), "rating", rating);
	/* gtk_widget_show is called later. */
	gtk_table_attach(GTK_TABLE(display_table), rating,
			 2, 3, 5, 6, GTK_EXPAND, 0, 0, 0);

	/* Add "Ranking" label */
	ranking_label = gtk_label_new(_("Rank:"));
	g_object_set_data(G_OBJECT(dlg_info), "ranking_label", ranking_label);
	/* gtk_widget_show is called later. */
	gtk_table_attach(GTK_TABLE(display_table), ranking_label,
			 1, 2, 6, 7, 0, 0, 0, 0);

	ranking = gtk_label_new("");
	g_object_set_data(G_OBJECT(dlg_info), "ranking", ranking);
	/* gtk_widget_show is called later. */
	gtk_table_attach(GTK_TABLE(display_table), ranking,
			 2, 3, 6, 7, GTK_EXPAND, 0, 0, 0);

	/* Add "Highscore" label */
	highscore_label = gtk_label_new(_("Score:"));
	g_object_set_data(G_OBJECT(dlg_info), "highscore_label",
			  highscore_label);
	/* gtk_widget_show is called later. */
	gtk_table_attach(GTK_TABLE(display_table), highscore_label,
			 1, 2, 7, 8, 0, 0, 0, 0);

	highscore = gtk_label_new("");
	g_object_set_data(G_OBJECT(dlg_info), "highscore", highscore);
	/* gtk_widget_show is called later. */
	gtk_table_attach(GTK_TABLE(display_table), highscore,
			 2, 3, 7, 8, GTK_EXPAND, 0, 0, 0);

	/* Add "Private chat" label */
	chat_label = gtk_label_new(_("Message:"));
	g_object_set_data(G_OBJECT(dlg_info), "chat_label", chat_label);
	gtk_widget_show(chat_label);
	gtk_table_attach(GTK_TABLE(display_table), chat_label,
			 1, 2, 8, 9, 0, 0, 0, 0);

	chat = gtk_entry_new();
	g_object_set_data(G_OBJECT(dlg_info), "chat", chat);
	gtk_widget_show(chat);
	gtk_table_attach(GTK_TABLE(display_table), chat,
			 2, 3, 8, 9, GTK_EXPAND, 0, 0, 0);
	g_signal_connect(chat, "activate",
			 GTK_SIGNAL_FUNC(chat_activate), server);

	g_signal_connect(dlg_info, "destroy",
			 GTK_SIGNAL_FUNC(gtk_widget_destroyed), &dialog);
	g_signal_connect(dlg_info, "response",
			 GTK_SIGNAL_FUNC(gtk_widget_destroy), NULL);

	return dlg_info;
}


/* player_info_create_or_raise() - Displays the dialog or updates current
 *                          dialog with new room's information
 *
 * Recieves:
 * GGZPlayer*	: Player to display info about
 *
 * Returns:
 */
void player_info_create_or_raise(GGZPlayer *player)
{
	GtkWidget *tmp;
	GGZRoom *room = ggzcore_player_get_room(player);
	GGZTable *table = ggzcore_player_get_table(player);
	GGZServer *server = ggzcore_room_get_server(room);
	char text[4096];
	char *ptype = _("Unknown");
	int wins, losses, ties, forfeits;
	int rating, ranking;
	int highscore;
	GGZPerm p;

	if (!dialog) {
		dialog = create_dlg_info(server);
		gtk_widget_show(dialog);
	} else {
		gdk_window_show(dialog->window);
		gdk_window_raise(dialog->window);
	}

	tmp = g_object_get_data(G_OBJECT(dialog), "handle");
	gtk_label_set_text(GTK_LABEL(tmp),
			   ggzcore_player_get_name(player));
	if (player_name)
		g_free(player_name);
	player_name = g_strdup(ggzcore_player_get_name(player));

	tmp = g_object_get_data(G_OBJECT(dialog), "table");
	if (table)
		snprintf(text, sizeof(text), "%d",
			 ggzcore_table_get_id(table));
	else
		snprintf(text, sizeof(text), "-");
	gtk_label_set_text(GTK_LABEL(tmp), text);


	tmp = g_object_get_data(G_OBJECT(dialog), "type");
	switch (ggzcore_player_get_type(player)) {
	case GGZ_PLAYER_UNKNOWN:
		ptype = _("Unknown");
		break;
	case GGZ_PLAYER_NORMAL:
		ptype = _("Registered");
		break;
	case GGZ_PLAYER_GUEST:
		ptype = _("Guest");
		break;
	case GGZ_PLAYER_HOST:
		ptype = _("Host");
		break;
	case GGZ_PLAYER_ADMIN:
		ptype = _("Administrator");
		break;
	case GGZ_PLAYER_BOT:
		ptype = _("Bot");
		break;
	}
	gtk_label_set_text(GTK_LABEL(tmp), ptype);

	/* UTF-8 FIXME.
	 * This should be done as a checkbox rather than a text list.
	 * The interface would then double as an admin editor. */

	tmp = g_object_get_data(G_OBJECT(dialog), "perms");
	text[0] = '\0';
	for (p = 0; p < GGZ_PERM_COUNT; p++) {
		if (ggzcore_player_has_perm(player, p)) {
			snprintf(text + strlen(text),
				 sizeof(text) - strlen(text),
				 "%s\n", perm_name(p));
		}
	}
	if (text[0] == '\0') {
		snprintf(text, sizeof(text), _("none/unknown"));
	} else {
		text[strlen(text) - 1] = '\0';
	}
	gtk_label_set_text(GTK_LABEL(tmp), text);

	/* The only thing we don't show is lag. */

	if (ggzcore_player_get_record(player, &wins,
				      &losses, &ties, &forfeits)) {
		tmp = g_object_get_data(G_OBJECT(dialog), "record_label");
		gtk_widget_show(tmp);
		tmp = g_object_get_data(G_OBJECT(dialog), "record");
		gtk_widget_show(tmp);

		snprintf(text, sizeof(text), "%d-%d", wins, losses);
		if (ties > 0) {
			snprintf(text + strlen(text),
				 sizeof(text) - strlen(text), "-%d", ties);
		}
		if (forfeits > 0) {
			snprintf(text + strlen(text),
				 sizeof(text) - strlen(text),
				 " (%d)", forfeits);
		}

		gtk_label_set_text(GTK_LABEL(tmp), text);
	} else {
		tmp = g_object_get_data(G_OBJECT(dialog), "record_label");
		gtk_widget_hide(tmp);
		tmp = g_object_get_data(G_OBJECT(dialog), "record");
		gtk_widget_hide(tmp);
	}

	if (ggzcore_player_get_rating(player, &rating)) {
		tmp = g_object_get_data(G_OBJECT(dialog), "rating_label");
		gtk_widget_show(tmp);
		tmp = g_object_get_data(G_OBJECT(dialog), "rating");
		gtk_widget_show(tmp);

		snprintf(text, sizeof(text), "%d", rating);

		gtk_label_set_text(GTK_LABEL(tmp), text);
	} else {
		tmp = g_object_get_data(G_OBJECT(dialog), "rating_label");
		gtk_widget_hide(tmp);
		tmp = g_object_get_data(G_OBJECT(dialog), "rating");
		gtk_widget_hide(tmp);
	}

	if (ggzcore_player_get_ranking(player, &ranking)) {
		tmp = g_object_get_data(G_OBJECT(dialog), "ranking_label");
		gtk_widget_show(tmp);
		tmp = g_object_get_data(G_OBJECT(dialog), "ranking");
		gtk_widget_show(tmp);

		snprintf(text, sizeof(text), _("#%d"), ranking);

		gtk_label_set_text(GTK_LABEL(tmp), text);
	} else {
		tmp = g_object_get_data(G_OBJECT(dialog), "ranking_label");
		gtk_widget_hide(tmp);
		tmp = g_object_get_data(G_OBJECT(dialog), "ranking");
		gtk_widget_hide(tmp);
	}

	if (ggzcore_player_get_highscore(player, &highscore)) {
		tmp = g_object_get_data(G_OBJECT(dialog),
					"highscore_label");
		gtk_widget_show(tmp);
		tmp = g_object_get_data(G_OBJECT(dialog), "highscore");
		gtk_widget_show(tmp);

		snprintf(text, sizeof(text), "%d", highscore);

		gtk_label_set_text(GTK_LABEL(tmp), text);
	} else {
		tmp = g_object_get_data(G_OBJECT(dialog),
					"highscore_label");
		gtk_widget_hide(tmp);
		tmp = g_object_get_data(G_OBJECT(dialog), "highscore");
		gtk_widget_hide(tmp);
	}
}
