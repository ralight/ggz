/*
 * File: chat.c
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

#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

#include "ggzcore.h"
#include "chat.h"
#include "xtext.h"

extern GtkWidget *win_main;

/* Aray of GdkColors currently used for chat and MOTD */
GdkColor colors[] =
{
        {0, 0, 0, 0},                   /* 0   black    */
        {0, 0, 0, 0xcccc},              /* 1   bule                     */
        {0, 0xcccc, 0, 0},              /* 2   red                      */
        {0, 0xbbbb, 0xbbbb, 0},         /* 3   yellow/brown             */
        {0, 0xbbbb, 0, 0xbbbb},         /* 4   purple                   */
        {0, 0xffff, 0xaaaa, 0},         /* 5   orange                   */
        {0, 0x7777, 0x7777, 0x7777},    /* 6   grey                     */
        {0, 0, 0xcccc, 0xcccc},         /* 7   aqua                     */
        {0, 0, 0, 0xcccc},              /* 8   blue markBack            */
        {0, 0, 0, 0},                   /* 9   black                    */
        {0, 0xcccc, 0xcccc, 0xcccc},    /* 10  white                    */
        {0, 0xffff, 0xffff, 0},         /* 11  yellow                   */
        {0, 0, 0xffff, 0},              /* 12  green                    */
        {0, 0, 0xffff, 0xffff},         /* 13  light aqua               */
        {0, 0, 0, 0xffff},              /* 14  blue                     */
        {0, 0xffff, 0, 0xffff},         /* 15  pink                     */
        {0, 0x9999, 0x9999, 0x9999},    /* 16  light grey               */
        {0, 0xeeee, 0xeeee, 0xeeee},    /* 17  white markFore           */
        {0, 0, 0, 0},                   /* 18  foreground (black)       */
        {0, 0xffff, 0xffff, 0xffff}     /* 19  background (white)       */
};

void chat_allocate_colors(void)
{
	gint i;
        /* Allocate standared colors */
        if (!colors[0].pixel)        /* don't do it again */
        {
                for (i = 0; i < 20; i++)
                {
                        colors[i].pixel = (gulong) ((colors[i].red & 0xff00) * 256 +
                                        (colors[i].green & 0xff00) +
                                        (colors[i].blue & 0xff00) / 256);
                        if (!gdk_color_alloc (gdk_colormap_get_system(),
                            &colors[i]))
                                g_error("*** GGZ: Couldn't alloc color\n");
                }
        }
}

void chat_display_message(CHATTypes id, char *player, char *message)
{
        GtkXText *tmp;
	gchar *name = NULL;

        tmp = gtk_object_get_data(GTK_OBJECT(win_main), "xtext_custom");
	switch(id)
	{
		case CHAT_MSG:
			if(!strncasecmp(message, "/me ", 4))
			{
				name = g_strdup_printf("%s %s", player, message+4);
			        gtk_xtext_append_indent(GTK_XTEXT(tmp), "*", 1, name, strlen(name));
			} else {
				name = g_strdup_printf("<%s>", player);
			        gtk_xtext_append_indent(GTK_XTEXT(tmp), name, strlen(name), message, strlen(message));
			}
			break;
		case CHAT_PRVMSG:
			name = g_strdup_printf(">%s<", player);
		        gtk_xtext_append_indent(GTK_XTEXT(tmp), name, strlen(name), message, strlen(message));
			break;
		case CHAT_BEEP:
			name = g_strdup_printf("%s", player);
		        gtk_xtext_append_indent(GTK_XTEXT(tmp), name, strlen(name), message, strlen(message));
			break;
		case CHAT_ANNOUNCE:
			name = g_strdup_printf("[%s]", player);
		        gtk_xtext_append_indent(GTK_XTEXT(tmp), name, strlen(name), message, strlen(message));
			break;
		case CHAT_SEND_PRVMSG:
			name = g_strdup_printf("--> %s", player);
		        gtk_xtext_append_indent(GTK_XTEXT(tmp), name, strlen(name), message, strlen(message));
			break;
	}
	g_free(name);
        gtk_xtext_refresh(tmp, 0);
}


void chat_send_msg(void)
{
        GtkEntry *tmp = NULL;

	tmp = gtk_object_get_data(GTK_OBJECT(win_main), "chat_entry");
        if (strcmp(gtk_entry_get_text(GTK_ENTRY(tmp)),""))
        {
                /* Send the current text */
                ggzcore_event_trigger(GGZ_USER_CHAT, gtk_entry_get_text(GTK_ENTRY(tmp)), NULL);
        
                /* Clear the entry box */
                gtk_entry_set_text(GTK_ENTRY(tmp), "");
        }
}


void chat_send_prvmsg(void)
{
        GtkEntry *tmp = NULL;
	gchar *name = NULL;
	gint i;
	char **data;

	tmp = gtk_object_get_data(GTK_OBJECT(win_main), "chat_entry");
        if (strcmp(gtk_entry_get_text(GTK_ENTRY(tmp)),""))
        {
		name = g_strdup(gtk_entry_get_text(GTK_ENTRY(tmp))+5);
		for(i = 0; i < strlen(name); i++)
		{
			if(name[i] == ' ')
			{
				name[i] = '\0';
				if(!(data = calloc(2, sizeof(char*))))
					ggzcore_error_sys_exit("calloc() failed in chat_send_prvmsg");

				data[0] = g_strdup(name);
				data[1] = g_strdup(name+1+i);
				ggzcore_event_trigger(GGZ_USER_CHAT_PRVMSG, data, g_free);
				chat_display_message(CHAT_SEND_PRVMSG, data[0], data[1]);
				i = strlen(name)+1;
			}
		}

                /* Clear the entry box */
                gtk_entry_set_text(GTK_ENTRY(tmp), "");
        }
}


void chat_send_beep(void)
{
        GtkEntry *tmp = NULL;

	tmp = gtk_object_get_data(GTK_OBJECT(win_main), "chat_entry");
	ggzcore_event_trigger(GGZ_USER_CHAT_BEEP,
		gtk_entry_get_text(GTK_ENTRY(tmp))+6, NULL);

        /* Clear the entry box */
        gtk_entry_set_text(GTK_ENTRY(tmp), "");

	tmp = gtk_object_get_data(GTK_OBJECT(win_main), "xtext_custom");
        gtk_xtext_append_indent(GTK_XTEXT(tmp), "---", 3, "Beep sent", 9);

}


void chat_enter(gchar *player)
{
        GtkEntry *tmp = NULL;

        tmp = gtk_object_get_data(GTK_OBJECT(win_main), "xtext_custom");
        gtk_xtext_append_indent(GTK_XTEXT(tmp), "-->", 3, player, strlen(player));
}


void chat_part(gchar *player)
{
        GtkEntry *tmp = NULL;

        tmp = gtk_object_get_data(GTK_OBJECT(win_main), "xtext_custom");
        gtk_xtext_append_indent(GTK_XTEXT(tmp), "<--", 3, player, strlen(player));
}

void chat_help(void)
{
	chat_display_message(CHAT_BEEP, "---", "Chat Commands");
	chat_display_message(CHAT_BEEP, "---", "-------------");
	chat_display_message(CHAT_BEEP, "---", "/me <action>");
	chat_display_message(CHAT_BEEP, "---", "/msg <username> <message>");
	chat_display_message(CHAT_BEEP, "---", "/beep <username>");
}
