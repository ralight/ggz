/*
 * File: chat.c
 * Author: Justin Zaun
 * Project: GGZ GTK Client
 * $Id: chat.c 10250 2008-07-09 18:44:38Z jdorje $
 *
 * This file contains all functions that are chat related.
 *
 * Copyright (C) 2000-2005 GGZ Development Team.
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
#  include <config.h>			/* Site-specific config */
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> /* For strcasecmp */
#include <gtk/gtk.h>
#include <sys/types.h>
#ifdef HAVE_SYS_SOCKET_H
#  include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#  include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#  include <arpa/inet.h>
#endif
#ifdef HAVE_WINSOCK_H
#  include <winsock.h>
#endif

/* Supposedly not present on some systems (solaris). */
#ifndef INADDR_NONE
#  define INADDR_NONE ((unsigned long) -1)
#endif

#include <glib.h>

#include "chat.h"
#include "client.h"
#include "ggz.h"
#include "ggzcore.h"
#include "ggzutils.h"
#include "login.h"
#include "server.h"
#include "xtext.h"
#define gettext_noop			/* FIXME: why is this necessary? */
#include "support.h"

static const gchar *chat_get_color(const gchar *name, const gchar *msg);
static void chat_load_lists(void);

static int friend_count;
static int ignore_count;

static void chat_send_msg(GGZServer *server, const gchar *message);

static void chat_send_prvmsg(GGZServer *server, const gchar *message);
static void chat_send_tablemsg(GGZServer *server, const gchar *message);
static void chat_send_wall(GGZServer *server, const gchar *message);
static void chat_send_beep(GGZServer *server, const gchar *message);
static void chat_help(GGZServer *server, const gchar *message);
static void chat_list_friend(GGZServer *server, const gchar *message);
static void chat_list_ignore(GGZServer *server, const gchar *message);

static void chat_kick(GGZServer *server, const gchar *message);
static void chat_gag(GGZServer *server, const gchar *message);
static void chat_ungag(GGZServer *server, const gchar *message);
static void chat_ban(GGZServer *server, const gchar *message);

/* Chat command emumeration and structure. These must match up. */
typedef enum {
	CCMD_MSG,
	CCMD_TABLE,
	CCMD_WALL,
	CCMD_BEEP,
	CCMD_HELP,
	CCMD_FRIENDS,
	CCMD_IGNORE,
	CCMD_KICK,
	CCMD_GAG,
	CCMD_UNGAG,
	CCMD_BAN
} CCMDType;

typedef struct {
	const char *cmd;
	void (*func)(GGZServer *server, const gchar *message);
	const char *params;
	const char *desc;
} GGZCommand;

static GGZCommand commands[] = {
	{ "/msg", chat_send_prvmsg,
	  N_("<username> <message>"),
	  N_("Send a player a private message") },
	{ "/table", chat_send_tablemsg,
	  N_("<message>"),
	  N_("Send a message to your table") },
	{ "/wall", chat_send_wall,
	  N_("<message>"),
	  N_("Send a message to all rooms") },
	{ "/beep",
	  chat_send_beep,
	  N_("<username>"),
	  N_("Beep a player") },
	{ "/help", chat_help,
	  NULL,
	  N_("Get help") },
	{ "/friends", chat_list_friend,
	  NULL,
	  N_("List your friends") },
	{ "/ignore", chat_list_ignore,
	  NULL,
	  N_("List people you're ignoring") },
	{ "/kick", chat_kick,
	  N_("<username>"),
	  N_("Kick a player from the room") },
	{ "/gag", chat_gag,
	  N_("<username>"),
	  N_("Gag a player to prevent them from talking") },
	{ "/ungag", chat_ungag,
	  N_("<username>"),
	  N_("Reverse the gag operation to allow a player to talk") },
	{ "/ban", chat_ban,
	  N_("<username>"),
	  N_("Ban a player from the server") }
};
#define NUM_CHAT_COMMANDS (sizeof(commands) / sizeof(commands[0]))

/* This one is hard-coded at the server end. */
static GGZCommand me_command = {
	"/me", NULL, N_("<action>"), N_("Send an action")
};

/* chatinfo holds 2 arrays one for friends and one for 
 * people to ignore. Friends are shown in a different 
 * color in chat and ingnored people's chats don't show
 * up at all.
 */
static struct chatinfo {
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
	chatinfo.friends = g_array_new(FALSE, FALSE, sizeof(gchar*));
	chatinfo.ignore = g_array_new(FALSE, FALSE, sizeof(gchar*));

	/* sets up background color for chat area*/
	if (ggzcore_conf_read_int("CHAT", "BACKGROUND", TRUE)) {
		set_color(18, color_black());
		set_color(19, color_white());
	}

	chat_load_lists();
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

void chat_display_server(GGZChatType type,
			  const char *sender, const char *message)
{
        GtkXText *tmp;
	gchar *name;

	/* are we ignoring this person? */
	if (sender && chat_is_ignore(sender))
		return;

        tmp = g_object_get_data(G_OBJECT(ggz_gtk.win_main), "xtext_custom");

	switch (type) {
	case GGZ_CHAT_UNKNOWN:
		/* We should handle the unknown case somehow */
		if (!message && !sender) {
			break;
		} else if (!message) {
			gchar *m;
			name = g_strdup_printf("?\003%s%s\003?",
				       chat_get_color(sender, ""),
				       sender);
			m = g_strdup_printf(_("You have received an "
					      "unknown message from %s."),
					    sender);
			gtk_xtext_append_indent(tmp->buffer,
						name, strlen(name),
						m, strlen(m));
			g_free(m);
			g_free(name);
			break;
		}
		/* else fall through */
	case GGZ_CHAT_NORMAL:
	case GGZ_CHAT_TABLE:
		if (!strncasecmp(message, "/me ", 4)) {
			name = g_strdup_printf("%s %s", sender, message+4);
			gtk_xtext_append_indent(tmp->buffer,
						"*", 1, name, strlen(name));
			g_free(name);
		} else {
			char bracketl, bracketr;
			if (type == GGZ_CHAT_NORMAL) {
				bracketl = '<';
				bracketr = '>';
			} else
				bracketl = bracketr = '|';

			name = g_strdup_printf("%c\003%s%s\003%c",
					       bracketl,
					       chat_get_color(sender, message),
					       sender,
					       bracketr);
			gtk_xtext_append_indent(tmp->buffer,
						name, strlen(name),
						(char*)message,
						strlen(message));
			g_free(name);
		}
		if (ggzcore_conf_read_int("CHAT", "RSYNTH", FALSE)) {
			char *command;
			command = g_strdup_printf("esddsp say -f 8 \"%s\"",
						  message);
			support_exec(command);
			g_free(command);
		}
		break;
	case GGZ_CHAT_PERSONAL:
		name = g_strdup_printf(">\003%s%s\003<",
				       chat_get_color(sender, message),
				       sender);
		gtk_xtext_append_indent(tmp->buffer,
					name, strlen(name),
					(char*)message, strlen(message));
		g_free(name);
		break;
	case GGZ_CHAT_ANNOUNCE:
		name = g_strdup_printf("[\003%s%s\003]",
				       chat_get_color(sender, message),
				       sender);
		gtk_xtext_append_indent(tmp->buffer,
					name, strlen(name),
					(char*)message, strlen(message));
		g_free(name);
		break;
	case GGZ_CHAT_BEEP:
		name = g_strdup_printf(_("You've been beeped by %s."), sender);
		chat_display_local(CHAT_LOCAL_NORMAL, NULL, name);
		if (ggzcore_conf_read_int("CHAT", "SOUND", TRUE))
			gdk_beep();
		g_free(name);
		break;
	}

	gtk_xtext_refresh(tmp, 0);
}

void chat_display_local(LocalChatType type,
			const char *target, const char *message)
{
        GtkXText *tmp;
	gchar *name;

        tmp = g_object_get_data(G_OBJECT(ggz_gtk.win_main), "xtext_custom");

	switch (type) {
	case CHAT_SEND_PERSONAL:
		name = g_strdup_printf("--> %s", target);
		gtk_xtext_append_indent(tmp->buffer,
					name, strlen(name),
					(char*)message, strlen(message));
		g_free(name);
		break;
	case CHAT_LOCAL_NORMAL:
		gtk_xtext_append_indent(tmp->buffer,
					"---", 3,
					(char*)message, strlen(message));
		break;
	case CHAT_LOCAL_HIGH:
		gtk_xtext_append_indent(tmp->buffer,
					"***", 3,
					(char*)message, strlen(message));
		break;

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

void chat_send(const gchar *message)
{
	unsigned int i;

	if (strcmp(message, "") == 0)
		return;

	for (i = 0; i < NUM_CHAT_COMMANDS; i++) {
		int len = strlen(commands[i].cmd);
		if (strncasecmp(message, commands[i].cmd, len) == 0) {
			(commands[i].func)(ggz_gtk.server, message + len);
			return;
		}
	}

	chat_send_msg(ggz_gtk.server, message);
}


/* chat_send_msg() - sends a chat message to the server as normal
 *
 * Recieves:
 *	GGZServer *server	: Currently connected server
 *	gchar *message		: The text to send as a normal message
 *
 * Returns:
 */
static void chat_send_msg(GGZServer *server, const gchar *message)
{
	GGZRoom *room = ggzcore_server_get_cur_room(server);

	/* Send the current text */
	ggzcore_room_chat(room, GGZ_CHAT_NORMAL, NULL, message);
}


/* chat_display_usage() - displays usage for a particular command
 *
 * Receives:
 *     ccmd_type - the command to display usage for
 */
static void chat_display_usage(CCMDType ccmd_type)
{
	char *text;

	text = g_strdup_printf("%s %s %s",
			       _("Usage:"),
			       commands[ccmd_type].cmd,
			       (commands[ccmd_type].params ? 
				_(commands[ccmd_type].params)
				: ""));

	chat_display_local(CHAT_LOCAL_NORMAL, NULL, text);
	g_free(text);

	text = g_strdup_printf("    %s", _(commands[ccmd_type].desc));
	chat_display_local(CHAT_LOCAL_NORMAL, NULL, text);
	g_free(text);
}


/* chat_send_prvmsg() - Sends a chat to a user as private
 *
 * Recieves:
 *	GGZServer *server	: Currently connected server
 *	gchar *message		: The text to send as a private message
 *
 * Returns:
 */
static void chat_send_prvmsg(GGZServer *server, const gchar *message)
{
	GGZRoom *room = ggzcore_server_get_cur_room(server);
	gchar *line, *name;
	guint i;

	line = ggz_strdup(message);
	
	name = g_strstrip(line);
	for(i = 0; i < strlen(name); i++)
	{
		if(name[i] == ' ')
		{
			gchar *msg = name + i + 1;
			name[i] = '\0';
			ggzcore_room_chat(room, GGZ_CHAT_PERSONAL, name, msg);
			chat_display_local(CHAT_SEND_PERSONAL, name, msg);
			ggz_free(line);
			return;
		}
	}

	ggz_free(line);

	/* Could not parse it. */
	chat_display_usage(CCMD_MSG);
}

/* chat_send_prvmsg() - Sends a chat to a user as private
 *
 * Recieves:
 *	GGZServer *server	: Currently connected server
 *	gchar *message		: The text to send as a private message
 *
 * Returns:
 */
static void chat_send_tablemsg(GGZServer *server, const gchar *message)
{
	GGZRoom *room = ggzcore_server_get_cur_room(server);
	char *msg = g_strstrip(ggz_strdup(message));
	ggzcore_room_chat(room, GGZ_CHAT_TABLE, NULL, msg);
	ggz_free(msg);
}


/* chat_send_wall() - Sends a message to all rooms
 *
 * Recieves:
 *	GGZServer *server	: Currently connected server
 *	gchar *message		: The text to send as a beep message
 *
 * Returns:
 */
static void chat_send_wall(GGZServer *server, const gchar *message)
{
	GGZRoom *room = ggzcore_server_get_cur_room(server);
	char *msg = g_strstrip(ggz_strdup(message));
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
static void chat_send_beep(GGZServer *server, const gchar *message)
{
	GGZRoom *room = ggzcore_server_get_cur_room(server);
	char *player = g_strstrip(ggz_strdup(message));
	char *alert;

	ggzcore_room_chat(room, GGZ_CHAT_BEEP, player, NULL);

	alert = g_strdup_printf(_("Beep sent to %s."), player);
	chat_display_local(CHAT_LOCAL_NORMAL, NULL, alert);
	g_free(alert);
	g_free(player);
}


/* chat_enter() - Displays a message in the chat area when someone
 *                enters the room
 *
 * Recieves:
 * 	gchar *player	: The players username
 *      int from_room   : the room the player is coming from (or -1/-2)
 *
 * Returns:
 */

void chat_enter(const gchar *player, gboolean room_known, GGZRoom *from_room)
{
        GtkXText *tmp = NULL;

        if (!ggzcore_conf_read_int("CHAT", "IGNORE", FALSE)) {
		char *message;

		if (!room_known) {
			/* Just entering server. */
			message = g_strdup_printf(_("%s (logged on)"), player);
		} else if (from_room) {
			message = g_strdup_printf(_("%s (from %s)"), player,
					ggzcore_room_get_name(from_room));
		} else {
			/* Older servers don't include this data. */
			message = g_strdup(player);
		}

	        tmp = g_object_get_data(G_OBJECT(ggz_gtk.win_main), "xtext_custom");
		gtk_xtext_append_indent(tmp->buffer,
					"-->", 3, message, strlen(message));
		g_free(message);
	}
}


/* chat_part() - Displays a message in the chat area when someone
 *               leaves the room
 *
 * Recieves:
 * gchar	*player	: The players username
 * int          to_room : The room the player is going to (or -1/-2)
 *
 * Returns:
 */

void chat_part(const gchar *player, int room_known, GGZRoom *to_room)
{
        GtkXText *tmp = NULL;

        if (!ggzcore_conf_read_int("CHAT", "IGNORE", FALSE)) {
		char *message;

		if (!room_known) {
			/* Leaving server. */
			message = g_strdup_printf(_("%s (logged off)"), player);
		} else if (to_room) {
			message = g_strdup_printf(_("%s (to %s)"), player,
					ggzcore_room_get_name(to_room));
		} else {
			/* Older servers don't include this data. */
			message = g_strdup(player);
		}

	        tmp = g_object_get_data(G_OBJECT(ggz_gtk.win_main), "xtext_custom");
		gtk_xtext_append_indent(tmp->buffer,
					"<--", 3, message, strlen(message));
		g_free(message);
	}
}

static void chat_display_header(const char *text)
{
	char footer[strlen(text) + 1];
	int i;

	/* FIXME: this function and most that call it do text-based
	   alignment assuming a fixed-width font.  But there is nothing
	   to ensure that the xtext is using a fixed-width font (in fact,
	   it's not). */

	for (i = 0; i < sizeof(footer); i++) {
		footer[i] = '-';
	}
	footer[i] = '\0';

	chat_display_local(CHAT_LOCAL_NORMAL, NULL, text);
	chat_display_local(CHAT_LOCAL_NORMAL, NULL, footer);
}

/* chat_help() - Displays help on all the chat commands
 *
 * Recieves:
 *
 * Returns:
 */
static void chat_help(GGZServer *server, const gchar *message)
{
	unsigned int i;

	/* FIXME: this function does text-based alignment assuming a
	   fixed-width font.  But there is nothing to ensure that the
	   xtext is using a fixed-width font (in fact, it's not). */

	/* FIXME2: this is mildly UTF-8 unclean since it prints text
	   into a fixed-width buffer. */

	chat_display_header(_("Chat Commands"));

	for (i = 0; i < NUM_CHAT_COMMANDS + 1; i++) {
		char text[4096];
		GGZCommand *cmd;

		if (i == 0) {
			cmd = &me_command;
		} else {
			cmd = commands + i - 1;
		}

		if (cmd->params) {
			snprintf(text, sizeof(text),
				 "%s %s ",
				 cmd->cmd, _(cmd->params));
		} else {
			snprintf(text, sizeof(text),
				 "%s ", cmd->cmd);
		}
		while (strlen(text) < 27) {
			sprintf(text + strlen(text), ".");
		}
		snprintf(text + strlen(text), sizeof(text) - strlen(text),
			 " %s", _(cmd->desc));
		chat_display_local(CHAT_LOCAL_NORMAL, NULL, text);
	}
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

int chat_checkurl(GtkWidget *xtext, char *word, int fake_length)
{
	char *at, *dot;
	int i, dots;
	int len = strlen(word);

	if(!word) return 0;

	/* Check for URLs */
	if (!strncasecmp (word, "ftp.", 4))
		return WORD_URL;
	if (!strncasecmp (word, "ftp://", 6))
		return WORD_URL;
	if (!strncasecmp (word, "www.", 4))
		return WORD_URL;
	if (!strncasecmp (word, "http://", 7))
		return WORD_URL;
	if (!strncasecmp (word, "https://", 8))
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
		if (inet_addr(word) != INADDR_NONE)
			return WORD_HOST;
	}

	if (len >= 5 && !strncasecmp (word + len - 5, ".html", 5))
		return WORD_HOST;
	if (len >= 4 && !strncasecmp (word + len - 4, ".org", 4))
		return WORD_HOST;
	if (len >= 4 && !strncasecmp (word + len - 4, ".net", 4))
		return WORD_HOST;
	if (len >= 4 && !strncasecmp (word + len - 4, ".com", 4))
		return WORD_HOST;
	if (len >= 4 && !strncasecmp (word + len - 4, ".edu", 4))
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
  	switch (chat_checkurl(GTK_WIDGET(xtext), word, strlen(word))) {
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
 * gchar		*color	: The color to use (a _static_ string)
 */
static const gchar *chat_get_color(const gchar *name, const gchar *msg)
{
	unsigned int pos;
	const char *srv_handle;
	int c;
	static gchar color[16];

	/* Is our name in the message? */
	srv_handle = ggzcore_server_get_handle(ggz_gtk.server);
	if(strlen(msg) > strlen(srv_handle)+1)
	{
		for(pos=0; pos<strlen(msg)-strlen(srv_handle)+1; pos++)
		{
			if(!strncasecmp(msg+pos, srv_handle,strlen(srv_handle)))
			{
				c = ggzcore_conf_read_int("CHAT", "H_COLOR", 3);
				snprintf(color, sizeof(color), "%02d", c);
				return color;
			}
		}
	}

	/* Is a friend talking? */
	if (chat_is_friend(name)) {
		c = ggzcore_conf_read_int("CHAT", "F_COLOR", 6);
		snprintf(color, sizeof(color), "%02d", c);
		return color;
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

void chat_add_friend(const gchar *name, gint display)
{
	gchar *out;
	char *name_copy;

	name_copy = ggz_strdup(name);
	g_array_append_val(chatinfo.friends, name_copy);
	friend_count++;
	if (display)
	{
		out = g_strdup_printf(_("Added %s to your friends list."), name);
		chat_display_local(CHAT_LOCAL_NORMAL, NULL, out);
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

void chat_remove_friend(const gchar *name)
{
	int i;
	char *p;
	char *out;

	for(i=0; i<friend_count; i++) {
		p = g_array_index(chatinfo.friends, char *, i);
		if(!strcasecmp(p, name)) {
			g_array_remove_index_fast(chatinfo.friends, i);
			out = g_strdup_printf(_("Removed %s from your friends list."), name);
			chat_display_local(CHAT_LOCAL_NORMAL, NULL, out);
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

void chat_add_ignore(const gchar *name, gint display)
{
	gchar *out;
	char *name_copy;

	name_copy = ggz_strdup(name);
	g_array_append_val(chatinfo.ignore, name_copy);
	ignore_count++;
	if(display)
	{
		out = g_strdup_printf(_("Added %s to your ignore list."), name);
		chat_display_local(CHAT_LOCAL_NORMAL, NULL, out);
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

void chat_remove_ignore(const gchar *name)
{
	int i;
	char *p;
	char *out;

	for(i=0; i<ignore_count; i++) {
		p = g_array_index(chatinfo.ignore, char *, i);
		if(!strcasecmp(p, name)) {
			g_array_remove_index_fast(chatinfo.ignore, i);
			out = g_strdup_printf(_("Removed %s from your ignore list."), name);
			chat_display_local(CHAT_LOCAL_NORMAL, NULL, out);
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

	for(i=0; i<ignore_count; i++) {
		snprintf(c_num, sizeof(c_num), "%d", i+1);
		p = g_array_index(chatinfo.ignore, char *, i);
		ggzcore_conf_write_string("IGNORE", c_num, p);
	}
	ggzcore_conf_write_int("IGNORE", "TOTAL", ignore_count);

	for(i=0; i<friend_count; i++) {
		snprintf(c_num, sizeof(c_num), "%d", i+1);
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
static void chat_load_lists(void)
{
	int i, count;
	char num[16], *p;

	count = ggzcore_conf_read_int("IGNORE", "TOTAL", 0);
	for(i=0; i<count; i++) {
		snprintf(num, sizeof(num), "%d", i+1);
		p = ggzcore_conf_read_string("IGNORE", num, "Unknown");
		chat_add_ignore(p, FALSE);
		ggz_free(p);
	}

	count = ggzcore_conf_read_int("FRIENDS", "TOTAL", 0);
	for(i=0; i<count; i++) {
		snprintf(num, sizeof(num), "%d", i+1);
		p = ggzcore_conf_read_string("FRIENDS", num, "Unknown");
		chat_add_friend(p, FALSE);
		ggz_free(p);
	}
}


static void chat_list_friend(GGZServer *server, const gchar *message)
{
	int i;

	chat_display_header(_("People currently your friends"));
	for(i=0; i<friend_count; i++)
		chat_display_local(CHAT_LOCAL_NORMAL, NULL,
				   g_array_index(chatinfo.friends, char *, i));
}


static void chat_list_ignore(GGZServer *server, const gchar *message)
{
	int i;

	chat_display_header(_("People you're currently ignoring"));
	for(i=0; i<ignore_count; i++)
		chat_display_local(CHAT_LOCAL_NORMAL, NULL,
				   g_array_index(chatinfo.ignore, char *, i));
}

static void chat_kick(GGZServer *server, const gchar *message)
{
	char *player = g_strstrip(ggz_strdup(message));

	ggzcore_room_admin(ggzcore_server_get_cur_room(server),
			   GGZ_ADMIN_KICK,
			   player, NULL);
}


static void chat_gag(GGZServer *server, const gchar *message)
{
	char *player = g_strstrip(ggz_strdup(message));

	ggzcore_room_admin(ggzcore_server_get_cur_room(server),
			   GGZ_ADMIN_GAG,
			   player, NULL);
}


static void chat_ungag(GGZServer *server, const gchar *message)
{
	char *player = g_strstrip(ggz_strdup(message));

	ggzcore_room_admin(ggzcore_server_get_cur_room(server),
			   GGZ_ADMIN_UNGAG,
			   player, NULL);
}


static void chat_ban(GGZServer *server, const gchar *message)
{
	char *player = g_strstrip(ggz_strdup(message));

	ggzcore_room_admin(ggzcore_server_get_cur_room(server),
			   GGZ_ADMIN_BAN,
			   player, NULL);
}


/* chat_complete_name() - Given a partial name we try to compleate it
 *
 * Recieves:
 * 	gchar	*name	: partial name
 *
 * Returns:
 * 	gchar*		: matched name (allocated with ggz_malloc), or NULL
 */

gchar *chat_complete_name(const gchar *name, int *perfect)
{
	gchar *returnname = NULL;
	int matches = 0;

	GGZRoom *room = ggzcore_server_get_cur_room(ggz_gtk.server);
	int num = ggzcore_room_get_num_players(room);
	int i, j;

	for (i = 0; i < num; i++) {
		GGZPlayer *player = ggzcore_room_get_nth_player(room, i);
		char* fullname = ggzcore_player_get_name(player);

		if (strncasecmp(fullname, name, strlen(name)) == 0) {
			if (matches == 0) {
				returnname = ggz_strdup(fullname);
			} else {
				if (matches == 1) {
					chat_display_local(CHAT_LOCAL_NORMAL,
						NULL, _("Multiple matches:"));
					chat_display_local(CHAT_LOCAL_NORMAL,
						NULL, returnname);
				}
				chat_display_local(CHAT_LOCAL_NORMAL,
					NULL, fullname);

				/* Terminate returnname after the last
				   matching character. */
				for (j = 0; fullname[j]; j++)
					if (fullname[j] != returnname[j])
						break;
				returnname[j] = '\0';
			}
			matches++;
		}
	}

	*perfect = (matches == 1);
	return returnname;
}

gint chat_is_friend(const gchar *name)
{
	int i;

	for(i=0; i<friend_count; i++)
		if(!strcasecmp(g_array_index(chatinfo.friends, char *, i),
			       name))
			return TRUE;

	return FALSE;
}

gint chat_is_ignore(const gchar *name)
{
	int i;

	for(i=0; i<ignore_count; i++)
		if(!strcasecmp(g_array_index(chatinfo.ignore, char *, i),
			       name))
			return TRUE;

	return FALSE;
}


void chat_lists_cleanup(void)
{
	int i;

	for(i=0; i<ignore_count; i++)
		ggz_free(g_array_index(chatinfo.ignore, char *, i));

	for(i=0; i<friend_count; i++)
		ggz_free(g_array_index(chatinfo.friends, char *, i));
}
