/* 
 * File: dlg_chat.c
 * Author: Jason Short
 * Project: GGZ GTK Games
 * Date: 2/20/2004 (moved from GGZCards)
 * Desc: Create the "Chat" Gtk dialog
 * $Id: dlg_chat.c 6268 2004-11-05 08:00:15Z jdorje $
 *
 * This file implements a chat widget.  Much of the code is taken from
 * Freeciv's chat widget, written by Vasco Alexandre da Silva Costa.
 *
 * Copyright (C) 2004 GGZ Development Team
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
#  include <config.h>		/* Site-specific config */
#endif

#include <stdio.h>

#include <gtk/gtk.h>
#include <ggzmod.h>

#include "dlg_chat.h"
#include "ggzintl.h"


typedef struct chatwidgets {
	GtkWidget *container;
	GtkWidget *text;
	struct chatwidgets *next;
} ChatWidgets;

static ChatWidgets *chats;
static GtkWidget *dlg_chat;
GGZMod *ggz;

static void handle_ggz_chat_event(GGZMod *ggzmod, GGZModEvent e, void *data)
{
	GGZChat *chat = data;
	ChatWidgets *list;
	char message[1024];
	GtkTextBuffer *buf;
	GtkTextIter i;
	GtkTextMark *mark;
	snprintf(message, sizeof(message), "\n%s : %s",
		 chat->player, chat->message);

	for (list = chats; list; list = list->next) {
		if (!list->container) continue;

		buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(list->text));
		gtk_text_buffer_get_end_iter(buf, &i);
		gtk_text_buffer_insert(buf, &i, message, -1);

		/* have to use a mark, or this won't work properly */
		gtk_text_buffer_get_end_iter(buf, &i);
		mark = gtk_text_buffer_create_mark(buf, NULL, &i, FALSE);
		gtk_text_view_scroll_mark_onscreen(GTK_TEXT_VIEW(list->text),
						   mark);
		gtk_text_buffer_delete_mark(buf, mark);
	}
}

void init_chat(GGZMod *ggzmod)
{
	ggz = ggzmod;
	ggzmod_set_handler(ggzmod, GGZMOD_EVENT_CHAT, handle_ggz_chat_event);
}

static void inputline_return(GtkEntry *w, gpointer data)
{
	const char *theinput = gtk_entry_get_text(w);

	if (*theinput) {
		ggzmod_request_chat(ggz, theinput);
	}

	gtk_entry_set_text(w, "");
}

GtkWidget *create_chat_widget(void)
{
	GtkWidget *vbox, *text, *inputline;
	ChatWidgets *list;
	GtkWidget *sw;

	vbox = gtk_vbox_new(FALSE, 0);

	/* Create a scrolled window for the chat lines. */
	sw = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(sw),
					    GTK_SHADOW_ETCHED_IN);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_ALWAYS);

	gtk_box_pack_start(GTK_BOX(vbox), sw, TRUE, TRUE, 0);
	gtk_widget_set_size_request(sw, -1, 50);

	/* The chatlines themselves go into a text window. */
	text = gtk_text_view_new();
	gtk_text_view_set_editable(GTK_TEXT_VIEW(text), FALSE);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(text), FALSE);
	gtk_container_add(GTK_CONTAINER(sw), text);
	gtk_widget_set_name(text, "chatline");
  
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text), GTK_WRAP_WORD);
	gtk_text_view_set_left_margin(GTK_TEXT_VIEW(text), 5);

	/* Text entry widget. */
	inputline = gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(vbox), inputline, FALSE, FALSE, 3);

	gtk_widget_show_all(vbox);

	list = ggz_malloc(sizeof(*list));
	list->next = chats;
	list->container = vbox;
	list->text = text;
	chats = list;

	g_signal_connect(inputline, "activate",
			 G_CALLBACK(inputline_return), NULL);

	g_signal_connect(vbox, "destroy",
			 GTK_SIGNAL_FUNC(gtk_widget_destroyed),
			 &list->container);

	return vbox;
}

static GtkWidget *create_dlg_chat(void)
{
	GtkWidget *dialog;
	GtkWidget *vbox;
	GtkWidget *chat_widget;
	GtkWidget *action_area;
	GtkWidget *close_button;

	/* 
	 * Create outer window.
	 */
	dialog = gtk_dialog_new();
	g_object_set_data(G_OBJECT(dialog), "dlg_players", dialog);
	gtk_window_set_title(GTK_WINDOW(dialog), _("Player List"));

	/* 
	 * Get vertical box packing widget.
	 */
	vbox = GTK_DIALOG(dialog)->vbox;
	g_object_set_data(G_OBJECT(dialog), "vbox", vbox);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
	gtk_widget_show(vbox);

	chat_widget = create_chat_widget();
	g_object_set_data_full(G_OBJECT(dialog),
			       "chat_widget", chat_widget,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_box_pack_start(GTK_BOX(vbox), chat_widget, FALSE, FALSE, 0);


	/* 
	 * Get "action area"
	 */
	action_area = GTK_DIALOG(dialog)->action_area;
	gtk_widget_show(action_area);

	/* 
	 * Make "close" button
	 */
	close_button = gtk_button_new_from_stock(GTK_STOCK_CLOSE);
	gtk_widget_ref(close_button);
	gtk_widget_show(close_button);
	gtk_box_pack_start(GTK_BOX(action_area), close_button, FALSE, FALSE,
			   0);
	g_signal_connect_swapped(close_button, "clicked",
				 GTK_SIGNAL_FUNC(gtk_widget_destroy),
				 dialog);

	/* 
	 * Set up callbacks
	 */
	g_signal_connect_swapped(dialog, "delete_event",
				 GTK_SIGNAL_FUNC(gtk_widget_destroy),
				 dialog);

	/* 
	 * Done!
	 */
	return dialog;
}

void create_or_raise_dlg_chat(void)
{
	if (dlg_chat) {
		gdk_window_show(dlg_chat->window);
		gdk_window_raise(dlg_chat->window);
	} else {
		dlg_chat = create_dlg_chat();
		g_signal_connect(dlg_chat, "destroy",
				 GTK_SIGNAL_FUNC(gtk_widget_destroyed),
				 &dlg_chat);
		gtk_widget_show(dlg_chat);
	}
}

void toggle_chat_window(gpointer data, guint action, GtkWidget *w)
{
	ChatWidgets *list;

	for (list = chats; list; list = list->next) {
		if (!list->container) continue;

		if (GTK_CHECK_MENU_ITEM(w)->active) {
			gtk_widget_show(list->container);
		} else {
			gtk_widget_hide(list->container);
		}
	}
}
