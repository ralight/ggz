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
#include "ggzrc.h"
#include "dlg_main.h"
#include "xtext.h"

void chat_print(int color, char* left, char* right);

extern GtkWidget *main_win;
extern struct ConnectInfo connection;

gint color_index;	/* color for chat */

void display_chat(gchar *name, gchar *msg)
{
	gchar *buf;
	gchar cmd[1024];		/* command used in chat */
	gchar out[1024];		/* text following command */
	gchar *line;			/* line to parse */
	gint cmd_index=0, out_index=0;	/* indexes */

	line=g_strdup(msg);
	strcpy(line,msg);

	/* Get color for user */
	color_index = -1;
	if(ggzrc_read_int("CHAT","ColorNames", CHAT_COLOR_SOME) == CHAT_COLOR_SOME)
	{
		color_index=1;
		if (!strcmp(connection.username, name))
			color_index=0;
	}
        
        /* Skip until we find the end of the command, converting */
        /* everything (for convenience) to lowercase as we go */
        while(*line != ' ' && *line != '\n' && *line != '\0'){
                *line = tolower(*line);
		cmd[cmd_index]=*line;
                line++;
		cmd_index++;
        }
	cmd[cmd_index]='\0';
        line++;

	/* get all remaining text */
        while(*line != '\0'){
		out[out_index]=*line;
                line++;
		out_index++;
        }
	out[out_index]='\0';


	if (!strcmp(cmd,"/me")){		/* Action, like in IRC */
		buf = g_strdup_printf("%s %s", name, out);
		chat_print(color_index, "*", buf);
		g_free(buf);
	}else if (!strcmp(cmd,"/beep")){	/* Beep a person threw chat */
		if (!strcmp(out,connection.username)){
			if(ggzrc_read_int("CHAT","Beep",TRUE) == TRUE)
				gdk_beep();
			buf = g_strdup_printf("You've been beeped by %s", name);
			chat_print(color_index, "---", buf);
			g_free(buf);
		}else{
			buf = g_strdup_printf("%s was been beeped by %s", out, name);
			chat_print(color_index, "---", buf);
			g_free(buf);
		}
	}else if (!strcmp(cmd,"/sjoins")){	/* Server message join room */
		if(ggzrc_read_int("CHAT","IgnoreJoinPart",FALSE) == FALSE)
			chat_print(color_index, "-->", name);
	}else if (!strcmp(cmd,"/sparts")){	/* Server message part room */
		if(ggzrc_read_int("CHAT","IgnoreJoinPart",FALSE) == FALSE)
			chat_print(color_index, "<--", name);
	}else{		/* No command given, display it all */
		chat_print(color_index, name, msg);
	}
}

void chat_print(int color, char* left, char* right)
{
	gpointer tmp;			/* chat widget */

	if(color == -1)
	{
		tmp = gtk_object_get_data(GTK_OBJECT(main_win), "chat_text");
		gtk_xtext_append_indent(GTK_XTEXT(tmp), left, strlen(left), right, strlen(right));
		gtk_xtext_refresh(tmp);
	} else {
		tmp = gtk_object_get_data(GTK_OBJECT(main_win), "chat_text");
		gtk_xtext_append_indent(GTK_XTEXT(tmp), left, strlen(left), right, strlen(right));
		gtk_xtext_refresh(tmp);
	}
}
