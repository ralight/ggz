/*
 * File: playerinfo.c
 * Author: Justin Zaun
 * Project: GGZ GTK Client
 * $Id: playerinfo.c 6323 2004-11-11 04:02:23Z jdorje $
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
#include "playerinfo.h"
#include "server.h"
#include "support.h"


static GtkWidget *dialog;
static GtkWidget *create_dlg_info(void);
static char *player_name;


/* player_info_create_or_raise() - Displays the dialog or updates current
 *                          dialog with new room's information
 *
 * Recieves:
 * GGZPlayer*	: Player to display info about
 *
 * Returns:
 */

void player_info_create_or_raise(GGZPlayer * player)
{
	GtkWidget *tmp;
	GGZTable *table = ggzcore_player_get_table(player);
	char text[128];
	char *ptype = _("Unknown");
	int wins, losses, ties, forfeits;
	int rating, ranking;
	int highscore;

	if (!dialog) {
		dialog = create_dlg_info();
		gtk_widget_show(dialog);
	} else {
		gdk_window_show(dialog->window);
		gdk_window_raise(dialog->window);
	}

	tmp = g_object_get_data(G_OBJECT(dialog), "handle");
	gtk_label_set_text(GTK_LABEL(tmp), ggzcore_player_get_name(player));
	if (player_name) g_free(player_name);
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
	case GGZ_PLAYER_ADMIN:
		ptype = _("Administrator");
		break;
	case GGZ_PLAYER_BOT:
		ptype = _("Bot");
		break;
	}
	gtk_label_set_text(GTK_LABEL(tmp), ptype);

	/* The only thing we don't show is lag. */

	if (ggzcore_player_get_record(player, &wins,
				      &losses, &ties, &forfeits)) {
		tmp = g_object_get_data(G_OBJECT(dialog), "record_label");
		gtk_widget_show(tmp);
		tmp = g_object_get_data(G_OBJECT(dialog), "record_hbox");
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
		tmp = g_object_get_data(G_OBJECT(dialog), "record_hbox");
		gtk_widget_hide(tmp);
	}

	if (ggzcore_player_get_rating(player, &rating)) {
		tmp = g_object_get_data(G_OBJECT(dialog), "rating_label");
		gtk_widget_show(tmp);
		tmp = g_object_get_data(G_OBJECT(dialog), "rating_hbox");
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
		tmp = g_object_get_data(G_OBJECT(dialog), "rating_hbox");
		gtk_widget_hide(tmp);
	}

	if (ggzcore_player_get_ranking(player, &ranking)) {
		tmp = g_object_get_data(G_OBJECT(dialog),
					  "ranking_label");
		gtk_widget_show(tmp);
		tmp = g_object_get_data(G_OBJECT(dialog), "ranking_hbox");
		gtk_widget_show(tmp);
		tmp = g_object_get_data(G_OBJECT(dialog), "ranking");
		gtk_widget_show(tmp);

		snprintf(text, sizeof(text), "#%d", ranking);

		gtk_label_set_text(GTK_LABEL(tmp), text);
	} else {
		tmp = g_object_get_data(G_OBJECT(dialog),
					  "ranking_label");
		gtk_widget_hide(tmp);
		tmp = g_object_get_data(G_OBJECT(dialog), "ranking");
		gtk_widget_hide(tmp);
		tmp = g_object_get_data(G_OBJECT(dialog), "ranking_hbox");
		gtk_widget_hide(tmp);
	}

	if (ggzcore_player_get_highscore(player, &highscore)) {
		tmp = g_object_get_data(G_OBJECT(dialog),
					  "highscore_label");
		gtk_widget_show(tmp);
		tmp = g_object_get_data(G_OBJECT(dialog),
					  "highscore_hbox");
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
		tmp = g_object_get_data(G_OBJECT(dialog),
					  "highscore_hbox");
		gtk_widget_hide(tmp);
	}
}


static void chat_activate(GtkEditable *editable, gpointer data)
{
	GtkEntry *entry;
	GtkLabel *handle;
	const char *text;
	gchar *name;
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


GtkWidget *create_dlg_info(void)
{
	GtkWidget *dlg_info;
	GtkWidget *dialog_vbox;
	GtkWidget *display_hbox;
	GtkWidget *game_pixmap;
	GtkWidget *info_vbox;
	GtkWidget *handle_hbox;
	GtkWidget *handle_label;
	GtkWidget *handle;
	GtkWidget *table_hbox;
	GtkWidget *table_label;
	GtkWidget *table;
	GtkWidget *type_hbox;
	GtkWidget *type_label;
	GtkWidget *type;
	GtkWidget *record_hbox;
	GtkWidget *record_label;
	GtkWidget *record;
	GtkWidget *rating_hbox;
	GtkWidget *rating_label;
	GtkWidget *rating;
	GtkWidget *ranking_hbox;
	GtkWidget *ranking_label;
	GtkWidget *ranking;
	GtkWidget *highscore_hbox;
	GtkWidget *highscore_label;
	GtkWidget *highscore;
	GtkWidget *chat_hbox;
	GtkWidget *chat_label;
	GtkWidget *chat;
	GtkWidget *dialog_action_area1;
	GtkWidget *button_box;
	GtkWidget *ok_button;

	dlg_info = gtk_dialog_new();
	gtk_window_set_transient_for(GTK_WINDOW(dlg_info),
				     GTK_WINDOW(win_main));
	g_object_set_data(G_OBJECT(dlg_info), "dlg_info", dlg_info);
	gtk_window_set_title(GTK_WINDOW(dlg_info), _("Player Information"));

	dialog_vbox = GTK_DIALOG(dlg_info)->vbox;
	g_object_set_data(G_OBJECT(dlg_info), "dialog_vbox", dialog_vbox);
	gtk_widget_show(dialog_vbox);

	display_hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(display_hbox);
	g_object_set_data_full(G_OBJECT(dlg_info), "display_hbox",
				 display_hbox,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(display_hbox);
	gtk_box_pack_start(GTK_BOX(dialog_vbox), display_hbox, FALSE, TRUE,
			   0);
	gtk_container_set_border_width(GTK_CONTAINER(display_hbox), 5);

	game_pixmap = create_pixmap(dlg_info, NULL);
	gtk_widget_ref(game_pixmap);
	g_object_set_data_full(G_OBJECT(dlg_info), "game_pixmap",
				 game_pixmap,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(game_pixmap);
	gtk_box_pack_start(GTK_BOX(display_hbox), game_pixmap, FALSE, TRUE,
			   0);
	gtk_misc_set_alignment(GTK_MISC(game_pixmap), 0.5, 0);

	info_vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(info_vbox);
	g_object_set_data_full(G_OBJECT(dlg_info), "info_vbox", info_vbox,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(info_vbox);
	gtk_box_pack_start(GTK_BOX(display_hbox), info_vbox, TRUE, TRUE, 0);


	/* Add 'handle' label */
	handle_hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(handle_hbox);
	g_object_set_data_full(G_OBJECT(dlg_info), "handle_hbox",
				 handle_hbox,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(handle_hbox);
	gtk_box_pack_start(GTK_BOX(info_vbox), handle_hbox, TRUE, TRUE, 0);

	handle_label = gtk_label_new(_("Player Handle:"));
	gtk_widget_ref(handle_label);
	g_object_set_data_full(G_OBJECT(dlg_info), "handle_label",
				 handle_label,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(handle_label);
	gtk_box_pack_start(GTK_BOX(handle_hbox), handle_label, FALSE, FALSE,
			   0);
	gtk_misc_set_alignment(GTK_MISC(handle_label), 0.02, 0.5);

	handle = gtk_label_new("");
	gtk_widget_ref(handle);
	g_object_set_data_full(G_OBJECT(dlg_info), "handle", handle,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(handle);
	gtk_box_pack_start(GTK_BOX(handle_hbox), handle, TRUE, TRUE, 0);
	gtk_misc_set_alignment(GTK_MISC(handle), 0, 0.5);


	/* Add "table" label */
	table_hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(table_hbox);
	g_object_set_data_full(G_OBJECT(dlg_info), "table_hbox",
				 table_hbox,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(table_hbox);
	gtk_box_pack_start(GTK_BOX(info_vbox), table_hbox, TRUE, TRUE, 0);

	table_label = gtk_label_new(_("Table:"));
	gtk_widget_ref(table_label);
	g_object_set_data_full(G_OBJECT(dlg_info), "table_label",
				 table_label,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(table_label);
	gtk_box_pack_start(GTK_BOX(table_hbox), table_label, FALSE, FALSE, 0);
	gtk_misc_set_alignment(GTK_MISC(table_label), 0.02, 0.5);

	table = gtk_label_new("-");
	gtk_widget_ref(table);
	g_object_set_data_full(G_OBJECT(dlg_info), "table", table,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(table);
	gtk_box_pack_start(GTK_BOX(table_hbox), table, TRUE, TRUE, 0);
	gtk_misc_set_alignment(GTK_MISC(table), 0, 0.5);


	/* Add "type" label */
	type_hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(type_hbox);
	g_object_set_data_full(G_OBJECT(dlg_info), "type_hbox", type_hbox,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(type_hbox);
	gtk_box_pack_start(GTK_BOX(info_vbox), type_hbox, TRUE, TRUE, 0);

	type_label = gtk_label_new(_("Account:"));
	gtk_widget_ref(type_label);
	g_object_set_data_full(G_OBJECT(dlg_info), "type_label",
				 type_label,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(type_label);
	gtk_box_pack_start(GTK_BOX(type_hbox), type_label, FALSE, FALSE, 0);
	gtk_misc_set_alignment(GTK_MISC(type_label), 0.02, 0);

	type = gtk_label_new("");
	gtk_widget_ref(type);
	g_object_set_data_full(G_OBJECT(dlg_info), "type", type,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(type);
	gtk_box_pack_start(GTK_BOX(type_hbox), type, TRUE, TRUE, 0);
	gtk_label_set_line_wrap(GTK_LABEL(type), TRUE);
	gtk_misc_set_alignment(GTK_MISC(type), 0, 0.5);

	dialog_action_area1 = GTK_DIALOG(dlg_info)->action_area;
	g_object_set_data(G_OBJECT(dlg_info), "dialog_action_area1",
			  dialog_action_area1);
	gtk_widget_show(dialog_action_area1);
	gtk_container_set_border_width(GTK_CONTAINER(dialog_action_area1),
				       10);


	/* Add "Record" label */
	record_hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(record_hbox);
	g_object_set_data_full(G_OBJECT(dlg_info), "record_hbox",
				 record_hbox,
				 (GtkDestroyNotify) gtk_widget_unref);
	/* Widget shown later */
	gtk_box_pack_start(GTK_BOX(info_vbox), record_hbox, TRUE, TRUE, 0);

	record_label = gtk_label_new(_("Record:"));
	gtk_widget_ref(record_label);
	g_object_set_data_full(G_OBJECT(dlg_info), "record_label",
				 record_label,
				 (GtkDestroyNotify) gtk_widget_unref);
	/* Widget shown later */
	gtk_box_pack_start(GTK_BOX(record_hbox), record_label, FALSE, FALSE,
			   0);
	gtk_misc_set_alignment(GTK_MISC(record_label), 0.02, 0);

	record = gtk_label_new("");
	gtk_widget_ref(record);
	g_object_set_data_full(G_OBJECT(dlg_info), "record", record,
				 (GtkDestroyNotify) gtk_widget_unref);
	/* Widget shown later */
	gtk_box_pack_start(GTK_BOX(record_hbox), record, TRUE, TRUE, 0);
	gtk_label_set_line_wrap(GTK_LABEL(record), TRUE);
	gtk_misc_set_alignment(GTK_MISC(record), 0, 0.5);


	/* Add "Rating" label */
	rating_hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(rating_hbox);
	g_object_set_data_full(G_OBJECT(dlg_info), "rating_hbox",
				 rating_hbox,
				 (GtkDestroyNotify) gtk_widget_unref);
	/* Widget shown later */
	gtk_box_pack_start(GTK_BOX(info_vbox), rating_hbox, TRUE, TRUE, 0);

	rating_label = gtk_label_new(_("Rating:"));
	gtk_widget_ref(rating_label);
	g_object_set_data_full(G_OBJECT(dlg_info), "rating_label",
				 rating_label,
				 (GtkDestroyNotify) gtk_widget_unref);
	/* Widget shown later */
	gtk_box_pack_start(GTK_BOX(rating_hbox), rating_label, FALSE, FALSE,
			   0);
	gtk_misc_set_alignment(GTK_MISC(rating_label), 0.02, 0);

	rating = gtk_label_new("");
	gtk_widget_ref(rating);
	g_object_set_data_full(G_OBJECT(dlg_info), "rating", rating,
				 (GtkDestroyNotify) gtk_widget_unref);
	/* Widget shown later */
	gtk_box_pack_start(GTK_BOX(rating_hbox), rating, TRUE, TRUE, 0);
	gtk_label_set_line_wrap(GTK_LABEL(rating), TRUE);
	gtk_misc_set_alignment(GTK_MISC(rating), 0, 0.5);


	/* Add "Ranking" label */
	ranking_hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(ranking_hbox);
	g_object_set_data_full(G_OBJECT(dlg_info), "ranking_hbox",
				 ranking_hbox,
				 (GtkDestroyNotify) gtk_widget_unref);
	/* Widget shown later */
	gtk_box_pack_start(GTK_BOX(info_vbox), ranking_hbox, TRUE, TRUE, 0);

	ranking_label = gtk_label_new(_("Rank:"));
	gtk_widget_ref(ranking_label);
	g_object_set_data_full(G_OBJECT(dlg_info), "ranking_label",
				 ranking_label,
				 (GtkDestroyNotify) gtk_widget_unref);
	/* Widget shown later */
	gtk_box_pack_start(GTK_BOX(ranking_hbox), ranking_label, FALSE, FALSE,
			   0);
	gtk_misc_set_alignment(GTK_MISC(ranking_label), 0.02, 0);

	ranking = gtk_label_new("");
	gtk_widget_ref(ranking);
	g_object_set_data_full(G_OBJECT(dlg_info), "ranking", ranking,
				 (GtkDestroyNotify) gtk_widget_unref);
	/* Widget shown later */
	gtk_box_pack_start(GTK_BOX(ranking_hbox), ranking, TRUE, TRUE, 0);
	gtk_label_set_line_wrap(GTK_LABEL(ranking), TRUE);
	gtk_misc_set_alignment(GTK_MISC(ranking), 0, 0.5);


	/* Add "Highscore" label */
	highscore_hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(highscore_hbox);
	g_object_set_data_full(G_OBJECT(dlg_info), "highscore_hbox",
				 highscore_hbox,
				 (GtkDestroyNotify) gtk_widget_unref);
	/* Widget shown later */
	gtk_box_pack_start(GTK_BOX(info_vbox), highscore_hbox, TRUE, TRUE, 0);

	highscore_label = gtk_label_new(_("Score:"));
	gtk_widget_ref(highscore_label);
	g_object_set_data_full(G_OBJECT(dlg_info), "highscore_label",
				 highscore_label,
				 (GtkDestroyNotify) gtk_widget_unref);
	/* Widget shown later */
	gtk_box_pack_start(GTK_BOX(highscore_hbox), highscore_label, FALSE,
			   FALSE, 0);
	gtk_misc_set_alignment(GTK_MISC(highscore_label), 0.02, 0);

	highscore = gtk_label_new("");
	gtk_widget_ref(highscore);
	g_object_set_data_full(G_OBJECT(dlg_info), "highscore", highscore,
				 (GtkDestroyNotify) gtk_widget_unref);
	/* Widget shown later */
	gtk_box_pack_start(GTK_BOX(highscore_hbox), highscore, TRUE, TRUE, 0);
	gtk_label_set_line_wrap(GTK_LABEL(highscore), TRUE);
	gtk_misc_set_alignment(GTK_MISC(highscore), 0, 0.5);

	/* Add "Private chat" label */
	chat_hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(chat_hbox);
	g_object_set_data_full(G_OBJECT(dlg_info), "chat_hbox",
				 chat_hbox,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(chat_hbox);
	gtk_box_pack_start(GTK_BOX(info_vbox), chat_hbox, TRUE, TRUE, 0);

	chat_label = gtk_label_new(_("Message:"));
	gtk_widget_ref(chat_label);
	g_object_set_data_full(G_OBJECT(dlg_info), "chat_label",
				 chat_label,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(chat_label);
	gtk_box_pack_start(GTK_BOX(chat_hbox), chat_label, FALSE,
			   FALSE, 0);
	gtk_misc_set_alignment(GTK_MISC(chat_label), 0.02, 0);

	chat = gtk_entry_new();
	gtk_widget_ref(chat);
	g_object_set_data_full(G_OBJECT(dlg_info), "chat", chat,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(chat);
	gtk_box_pack_start(GTK_BOX(chat_hbox), chat, TRUE, TRUE, 0);
	/* gtk_misc_set_alignment(GTK_MISC(chat), 0, 0.5); */
	g_signal_connect(chat, "activate",
			 GTK_SIGNAL_FUNC(chat_activate), NULL);

	/* Make ACTION area. */
	dialog_action_area1 = GTK_DIALOG(dlg_info)->action_area;
	g_object_set_data(G_OBJECT(dlg_info), "dialog_action_area1",
			  dialog_action_area1);
	gtk_widget_show(dialog_action_area1);
	gtk_container_set_border_width(GTK_CONTAINER(dialog_action_area1),
				       10);


	button_box = gtk_hbutton_box_new();
	gtk_widget_ref(button_box);
	g_object_set_data_full(G_OBJECT(dlg_info), "button_box",
				 button_box,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(button_box);
	gtk_box_pack_start(GTK_BOX(dialog_action_area1), button_box, TRUE,
			   TRUE, 0);

	ok_button = gtk_button_new_from_stock(GTK_STOCK_OK);
	gtk_widget_ref(ok_button);
	g_object_set_data_full(G_OBJECT(dlg_info), "ok_button", ok_button,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(ok_button);
	gtk_container_add(GTK_CONTAINER(button_box), ok_button);
	GTK_WIDGET_SET_FLAGS(ok_button, GTK_CAN_DEFAULT);

	g_signal_connect(GTK_OBJECT(dlg_info), "destroy",
			   GTK_SIGNAL_FUNC(gtk_widget_destroyed), &dialog);
	g_signal_connect_swapped(GTK_OBJECT(ok_button), "clicked",
				  GTK_SIGNAL_FUNC(gtk_widget_destroy),
				  GTK_OBJECT(dlg_info));

	return dlg_info;
}
