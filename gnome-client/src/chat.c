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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <glib.h>
#include <gnome.h>

#include "chat.h"
#include "ggz.h"
#include "ggzcore.h"
#include "login.h"
#include "xtext.h"
#include "support.h"

void chat_allocate_colors(void);
void chat_send_msg(GGZServer * server, gchar * message);
void chat_send_wall(GGZServer * server, gchar * message);
void chat_send_prvmsg(GGZServer * server, gchar * message);
void chat_send_beep(GGZServer * server, gchar * message);
void chat_save_lists(void);
void chat_load_lists(void);
void chat_list_friend(void);
void chat_list_ignore(void);
static const gchar *chat_get_color(gchar * name, gchar * msg);
extern GtkWidget *interface;
extern GGZServer *server;

static int friend_count;
static int ignore_count;

/* Aray of GdkColors currently used for chat and MOTD
 * They are all non-ditherable and as such should look the same everywhere
 */
GdkColor colors[] = {
	{0, 0x0000, 0x0000, 0x0000},	/* 0   Black                      */
	{0, 0xFFFF, 0xFFFF, 0x3333},	/* 1   Dark Goldenrod             */
	{0, 0xCCCC, 0x0000, 0x0000},	/* 2   Orange Red 3               */
	{0, 0x6666, 0x9999, 0x0000},	/* 3   Olive Drab                 */
	{0, 0xCCCC, 0x3333, 0xCCCC},	/* 4   Medium Orchid              */
	{0, 0x9999, 0x3333, 0x3333},	/* 5   Indian Red 4               */
	{0, 0x0000, 0x6666, 0xFFFF},	/* 6   Royal Blue 2               */
	{0, 0xFFFF, 0x9999, 0x3333},	/* 7   Tan 1                      */
	{0, 0x6666, 0xCCCC, 0xCCCC},	/* 8   Dark Slate Grey 3          */
	{0, 0x6666, 0xCCCC, 0xFFFF},	/* 9   Cadet Blue                 */
	{0, 0x9999, 0x3333, 0xFFFF},	/* 10  Purple 2                   */
	{0, 0x9999, 0x0000, 0x6666},	/* 11  Violet Red 4               */
	{0, 0x3333, 0x0000, 0x6666},	/* 12  Dark Blue                  */
	{0, 0x9999, 0x3333, 0x3333},	/* 13  Indian Red                 */
	{0, 0x3333, 0x6666, 0xFFFF},	/* 14  Blue                       */
	{0, 0x6666, 0xCCCC, 0xFFFF},	/* 15  Pale Violet Red            */
	{0, 0xCCCC, 0xCCCC, 0x3333},	/* 16  Yellow 3                   */
	{0, 0x6666, 0xFFFF, 0xCCCC},	/* 17  Aquamarine 2               */
	{0, 0xFFFF, 0xFFFF, 0xFFFF},	/* 19  foreground (White)         */
	{0, 0x0000, 0x0000, 0x0000}	/* 18  background (Black)         */
};


/* chatinfo holds 2 arrays one for friends and one for 
 * people to ignore. Friends are shown in a different 
 * color in chat and ingnored people's chats don't show
 * up at all.
 */
struct chatinfo {
	GArray *friends;
	GArray *ignore;
} chatinfo;

/* chat_init() - setup chatinfo and allocates colors
 *
 * Recieves:
 *
 * Returns:   
 */
void chat_init(void)
{
	chat_allocate_colors();
	chatinfo.friends = g_array_new(FALSE, FALSE, sizeof(gchar *));
	chatinfo.ignore = g_array_new(FALSE, FALSE, sizeof(gchar *));

	chat_load_lists();
}


/* chat_allocate_colors() - Allocates the colors all at once so they
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
	if (!colors[0].pixel) {	/* don't do it again */
		for (i = 0; i < 20; i++) {
			colors[i].pixel =
			    (gulong) ((colors[i].red & 0xff00) * 256 +
				      (colors[i].green & 0xff00) +
				      (colors[i].blue & 0xff00) / 256);
			if (!gdk_color_alloc
			    (gdk_colormap_get_system(), &colors[i]))
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
	GtkWidget *tmp = NULL;
	gchar *name = NULL;
	gchar *command;
	gint ignore = FALSE;
	int i;
	char *p;

	/* are we ignoring this person? */
	if (player != NULL) {
		for (i = 0; i < ignore_count; i++) {
			p = g_array_index(chatinfo.ignore, char *, i);
			if (!strcmp(p, player))
				ignore = TRUE;
		}
	}

	if (ignore == FALSE) {
		tmp = lookup_widget(interface, "custChatXText");
		switch (id) {
		case CHAT_MSG:
			if (!strncasecmp(message, "/me ", 4)) {
				name =
				    g_strdup_printf("%s %s", player,
						    message + 4);
				gtk_xtext_append_indent(GTK_XTEXT(tmp)->
							buffer, "*", 1,
							name,
							strlen(name));
			} else {
				name =
				    g_strdup_printf("<\003%s%s\003>",
						    chat_get_color(player,
								   message),
						    player);
				gtk_xtext_append_indent(GTK_XTEXT(tmp)->
							buffer, name,
							strlen(name),
							message,
							strlen(message));
			}
			if (ggzcore_conf_read_int("CHAT", "RSYNTH", FALSE)) {
				command =
				    g_strdup_printf
				    ("esddsp say -f 8 \"%s\"", message);
				//support_exec(command);
				g_free(command);
			}
			break;
		case CHAT_PRVMSG:
			name =
			    g_strdup_printf(">\003%s%s\003<",
					    chat_get_color(player,
							   message),
					    player);
			gtk_xtext_append_indent(GTK_XTEXT(tmp)->buffer,
						name, strlen(name),
						message, strlen(message));
			break;
		case CHAT_ANNOUNCE:
			name =
			    g_strdup_printf("[\003%s%s\003]",
					    chat_get_color(player,
							   message),
					    player);
			gtk_xtext_append_indent(GTK_XTEXT(tmp)->buffer,
						name, strlen(name),
						message, strlen(message));
			break;
		case CHAT_SEND_PRVMSG:
			name = g_strdup_printf("--> %s", player);
			gtk_xtext_append_indent(GTK_XTEXT(tmp)->buffer,
						name, strlen(name),
						message, strlen(message));
			break;
		case CHAT_LOCAL_NORMAL:
			gtk_xtext_append_indent(GTK_XTEXT(tmp)->buffer,
						"---", 3, message,
						strlen(message));
			break;
		case CHAT_LOCAL_HIGH:
			gtk_xtext_append_indent(GTK_XTEXT(tmp)->buffer,
						"***", 3, message,
						strlen(message));
			break;
		}
		g_free(name);
		gtk_xtext_refresh(GTK_XTEXT(tmp), 0);
	}
}


/* chat_send() - Sends a chat message to the server, also parses commads
 *		 within the chat message.
 *
 * Recieves:
 *	gchar	*message	: The text to send to the server as a chat message
 *
 * Returns:
 */

void chat_send(gchar * message)
{
	if (strcmp(message, "")) {
		if (strncasecmp(message, "/msg", 4) == 0)
			chat_send_prvmsg(server, message);
		else if (strncasecmp(message, "/beep", 5) == 0)
			chat_send_beep(server, message);
		else if (strncasecmp(message, "/help", 5) == 0)
			chat_help();
		else if (strncasecmp(message, "/friends", 8) == 0)
			chat_list_friend();
		else if (strncasecmp(message, "/ignore", 7) == 0)
			chat_list_ignore();
		else if (strncasecmp(message, "/wall", 5) == 0)
			chat_send_wall(server, message);
		else
			chat_send_msg(server, message);
	}
}


/* chat_send_msg() - sends a chat message to the server as normal
 *
 * Recieves:
 *	GGZServer *server	: Currently connected server
 *	gchar *message		: The text to send as a normal message
 *
 * Returns:
 */

void chat_send_msg(GGZServer * server, gchar * message)
{
	GGZRoom *room = ggzcore_server_get_cur_room(server);

	/* Send the current text */
	ggzcore_room_chat(room, GGZ_CHAT_NORMAL, NULL, message);
}


/* chat_send_prvmsg() - Sends a chat to a user as private
 *
 * Recieves:
 *	GGZServer *server	: Currently connected server
 *	gchar *message		: The text to send as a private message
 *
 * Returns:
 */

void chat_send_prvmsg(GGZServer * server, gchar * message)
{
	GGZRoom *room = ggzcore_server_get_cur_room(server);
	gchar *line, *name;
	gint i;
	int success = FALSE;

	assert(strlen(message) >= 4);
	line = ggz_strdup(message + 5);

	name = g_strstrip(line);
	for (i = 0; i < strlen(name); i++) {
		if (name[i] == ' ') {
			gchar *msg = name + i + 1;
			name[i] = '\0';
			ggzcore_room_chat(room, GGZ_CHAT_PERSONAL, name,
					  msg);
			chat_display_message(CHAT_SEND_PRVMSG, name, msg);
			success = TRUE;
			break;
		}
	}

	if (!success) {
		chat_display_message(CHAT_LOCAL_NORMAL, NULL,
				     "Usage: /msg <username> <message>");
		chat_display_message(CHAT_LOCAL_NORMAL, NULL,
				     "    Sends a private message to a user on the network.");
	}

	ggz_free(line);
}



/* chat_send_wall() - Sends a message to all rooms
 *
 * Recieves:
 *	GGZServer *server	: Currently connected server
 *	gchar *message		: The text to send as a beep message
 *
 * Returns:
 */

void chat_send_wall(GGZServer * server, gchar * message)
{
	GGZRoom *room = ggzcore_server_get_cur_room(server);
	char *msg;

	assert(strlen(message) >= 4);
	msg = g_strstrip(ggz_strdup(message + 5));
	ggzcore_room_chat(room, GGZ_CHAT_ANNOUNCE, NULL, msg);
	ggz_free(msg);
}


/* chat_send_beep() - Sends a beep to a user
 *
 * Recieves:
 *	GGZServer *server	: Currently connected server
 *	gchar *message		: The text to send as a beep message
 *
 * Returns:
 */

void chat_send_beep(GGZServer * server, gchar * message)
{
	GGZRoom *room = ggzcore_server_get_cur_room(server);
	char *player;

	assert(strlen(message) >= 5);
	player = g_strstrip(ggz_strdup(message + 6));

	ggzcore_room_chat(room, GGZ_CHAT_BEEP, player, NULL);

	chat_display_message(CHAT_LOCAL_NORMAL, NULL, _("Beep Sent"));
	ggz_free(player);
}


/* chat_enter() - Displays a message in the chat area when someone
 *                enters the room
 *
 * Recieves:
 * 	gchar *player	: The players username
 *
 * Returns:
 */

void chat_enter(const gchar * player)
{
	GtkWidget *tmp = NULL;

	if (!ggzcore_conf_read_int("CHAT", "IGNORE", FALSE)) {
		gchar *name = g_strdup(player);

		tmp = lookup_widget(interface, "custChatXText");
		gtk_xtext_append_indent(GTK_XTEXT(tmp)->buffer, "-->", 3,
					name, strlen(name));

		g_free(name);
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

void chat_part(const gchar * player)
{
	GtkWidget *tmp = NULL;

	if (!ggzcore_conf_read_int("CHAT", "IGNORE", FALSE)) {
		gchar *name = g_strdup(player);

		tmp = lookup_widget(interface, "custChatXText");
		gtk_xtext_append_indent(GTK_XTEXT(tmp)->buffer, "<--", 3,
					name, strlen(name));

		g_free(name);
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
	chat_display_message(CHAT_LOCAL_NORMAL, NULL,
			     _
			     ("/me <action> .............. Send an action"));
	chat_display_message(CHAT_LOCAL_NORMAL, NULL,
			     _
			     ("/msg <username> <message> . Private message a player"));
	chat_display_message(CHAT_LOCAL_NORMAL, NULL,
			     _
			     ("/beep <username> .......... Beep a player"));
	chat_display_message(CHAT_LOCAL_NORMAL, NULL,
			     _
			     ("/friends .................. List your friends"));
	chat_display_message(CHAT_LOCAL_NORMAL, NULL,
			     _
			     ("/ignore ................... List people your ignoring"));
	chat_display_message(CHAT_LOCAL_NORMAL, NULL,
			     _
			     ("/wall <message> ........... Admin command"));
}



#define WORD_URL     1
#define WORD_GGZ     2
#define WORD_HOST    3
#define WORD_EMAIL   4

/* chat_checkurl() - Checks the current word to see if it is a valid URL
 *                   to underline.
 *
 * Recieves:
 * GtkXText	*xtext	: The xtext widget the word is from
 * char		word	: The current word to chek
 *
 * Returns:
 * 0		: Not a valid URL
 * WORD_*	: The URL Type
 */

int chat_checkurl(GtkXText * xtext, char *word)
{
	char *at, *dot;
	int i, dots;
	int len = strlen(word);

	/* Check for URLs */
	if (!strncasecmp(word, "ftp.", 4))
		return WORD_URL;
	if (!strncasecmp(word, "ftp://", 6))
		return WORD_URL;
	if (!strncasecmp(word, "www.", 4))
		return WORD_URL;
	if (!strncasecmp(word, "http://", 7))
		return WORD_URL;
	if (!strncasecmp(word, "https://", 8))
		return WORD_URL;
	if (!strncasecmp(word, "ggz.", 4))
		return WORD_GGZ;
	if (!strncasecmp(word, "ggz://", 6))
		return WORD_GGZ;

	/* Check for email addresses */
	at = strchr(word, '@');
	dot = strrchr(word, '.');
	if (at && dot) {
		if ((unsigned long)at < (unsigned long)dot) {

			if (strchr(word, '*'))
				return WORD_HOST;
			else
				return WORD_EMAIL;
		}
	}

	/* Check for IPs */
	dots = 0;
	for (i = 0; i < len; i++) {
		if (word[i] == '.')
			dots++;
	}
	if (dots == 3) {
		if (inet_addr(word) != -1)
			return WORD_HOST;
	}

	if (!strncasecmp(word + len - 5, ".html", 5))
		return WORD_HOST;
	if (!strncasecmp(word + len - 4, ".org", 4))
		return WORD_HOST;
	if (!strncasecmp(word + len - 4, ".net", 4))
		return WORD_HOST;
	if (!strncasecmp(word + len - 4, ".com", 4))
		return WORD_HOST;
	if (!strncasecmp(word + len - 4, ".edu", 4))
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

void chat_word_clicked(GtkXText * xtext, char *word,
		       GdkEventButton * event)
{
	switch (chat_checkurl(xtext, word)) {
	case WORD_GGZ:
//                      login_goto_server(word);
		break;
	case WORD_HOST:
	case WORD_URL:
//                      support_goto_url(word);
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
 * gchar		*color	: The color to use (a _static_ string)
 */

/* FIXME: Everything that calls this needs to free the memory */
const gchar *chat_get_color(gchar * name, gchar * msg)
{
	int i;
	int pos;
	char *srv_handle;
	char *p;
	int c;
	static gchar color[16];

	/* Is our name in the message? */
	srv_handle = ggzcore_server_get_handle(server);
	if (strlen(msg) > strlen(srv_handle) + 1) {
		for (pos = 0; pos < strlen(msg) - strlen(srv_handle) + 1;
		     pos++) {
			if (!strncasecmp
			    (msg + pos, srv_handle, strlen(srv_handle))) {
				c = ggzcore_conf_read_int("CHAT",
							  "H_COLOR", 3);
				snprintf(color, sizeof(color), "%02d", c);
				return color;
			}
		}
	}

	/* Is a friend talking? */
	for (i = 0; i < friend_count; i++) {
		p = g_array_index(chatinfo.friends, char *, i);
		if (!strcmp(p, name)) {
			c = ggzcore_conf_read_int("CHAT", "F_COLOR", 6);
			snprintf(color, sizeof(color), "%02d", c);
			return color;
		}
	}

	/* Normal color */
	c = ggzcore_conf_read_int("CHAT", "N_COLOR", 2);
	snprintf(color, sizeof(color), "%02d", c);
	return color;
}


/* chat_add_friend() - Adds a name to your friends list
 *
 * Recieves:
 * gchar	*name	: name to add
 * gint		display : Display the add message in xtext
 *
 * Returns:
 */

void chat_add_friend(gchar * name, gint display)
{
	gchar *out;
	char *name_copy;

	name_copy = ggz_strdup(name);
	g_array_append_val(chatinfo.friends, name_copy);
	friend_count++;
	if (display == TRUE) {
		out =
		    g_strdup_printf(_("Added %s to your friends list."),
				    name);
		chat_display_message(CHAT_LOCAL_NORMAL, NULL, out);
		g_free(out);
	}
}

/* chat_remove_friend() - Removes a name to your friends list
 *
 * Recieves:
 * gchar	*name	: name to add
 *
 * Returns:
 */

void chat_remove_friend(gchar * name)
{
	int i;
	char *p;
	char *out;

	for (i = 0; i < friend_count; i++) {
		p = g_array_index(chatinfo.friends, char *, i);
		if (!strcmp(p, name)) {
			g_array_remove_index_fast(chatinfo.friends, i);
			out =
			    g_strdup_printf(_
					    ("Removed %s from your friends list."),
					    name);
			chat_display_message(CHAT_LOCAL_NORMAL, NULL, out);
			g_free(out);
			friend_count--;
			ggz_free(p);
			return;
		}
	}
}



/* chat_add_ignore() - Adds a name to your ignore list
 *
 * Recieves:
 * gchar	*name	: name to add
 * gint		display : Display the add message in xtext
 *
 * Returns:
 */

void chat_add_ignore(gchar * name, gint display)
{
	gchar *out;
	char *name_copy;

	name_copy = ggz_strdup(name);
	g_array_append_val(chatinfo.ignore, name_copy);
	ignore_count++;
	if (display == TRUE) {
		out =
		    g_strdup_printf(_("Added %s to your ignore list."),
				    name);
		chat_display_message(CHAT_LOCAL_NORMAL, NULL, out);
		g_free(out);
	}
}


/* chat_remove_ignore() - Removes a name to your ignore list
 *
 * Recieves:
 * gchar	*name	: name to add
 *
 * Returns:
 */

void chat_remove_ignore(gchar * name)
{
	int i;
	char *p;
	char *out;

	for (i = 0; i < ignore_count; i++) {
		p = g_array_index(chatinfo.ignore, char *, i);
		if (!strcmp(p, name)) {
			g_array_remove_index_fast(chatinfo.ignore, i);
			out =
			    g_strdup_printf(_
					    ("Removed %s from your ignore list."),
					    name);
			chat_display_message(CHAT_LOCAL_NORMAL, NULL, out);
			g_free(out);
			ignore_count--;
			ggz_free(p);
			return;
		}
	}
}


/* chat_save_lists() - saves the friends and ignore list to the rc file
 *
 * Recieves:
 *
 * Returns:
 */

void chat_save_lists(void)
{
	int i;
	char *p;
	char c_num[16];

	for (i = 0; i < ignore_count; i++) {
		snprintf(c_num, sizeof(c_num), "%d", i + 1);
		p = g_array_index(chatinfo.ignore, char *, i);
		ggzcore_conf_write_string("IGNORE", c_num, p);
	}
	ggzcore_conf_write_int("IGNORE", "TOTAL", ignore_count);

	for (i = 0; i < friend_count; i++) {
		snprintf(c_num, sizeof(c_num), "%d", i + 1);
		p = g_array_index(chatinfo.friends, char *, i);
		ggzcore_conf_write_string("FRIENDS", c_num, p);
	}
	ggzcore_conf_write_int("FRIENDS", "TOTAL", friend_count);

	ggzcore_conf_commit();
}


/* chat_load_lists() - loads the friends and ignore list from the rc file
 *
 * Recieves:
 *
 * Returns:
 */

void chat_load_lists(void)
{
	int i, count;
	char num[16], *p;

	count = ggzcore_conf_read_int("IGNORE", "TOTAL", 0);
	for (i = 0; i < count; i++) {
		snprintf(num, sizeof(num), "%d", i + 1);
		p = ggzcore_conf_read_string("IGNORE", num, "Unknown");
		chat_add_ignore(p, FALSE);
		ggz_free(p);
	}

	count = ggzcore_conf_read_int("FRIENDS", "TOTAL", 0);
	for (i = 0; i < count; i++) {
		snprintf(num, sizeof(num), "%d", i + 1);
		p = ggzcore_conf_read_string("FRIENDS", num, "Unknown");
		chat_add_friend(p, FALSE);
		ggz_free(p);
	}
}


void chat_list_friend(void)
{
	int i;

	chat_display_message(CHAT_LOCAL_NORMAL, NULL,
			     "People currently your friends");
	chat_display_message(CHAT_LOCAL_NORMAL, NULL,
			     "-----------------------------");
	for (i = 0; i < friend_count; i++)
		chat_display_message(CHAT_LOCAL_NORMAL, NULL,
				     g_array_index(chatinfo.friends,
						   char *, i));
}


void chat_list_ignore(void)
{
	int i;

	chat_display_message(CHAT_LOCAL_NORMAL, NULL,
			     "People your currently ignoring");
	chat_display_message(CHAT_LOCAL_NORMAL, NULL,
			     "------------------------------");
	for (i = 0; i < ignore_count; i++)
		chat_display_message(CHAT_LOCAL_NORMAL, NULL,
				     g_array_index(chatinfo.ignore, char *,
						   i));
}


/* chat_complete_name() - Given a partial name we try to compleate it
 *
 * Recieves:
 * 	gchar	*name	: partial name
 *
 * Returns:
 * 	gchar*		: NULL if not matched, or matched name
 */

const gchar *chat_complete_name(gchar * name)
{
	GtkWidget *tmp;
	GdkPixmap *pixmap;
	GdkBitmap *mask;
	gchar *fullname = NULL;
	gchar *returnname = NULL;
	gint currow;
	gint multiple = FALSE;
	gint first = FALSE;
	guint8 space;

	tmp = lookup_widget(interface, "ctPlayers");
	for (currow = 0; currow < GTK_CLIST(tmp)->rows; currow++) {
		gtk_clist_get_pixtext(GTK_CLIST(tmp), currow, 2, &fullname,
				      &space, &pixmap, &mask);

		if (strncasecmp(fullname, name, strlen(name)) == 0) {
			if (multiple == FALSE) {
				returnname = fullname;
				multiple = TRUE;
			} else {
				if (first == FALSE) {
					chat_display_message
					    (CHAT_LOCAL_NORMAL, NULL,
					     _("Multiple matches:"));
					chat_display_message
					    (CHAT_LOCAL_NORMAL, NULL,
					     returnname);
					first = TRUE;
				}
				returnname = fullname;
				chat_display_message(CHAT_LOCAL_NORMAL,
						     NULL, returnname);
				returnname = NULL;
			}
		}
	}

	if (returnname != NULL)
		return returnname;
	return NULL;
}

gint chat_is_friend(gchar * name)
{
	int i;

	for (i = 0; i < friend_count; i++)
		if (!strcmp
		    (g_array_index(chatinfo.friends, char *, i), name))
			 return TRUE;

	return FALSE;
}

gint chat_is_ignore(gchar * name)
{
	int i;

	for (i = 0; i < ignore_count; i++)
		if (!strcmp
		    (g_array_index(chatinfo.ignore, char *, i), name))
			 return TRUE;

	return FALSE;
}


void chat_lists_cleanup(void)
{
	int i;

	for (i = 0; i < ignore_count; i++)
		ggz_free(g_array_index(chatinfo.ignore, char *, i));

	for (i = 0; i < friend_count; i++)
		ggz_free(g_array_index(chatinfo.friends, char *, i));
}
