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

/*
 * Description
 *
 * This file contains all functions that are chat related.
 */

#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "chat.h"
#include "ggzcore.h"
#include "login.h"
#include "xtext.h"
#include "support.h"

static gchar *chat_get_color(gchar *name);
extern GtkWidget *win_main;
extern GGZServer *server;

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
        {0, 0xcccc, 0xffff, 0xcccc},    /* 10  ?????                    */
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


/* chat_allocate_colors() - Allocates the collors all at once so they
 *                          can be called without the need to allocate
 *                          each time.
 *
 * Recieves:
 *
 * Returns:   
 */

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

/* chat_display_message() - Adds text to the xtext widget wich is used to diaplying
 *                          chatt and system messages.
 *
 * Recieves:
 * CHATTypes	id	: How the chat should be displayed
 * char		*player	: Diaplayed to the left of the line
 * char		*message: Displayed to the right of the line
 *
 * Returns:
 */

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
				name = g_strdup_printf("<\003%s%s\00300>", chat_get_color(player), player);
			        gtk_xtext_append_indent(GTK_XTEXT(tmp), name, strlen(name), message, strlen(message));
			}
			break;
		case CHAT_PRVMSG:
			name = g_strdup_printf(">\003%s%s\00300<", chat_get_color(player), player);
		        gtk_xtext_append_indent(GTK_XTEXT(tmp), name, strlen(name), message, strlen(message));
			break;
		case CHAT_BEEP:
			name = g_strdup_printf("%s", player);
		        gtk_xtext_append_indent(GTK_XTEXT(tmp), name, strlen(name), message, strlen(message));
			break;
		case CHAT_ANNOUNCE:
			name = g_strdup_printf("[\003%s%s\00300]", chat_get_color(player), player);
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


/* chat_send_msg() - sends the current text on chat_entry to the server
 *
 * Recieves:
 *
 * Returns:
 */

void chat_send_msg(GGZServer *server)
{
        GtkEntry *tmp = NULL;
	GGZRoom *room = ggzcore_server_get_cur_room(server);

	tmp = gtk_object_get_data(GTK_OBJECT(win_main), "chat_entry");
        if (strcmp(gtk_entry_get_text(GTK_ENTRY(tmp)),""))
        {
                /* Send the current text */
		ggzcore_room_chat(room, GGZ_CHAT_NORMAL, NULL, gtk_entry_get_text(GTK_ENTRY(tmp)));
        
                /* Clear the entry box */
                gtk_entry_set_text(GTK_ENTRY(tmp), "");
        }
}


/* chat_send_prvmsg() - Sends a privet nessage to a users
 *
 * Recieves:
 *
 * Returns:
 */

void chat_send_prvmsg(GGZServer *server)
{
	GGZRoom *room = ggzcore_server_get_cur_room(server);
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

				ggzcore_room_chat(room, GGZ_CHAT_PERSONAL, name, name+1+i);
				chat_display_message(CHAT_SEND_PRVMSG, data[0], data[1]);
				i = strlen(name)+1;
			}
		}

                /* Clear the entry box */
                gtk_entry_set_text(GTK_ENTRY(tmp), "");
        }
}


/* chat_send_beep() - Sends a beep to a user
 *
 * Recieves:
 *
 * Returns:
 */

void chat_send_beep(GGZServer *server)
{
        GtkEntry *tmp = NULL;
	GGZRoom *room = ggzcore_server_get_cur_room(server);
	char *player;

	tmp = gtk_object_get_data(GTK_OBJECT(win_main), "chat_entry");
	player = strdup(gtk_entry_get_text(GTK_ENTRY(tmp))+6);
	ggzcore_room_chat(room, GGZ_CHAT_BEEP, player, NULL);

        /* Clear the entry box */
        gtk_entry_set_text(GTK_ENTRY(tmp), "");

	tmp = gtk_object_get_data(GTK_OBJECT(win_main), "xtext_custom");
        gtk_xtext_append_indent(GTK_XTEXT(tmp), "---", 3, _("Beep sent"), 9);

}


/* chat_enter() - Displays a message in the chat area when someone
 *                enters the room
 *
 * Recieves:
 * gchar	*player	: The players username
 *
 * Returns:
 */

void chat_enter(gchar *player)
{
        GtkEntry *tmp = NULL;

        if( !ggzcore_conf_read_int("CHAT", "IGNORE", FALSE) )
        {
	        tmp = gtk_object_get_data(GTK_OBJECT(win_main), "xtext_custom");
        	gtk_xtext_append_indent(GTK_XTEXT(tmp), "-->", 3, player, strlen(player));
	}
}


/* chat_part() - Displays a message in the chat area when someone
 *               leaves the room
 *
 * Recieves:
 * gchar	*player	: The players username
 *
 * Returns:
 */

void chat_part(gchar *player)
{
        GtkEntry *tmp = NULL;

        if( !ggzcore_conf_read_int("CHAT", "IGNORE", FALSE) )
        {
	        tmp = gtk_object_get_data(GTK_OBJECT(win_main), "xtext_custom");
        	gtk_xtext_append_indent(GTK_XTEXT(tmp), "<--", 3, player, strlen(player));
	}
}

/* chat_help() - Displays help on all the chat commands
 *
 * Recieves:
 *
 * Returns:
 */

void chat_help(void)
{
	chat_display_message(CHAT_BEEP, "---", _("Chat Commands"));
	chat_display_message(CHAT_BEEP, "---", _("-------------"));
	chat_display_message(CHAT_BEEP, "---", _("/me <action>"));
	chat_display_message(CHAT_BEEP, "---", _("/msg <username> <message>"));
	chat_display_message(CHAT_BEEP, "---", _("/beep <username>"));
}



#define WORD_URL     1
#define WORD_GGZ     2
#define WORD_HOST    3
#define WORD_EMAIL   4

/* chat_checkurl() - Checks the current word to see if it is a valid URL
 *                   to underline.
 *
 * Recieves:
 * GtkXText	*xtest	: The xtext widget the word if from
 * char		word	: The current word to chek
 *
 * Returns:
 * 0		: Not a valid URL
 * WORD_*	: The URL Type
 */

int chat_checkurl(GtkXText *xtext, char *word)
{
	char *at, *dot;
	int i, dots;
	int len = strlen (word);

	/* Check for URLs */
	if (!strncasecmp (word, "ftp.", 4))
		return WORD_URL;
	if (!strncasecmp (word, "ftp:", 4))
		return WORD_URL;
	if (!strncasecmp (word, "www.", 4))
		return WORD_URL;
	if (!strncasecmp (word, "http:", 5))
		return WORD_URL;
	if (!strncasecmp (word, "https:", 6))
		return WORD_URL;
	if (!strncasecmp (word, "ggz.", 4))
		return WORD_GGZ;
	if (!strncasecmp (word, "ggz://", 6))
		return WORD_GGZ;

	/* Check for email addresses */
	at = strchr (word, '@');
	dot = strrchr (word, '.');
	if (at && dot)
	{
		if ((unsigned long) at < (unsigned long) dot)
		{

			if (strchr (word, '*'))
				return WORD_HOST;
			else
				return WORD_EMAIL;
		}
	}

	/* Check for IPs */
	dots = 0;
	for (i = 0; i < len; i++)
	{
		if (word[i] == '.')
			dots++;
	}
	if(dots == 3)
	{
		if (inet_addr(word) != -1)
			return WORD_HOST;
	}

	if (!strncasecmp (word + len - 5, ".html", 5))
		return WORD_HOST;
	if (!strncasecmp (word + len - 4, ".org", 4))
		return WORD_HOST;
	if (!strncasecmp (word + len - 4, ".net", 4))
		return WORD_HOST;
	if (!strncasecmp (word + len - 4, ".com", 4))
		return WORD_HOST;
	if (!strncasecmp (word + len - 4, ".edu", 4))
		return WORD_HOST;


	/* nothing of intrest to us */
	return 0;
}

/* chat_word_clicked() - The callback for clicking on a word in a
 *                       xtext widget
 *
 * Recieves:
 * GtkXText		*xtext	: The current xtext widget
 * char			*word	: The word that was clicked
 * GdkEventButton	*event	: The button event used to click the word
 *
 * Returns:
 */

void chat_word_clicked(GtkXText *xtext, char *word,
        GdkEventButton *event)
{
	switch(chat_checkurl(xtext, word))
	{
		case WORD_GGZ:
			login_goto_server(word);
			break;
		case WORD_HOST:
		case WORD_URL:
			goto_url(word);
			break;
		default:
			break;
	}
}


/* chat_get_color() - Get the color for a user based on name
 *
 * Recieves:
 * gchar		*name	: The name to get the color for
 *
 * Returns:
 * gchar		*color	: The color to use
 */

gchar *chat_get_color(gchar *name)
{
	gint asc;
	div_t x;

	if(!ggzcore_conf_read_int("CHAT", "COLOR", TRUE))
	{
		/* Dont use color */
		return "00";
	} else {
		if(ggzcore_conf_read_int("CHAT", "SOME_COLOR", TRUE))
		{
			if(!strcmp(name, ggzcore_server_get_handle(server)))
			{
				if(ggzcore_conf_read_int("CHAT", "Y_COLOR", 8) > 9)
					return g_strdup_printf("%d", ggzcore_conf_read_int("CHAT", "Y_COLOR", 8));
				else
					return g_strdup_printf("0%d", ggzcore_conf_read_int("CHAT", "Y_COLOR", 8));
			}

			if(ggzcore_conf_read_int("CHAT", "O_COLOR", 2) > 9)
				return g_strdup_printf("%d", ggzcore_conf_read_int("CHAT", "O_COLOR", 2));
			else
				return g_strdup_printf("0%d", ggzcore_conf_read_int("CHAT", "O_COLOR", 2));
		} else if(ggzcore_conf_read_int("CHAT", "FULL_COLOR", FALSE)) {
			asc = (gint)name[0];
			x = div(asc, 17);
			return g_strdup_printf("%d", x.rem);
		}
	}

	return "00";

}
