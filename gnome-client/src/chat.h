/*
 * File: chat.h
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

#ifndef _CHAT_
#define _CHAT_

#include "xtext.h"
#include "ggzcore.h"

typedef enum {
	CHAT_MSG = 0x0001,
	CHAT_PRVMSG = 0x0002,
	CHAT_ANNOUNCE = 0x0003,
	CHAT_SEND_PRVMSG = 0x0004,
	CHAT_LOCAL_NORMAL = 0x0005,
	CHAT_LOCAL_HIGH = 0x0006
} CHATTypes;

void chat_init(void);
void chat_allocate_colors(void);
void chat_display_message(CHATTypes id, char *player, char *message);
void chat_send(gchar * message);
void chat_enter(const gchar * player);
void chat_part(const gchar * player);
void chat_help(void);
int chat_checkurl(GtkXText * xtext, char *word);
void chat_word_clicked(GtkXText * xtext, char *word,
		       GdkEventButton * event);
void chat_add_friend(gchar * name, gint display);
void chat_remove_friend(gchar * name);
void chat_add_ignore(gchar * name, gint display);
void chat_remove_ignore(gchar * name);
void chat_save_lists(void);
const gchar *chat_complete_name(gchar * name);
gint chat_is_friend(gchar * name);
gint chat_is_ignore(gchar * name);
void chat_lists_cleanup(void);

#endif
