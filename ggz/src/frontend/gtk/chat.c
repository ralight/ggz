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
#include <glib.h>

#include "chat.h"
#include "ggzcore.h"
#include "login.h"
#include "xtext.h"
#include "support.h"

void chat_allocate_colors(void);
void chat_send_msg(GGZServer *server, gchar *message);
void chat_send_prvmsg(GGZServer *server, gchar *message);
void chat_send_beep(GGZServer *server, gchar *message);
static gchar *chat_get_color(gchar *name, gchar *msg);
extern GtkWidget *win_main;
extern GGZServer *server;

/* Aray of GdkColors currently used for chat and MOTD */
/* They are all non-ditherable and as such should look the same everywhere */
GdkColor colors[] =
{
        {0, 0x0000, 0x0000, 0x0000},          /* 0   Black			*/
        {0, 0xFFFF, 0xFFFF, 0x3333},          /* 1   Dark Goldenrod		*/
        {0, 0xCCCC, 0x0000, 0x0000},          /* 2   Orange Red 3		*/
        {0, 0x6666, 0x9999, 0x0000},          /* 3   Olive Drab			*/
        {0, 0xCCCC, 0x3333, 0xCCCC},          /* 4   Medium Orchid		*/
        {0, 0x9999, 0x3333, 0x3333},          /* 5   Indian Red 4		*/
        {0, 0x0000, 0x6666, 0xFFFF},          /* 6   Royal Blue 2		*/
        {0, 0xFFFF, 0x9999, 0x3333},          /* 7   Tan 1			*/
        {0, 0x6666, 0xCCCC, 0xCCCC},          /* 8   Dark Slate Grey 3		*/
        {0, 0x6666, 0xCCCC, 0xFFFF},          /* 9   Cadet Blue			*/
        {0, 0x9999, 0x3333, 0xFFFF},          /* 10  Purple 2			*/
        {0, 0x9999, 0x0000, 0x6666},          /* 11  Violet Red 4		*/
        {0, 0x3333, 0x0000, 0x6666},          /* 12  Dark Blue			*/
        {0, 0x9999, 0x3333, 0x3333},          /* 13  Indian Red			*/
        {0, 0x3333, 0x6666, 0xFFFF},          /* 14  Blue			*/
        {0, 0x6666, 0xCCCC, 0xFFFF},          /* 15  Pale Violet Red		*/
        {0, 0xCCCC, 0xCCCC, 0x3333},          /* 16  Yellow 3			*/
        {0, 0x6666, 0xFFFF, 0xCCCC},          /* 17  Aquamarine 2		*/
        {0, 0xFFFF, 0xFFFF, 0xFFFF},          /* 18  forebround (White)		*/
        {0, 0x0000, 0x0000, 0x0000}           /* 19  background (Black)		*/
};

GdkColor ColorWhite = {0, 0xFFFF, 0xFFFF, 0xFFFF};
GdkColor ColorBlack = {0, 0x0000, 0x0000, 0x0000};

struct chatinfo {
	GArray *friends;
	GArray *ignore;
}chatinfo;

/* chat_init() - setsup chatinfo
 *
 * Recieves:
 *
 * Returns:   
 */
void chat_init(void)
{
	gchar *dummy;

	chat_allocate_colors();
	chatinfo.friends = g_array_new(TRUE, TRUE, sizeof(gchar*));
	chatinfo.ignore = g_array_new(TRUE, TRUE, sizeof(gchar*));

	dummy = g_strdup(" ");
	g_array_prepend_val(chatinfo.friends, dummy);
	g_array_prepend_val(chatinfo.ignore, dummy);
	g_free(dummy);
}


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
        ColorBlack.pixel = (gulong) ((ColorBlack.red & 0xff00) * 256 +
        			(ColorBlack.green & 0xff00) +
        			(ColorBlack.blue & 0xff00) / 256);
        if (!gdk_color_alloc (gdk_colormap_get_system(),
        	&ColorBlack))
        	g_error("*** GGZ: Couldn't alloc color\n");
        ColorWhite.pixel = (gulong) ((ColorWhite.red & 0xff00) * 256 +
        			(ColorWhite.green & 0xff00) +
        			(ColorWhite.blue & 0xff00) / 256);
        if (!gdk_color_alloc (gdk_colormap_get_system(),
        	&ColorWhite))
        	g_error("*** GGZ: Couldn't alloc color\n");

	/* Background */
	if (ggzcore_conf_read_int("CHAT", "BACKGROUND", TRUE) == TRUE)
	{
		colors[18] = ColorBlack;
		colors[19] = ColorWhite;
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
				name = g_strdup_printf("<\003%s%s\003>", chat_get_color(player, message),  player);
			        gtk_xtext_append_indent(GTK_XTEXT(tmp), name, strlen(name), message, strlen(message));
			}
			break;
		case CHAT_PRVMSG:
			name = g_strdup_printf(">\003%s%s\003<", chat_get_color(player, message), player);
		        gtk_xtext_append_indent(GTK_XTEXT(tmp), name, strlen(name), message, strlen(message));
			break;
		case CHAT_ANNOUNCE:
			name = g_strdup_printf("[\003%s%s\003]", chat_get_color(player, message), player);
		        gtk_xtext_append_indent(GTK_XTEXT(tmp), name, strlen(name), message, strlen(message));
			break;
		case CHAT_SEND_PRVMSG:
			name = g_strdup_printf("--> %s", player);
		        gtk_xtext_append_indent(GTK_XTEXT(tmp), name, strlen(name), message, strlen(message));
			break;
		case CHAT_LOCAL_NORMAL:
		        gtk_xtext_append_indent(GTK_XTEXT(tmp), "---", 3, message, strlen(message));
			break;
		case CHAT_LOCAL_HIGH:
		        gtk_xtext_append_indent(GTK_XTEXT(tmp), "***", 3, message, strlen(message));
			break;
	}
	g_free(name);
        gtk_xtext_refresh(tmp, 0);
}


/* chat_send() - Sends a chat message to the server
 *
 * Recieves:
 *	gchar	*message	: The text to send to the server as a chat message
 *
 * Returns:
 */

void chat_send(gchar *message)
{
	if(strcmp(message, ""))
	{
		if(strncasecmp(message, "/msg", 4) == 0)
			chat_send_prvmsg(server, message);
		else if(strncasecmp(message, "/beep", 5) == 0)
			chat_send_beep(server, message);
		else if(strncasecmp(message, "/help", 5) == 0)
			chat_help();
		else 
			chat_send_msg(server, message);
	}
}


/* chat_send_msg() - sends the current text on chat_entry to the server
 *
 * Recieves:
 *	GGZServer *server	: Currently connected server
 *	gchar *message		: The text to send as a normal message
 *
 * Returns:
 */

void chat_send_msg(GGZServer *server, gchar *message)
{
	GGZRoom *room = ggzcore_server_get_cur_room(server);

	/* Send the current text */
	ggzcore_room_chat(room, GGZ_CHAT_NORMAL, NULL, message);
}


/* chat_send_prvmsg() - Sends a private nessage to a users
 *
 * Recieves:
 *	GGZServer *server	: Currently connected server
 *	gchar *message		: The text to send as a private message
 *
 * Returns:
 */

void chat_send_prvmsg(GGZServer *server, gchar *message)
{
	GGZRoom *room = ggzcore_server_get_cur_room(server);
	gchar *name = NULL;
	gint i;

	name = g_strdup(message+5);
	for(i = 0; i < strlen(name); i++)
	{
		if(name[i] == ' ')
		{
			name[i] = '\0';
			ggzcore_room_chat(room, GGZ_CHAT_PERSONAL, name, name+1+i);
			chat_display_message(CHAT_SEND_PRVMSG, name, name+1+i);
			i = strlen(name)+1;
			return;
		}
	}

	/* Shouldn't get here */
	chat_display_message(CHAT_LOCAL_NORMAL, NULL, _("Usage: /msg <username> <message>"));
	chat_display_message(CHAT_LOCAL_NORMAL, NULL, _("    Sends a private message to a user on the network."));
}



/* chat_send_beep() - Sends a beep to a user
 *
 * Recieves:
 *	GGZServer *server	: Currently connected server
 *	gchar *message		: The text to send as a beep message
 *
 * Returns:
 */

void chat_send_beep(GGZServer *server, gchar *message)
{
	GGZRoom *room = ggzcore_server_get_cur_room(server);
	char *player;

	player = strdup(message+6);
	ggzcore_room_chat(room, GGZ_CHAT_BEEP, player, NULL);

	chat_display_message(CHAT_LOCAL_NORMAL, NULL, _("Beep Sent"));
}


/* chat_enter() - Displays a message in the chat area when someone
 *                enters the room
 *
 * Recieves:
 * 	gchar *player	: The players username
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
	chat_display_message(CHAT_LOCAL_NORMAL, NULL, _("Chat Commands"));
	chat_display_message(CHAT_LOCAL_NORMAL, NULL, _("-------------"));
	chat_display_message(CHAT_LOCAL_NORMAL, NULL, _("/me <action>"));
	chat_display_message(CHAT_LOCAL_NORMAL, NULL, _("/msg <username> <message>"));
	chat_display_message(CHAT_LOCAL_NORMAL, NULL, _("/beep <username>"));
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
			support_goto_url(word);
			break;
		default:
			break;
	}
}


/* chat_get_color() - Get the color for a user based on name
 *
 * Recieves:
 * gchar		*name	: The name to get the color for
 * gchar 		*msg	: Message that was sent
 *
 * Returns:
 * gchar		*color	: The color to use
 */

gchar *chat_get_color(gchar *name, gchar *msg)
{
	int pos;

	/* Is our name in the message? */
	if(strlen(msg) > strlen(ggzcore_server_get_handle(server))+1)
	{
		for(pos=0; pos<strlen(msg)-strlen(ggzcore_server_get_handle(server)); pos++)
		{
			if(!strncasecmp(msg + pos, ggzcore_server_get_handle(server), strlen(ggzcore_server_get_handle(server))))
			{
				if(ggzcore_conf_read_int("CHAT", "H_COLOR", 1) > 9)
					return g_strdup_printf("%d", ggzcore_conf_read_int("CHAT", "H_COLOR", 1));
				else
					return g_strdup_printf("0%d", ggzcore_conf_read_int("CHAT", "H_COLOR", 1));
			}
		}
	}

	/* Is a friend talking? */
	pos = 0;
	while(1)
	{
		/* have we hit the end of the list */
		if(g_array_index(chatinfo.friends, gchar*, pos) == NULL)
			break;
		if(!strcmp(g_array_index(chatinfo.friends, gchar*, pos), name))
		{
			if(ggzcore_conf_read_int("CHAT", "F_COLOR", 6) > 9)
				return g_strdup_printf("%d", ggzcore_conf_read_int("CHAT", "F_COLOR", 6));
			else
				return g_strdup_printf("0%d", ggzcore_conf_read_int("CHAT", "F_COLOR", 6));
		}
		pos++;
	}

	/* Normal color */
	if(ggzcore_conf_read_int("CHAT", "N_COLOR", 2) > 9)
		return g_strdup_printf("%d", ggzcore_conf_read_int("CHAT", "N_COLOR", 2));
	else
		return g_strdup_printf("0%d", ggzcore_conf_read_int("CHAT", "N_COLOR", 2));

	return "00";
}

/* chat_add_friend() - Adds a name to your friends list
 *
 * Recieves:
 * gchar	*name	: name to add
 *
 * Returns:
 */

void chat_add_friend(gchar *name)
{
	gchar *out;

	g_array_append_val(chatinfo.friends, name);
	out = g_strdup_printf(_("Added %s to your friends list."), name);
	chat_display_message(CHAT_LOCAL_NORMAL, "---", out);
	g_free(out);
}

/* chat_remove_friend() - Adds a name to your friends list
 *
 * Recieves:
 * gchar	*name	: name to add
 *
 * Returns:
 */

void chat_remove_friend(gchar *name)
{
	int x=0;
	gchar *out;

	while(1)
	{
		/* have we hit the end of the list */
		if(g_array_index(chatinfo.friends, gchar*, x) == NULL)
			break;
		if(!strcmp(g_array_index(chatinfo.friends, gchar*, x), name))
		{
			g_array_remove_index_fast(chatinfo.friends, x);
			out = g_strdup_printf(_("Removed %s from your friends list."), name);
			chat_display_message(CHAT_LOCAL_NORMAL, "---", out);
			g_free(out);
			break;
		}
		x++;
	}
}

