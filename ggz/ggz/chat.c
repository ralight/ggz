/*
 * File: chat.c
 * Author: Justin Zaun
 * Project: GGZ Client
 * Date: 04/26/2000
 *
 * This fils contains functions for connecting with the server
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

#include <gtk/gtk.h>
#include <string.h>
#include <ctype.h>

#include "chat.h"
#include "datatypes.h"
#include "dlg_main.h"
#include "ggzrc.h"
#include "protocols.h"
#include "xtext.h"

void chat_print(int color, char* left, char* right);

extern GtkWidget *main_win;
extern struct ConnectInfo client;

gint color_index;	/* color for chat */

void display_chat(guchar op, gchar *name, gchar *msg)
{
	gchar *buf;

	/* Get color for user */
	color_index = ggzrc_read_int("CHAT","ColorNames", CHAT_COLOR_SOME);
        
	switch(op)
	{
		case GGZ_CHAT_NORMAL:
			if(!strncasecmp(msg, "/me ", 4))
			{
				buf = g_strdup_printf("%s %s", name, msg+4);
				chat_print(color_index, "*", buf);
				g_free(buf);
			}else{
				buf = g_strdup_printf("<%s>", name);
				chat_print(color_index, buf, msg);
				g_free(buf);
			}
			break;
		case GGZ_CHAT_ANNOUNCE:
			buf = g_strdup_printf("[%s]", name);
			chat_print(color_index, buf, msg);
			g_free(buf);
			break;
		case GGZ_CHAT_BEEP:
			if(ggzrc_read_int("CHAT","Beep",TRUE) == TRUE) 
				gdk_beep();
			buf = g_strdup_printf("You've been beeped by %s.", name);
			chat_print(CHAT_COLOR_SERVER, "---", buf);
			g_free(buf);
			break;
		case GGZ_CHAT_PERSONAL:
			buf = g_strdup_printf(">%s<", name);
			chat_print(color_index, buf, msg);
			g_free(buf);
			break;
	}

}

void chat_print(int color, char* left, char* right)
{
	gpointer tmp;	/* chat widget */
	gchar *buf;	/* Temporary buffer */

	if(color == CHAT_COLOR_NONE)
	{
		tmp = gtk_object_get_data(GTK_OBJECT(main_win), "chat_text");
		gtk_xtext_append_indent(GTK_XTEXT(tmp), left, strlen(left), right, strlen(right));
		gtk_xtext_refresh(tmp);
	} else if(color == CHAT_COLOR_SOME) {
		tmp = gtk_object_get_data(GTK_OBJECT(main_win), "chat_text");
		buf = g_strdup_printf("<%s>",client.server.login); 
		if (!strcmp(buf, left))
		{
			buf = g_strdup_printf("\003%d%s\00300", 
			ggzrc_read_int("CHAT","YourColor",1), left);
			gtk_xtext_append_indent(GTK_XTEXT(tmp), buf, strlen(buf), right, strlen(right));
			g_free(buf);
		} else {
			buf = g_strdup_printf("\003%d%s\00300", 
			ggzrc_read_int("CHAT","OthersColor",12), left);
			gtk_xtext_append_indent(GTK_XTEXT(tmp), buf, strlen(buf), right, strlen(right));
			g_free(buf);
		}
		gtk_xtext_refresh(tmp);
	} else if(color == CHAT_COLOR_FULL) {
		tmp = gtk_object_get_data(GTK_OBJECT(main_win), "chat_text");
		gtk_xtext_append_indent(GTK_XTEXT(tmp), left, strlen(left), right, strlen(right));
		gtk_xtext_refresh(tmp);
	} else if(color == CHAT_COLOR_SERVER) {
		tmp = gtk_object_get_data(GTK_OBJECT(main_win), "chat_text");
		gtk_xtext_append_indent(GTK_XTEXT(tmp), left, strlen(left), right, strlen(right));
		gtk_xtext_refresh(tmp);
	}
}
