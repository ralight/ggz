/*
 * File: output.c
 * Author: Brent Hendricks
 * Project: GGZ Text Client 
 * Date: 9/26/00
 * $Id: output.c 7010 2005-03-18 10:20:41Z josef $
 *
 * Functions for display text/messages
 *
 * Copyright (C) 2000-2005 GGZ Development Team
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
#  include <config.h>
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_SYS_IOCTL_H
#  include <sys/ioctl.h>
#endif
#include <time.h>

#include <ggzcore.h>

#include "output.h"
#include "server.h"
#include "state.h"
#include "support.h"


/* Color Codes */
#define COLOR_BRIGHT_GREY	"\e[1m\e[30m"
#define COLOR_BRIGHT_RED	"\e[1m\e[31m"
#define COLOR_BRIGHT_GREEN	"\e[1m\e[32m"
#define COLOR_BRIGHT_ORANGE	"\e[1m\e[33m"
#define COLOR_BRIGHT_BLUE	"\e[1m\e[34m"
#define COLOR_BRIGHT_PINK	"\e[1m\e[35m"
#define COLOR_BRIGHT_PURPLE	"\e[1m\e[36m"
#define COLOR_BRIGHT_WHITE	"\e[1m\e[37m"
#define COLOR_GREY		"\e[0m\e[30m"
#define COLOR_RED		"\e[0m\e[31m"
#define COLOR_GREEN		"\e[0m\e[32m"
#define COLOR_ORANGE		"\e[0m\e[33m"
#define COLOR_BLUE		"\e[0m\e[34m"
#define COLOR_PINK		"\e[0m\e[35m"
#define COLOR_PURPLE		"\e[0m\e[36m"
#define COLOR_WHITE		"\e[0m\e[37m"

/* Escape sequences */
#define SEQUENCE_BEEP    "\007" /* Output a beep signal */
#define SEQUENCE_SAVE    "\e7"  /* Save cursor */
#define SEQUENCE_RESTORE "\e8"  /* Restore cursor */

/* Highest line number in the message buffer */
#define MAX_LINES 100

static struct winsize window;
static int tty_des;
static char **chat;
static int chat_offset = 0;
static int reverse_display = 0;
static int output_enabled = 1;

/* Private functions */
static void output_table_info(GGZTable *table);


void output_draw_text(void);		/* Draws the chat que to screen		*/
void output_label(char *label);		/* Display the color label in the	*/
					/* Status line.				*/
void output_goto(int row, int col);	/* Goto's <r>,<c> on the screen this is	*/
					/* a dangerious function if not	used	*/
					/* properly!				*/
					/* Make sure to save your current	*/
					/* position and restore it when done, 	*/
					/* unless ABSOLUTLY sure!		*/

void output_enable(int enabled)
{
	output_enabled = enabled;
}

void output_display_help(void)
{
	if(!output_enabled) return;

	output_text(_("--- GGZ Gaming Zone -- Help"));
	output_text(_("--- -----------------------"));
	output_text(_("---"));
	output_text(_("--- /connect [<server>[:<port>]] [<nick>] [<password>]"));
	output_text(_("---    Connect to a GGZ server"));
	output_text(_("--- /disconnect                             Disconnect from server"));
	output_text(_("--- /exit                                   Quit GGZ Gaming Zone"));
	output_text(_("--- /desc <room>                            Get description of room <room>"));
	output_text(_("--- /join room|table <num>                  Join room or table <num>"));
	output_text(_("--- /list players|rooms|tables|types        List the requested information."));
	output_text(_("--- /msg <player> <msg>                     Send a msg to a player"));
	output_text(_("--- /table <msg>                            Send a msg to your table"));
	output_text(_("--- /beep <player>                          Beep player <player>"));
	output_text(_("--- /launch                                 Launch a game if possible"));
	output_text(_("--- /version                                Display the client version"));
	output_text(_("--- /who                                    List current player in the room"));
	output_text(_("--- /wall <msg>                             Admin only command, broadcast to all rooms"));
}

void output_banner(void)
{
	if(!output_enabled) return;

	output_text(_("Welcome to the text-only GGZ client!"));
	output_text(_("--Written by Brent Hendricks & Justin Zaun (C) 2000"));
	output_text(_("--Maintained by the GGZ Gaming Zone developers (C) 2001 - 2005"));
	output_text(_("--Type /help to see a list of possible commands."));
}

void output_prompt(void)
{
	if(!output_enabled) return;

	output_goto(window.ws_row, 0);
	printf("\e[2K");
	output_goto(window.ws_row - 1, 0);

#ifndef HAVE_READLINE_READLINE_H
	printf("\e[2K%sGGZ%s>%s ",
		COLOR_BRIGHT_WHITE, COLOR_GREY,
		COLOR_WHITE);
#else
	printf("\e[2K");
#endif
}

void output_text(char* fmt, ...)
{
	char message[1024];	/* FIXME: Make me dynamic */
	int x;
	char *token, *messagedup, *orig;

	if(!output_enabled) return;

	va_list ap;
	va_start(ap, fmt);
	vsnprintf(message, sizeof(message), fmt, ap);
	va_end(ap);

	messagedup = ggz_strdup(message);
	orig = messagedup;

	token = strsep(&messagedup, "\n");
	while(token) {
		/* Shift everything in the buffer up */
		if (chat[MAX_LINES])
			ggz_free(chat[MAX_LINES]);
		for (x = MAX_LINES; x > 0; x--)
			chat[x] = chat[x-1];
		chat[0] = ggz_strdup(token);

		output_draw_text();

		token = strsep(&messagedup, "\n");
	}

	ggz_free(orig);

#if 0
	printf(SEQUENCE_SAVE);
	output_goto(window.ws_row - 4, 0);
	printf("\eD%s%s", message, COLOR_BLUE);
	printf(SEQUENCE_RESTORE);
#endif
}

void output_debug(char* fmt, ...)
{
	char message[1024];	/* FIXME: Make me dynamic */
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(message, sizeof(message), fmt, ap);
	va_end(ap);

	output_text("%s%s%s", COLOR_ORANGE, message, COLOR_GREY);
}

void output_draw_text(void)
{
	int x;

	if(!output_enabled) return;

	printf(SEQUENCE_SAVE);
	output_goto(0, 0);
	for (x = window.ws_row - 4 - chat_offset; x >= 0 + chat_offset; x--)
	{
		if (chat[x])
			printf("\e[K%s\n", chat[x]);
		else
			printf("\e[K\n");
	}
	printf(SEQUENCE_RESTORE);
}

void output_chat(GGZChatType type, const char *player, const char *message)
{
	char timestamp[12];
	time_t t;

	if(!output_enabled) return;

	if(1 == 1) {
		t = time(NULL);
		strftime(timestamp, sizeof(timestamp), "%X ", localtime(&t));
	} else {
		strcpy(timestamp, "");
	}

	switch(type) {
	case GGZ_CHAT_BEEP:
		output_text(_("%s--- You've been beeped by %s."), timestamp, player);
		printf(SEQUENCE_BEEP);
		break;
	case GGZ_CHAT_PERSONAL:
		output_text("%s>%s< %s", timestamp, player, message);
		break;
	case GGZ_CHAT_ANNOUNCE:
		output_text("%s[%s] %s", timestamp, player, message);
		break;
	case GGZ_CHAT_TABLE:
		output_text("%s|%s| %s", timestamp, player, message);
		break;
	case GGZ_CHAT_UNKNOWN:
	case GGZ_CHAT_NORMAL:
		output_text("%s<%s> %s", timestamp, player, message);
		break;
	}
}


void output_rooms(void)
{
	int i, num;
	GGZRoom *room;
	GGZGameType *type;

	if(!output_enabled) return;

	num = ggzcore_server_get_num_rooms(server);

	for (i = 0; i < num; i++) {
		room = ggzcore_server_get_nth_room(server, i);
		type = ggzcore_room_get_gametype(room);
		if (type)
			output_text(_("-- Room %d : %s (%s)"), i,
				    ggzcore_room_get_name(room),
				    ggzcore_gametype_get_name(type));
		else
			output_text(_("-- Room %d : %s"), i,
				    ggzcore_room_get_name(room));
	}
}


void output_types(void)
{
	int i, num;
	GGZGameType *type;

	if(!output_enabled) return;

	num = ggzcore_server_get_num_gametypes(server);

	for (i = 0; i < num; i++) {
		type = ggzcore_server_get_nth_gametype(server, i);
		output_text(_("-- Gametype %d : %s"), i,
			    ggzcore_gametype_get_name(type));
	}
}


void output_players(void)
{
	int i, num;
	GGZRoom *room;
	GGZPlayer *player;
	GGZTable *table;
	GGZPlayerType type;

	if(!output_enabled) return;

	room = ggzcore_server_get_cur_room(server);
	num = ggzcore_room_get_num_players(room);

	output_text(_("Players in current room"));
	for (i = 0; i < num; i++) {
		player = ggzcore_room_get_nth_player(room, i);
		table = ggzcore_player_get_table(player);
		type = ggzcore_player_get_type(player);

		output_text(_("-- %s %s %s"),
			ggzcore_player_get_name(player),
			table ? _("at table %d") : "",
			type == GGZ_PLAYER_ADMIN ? _("(Administrator)") :
			((type == GGZ_PLAYER_BOT) ? _("(Bot)") :
			((type == GGZ_PLAYER_GUEST) ? _("(Guest)") :
			"")));
	}
}


void output_tables(void)
{
	int i, num_tables;
	GGZRoom *room;
	GGZTable *table;

	if(!output_enabled) return;

	room = ggzcore_server_get_cur_room(server);
	num_tables = ggzcore_room_get_num_tables(room);

	if (num_tables > 0) {
		output_text(_("Tables in current room:"));
		for (i = 0; i < num_tables; i++) {
			table = ggzcore_room_get_nth_table(room, i);
			output_table_info(table);
		}
	}
	else
		output_text(_("No tables"));
}


void output_status(void)
{
	int num;
	time_t now;		/* time */
	char *currenttime;	/* String formatted time */
	char displaytime[9];	/* What we display */
	const char *user = NULL, *host = NULL, *roomname = NULL;
	char *currentstatus = NULL;
	GGZRoom *room;

	if(!output_enabled) return;

	currentstatus = state_get();

	if (ggzcore_server_is_online(server)) {
		user = ggzcore_server_get_handle(server);
		host = ggzcore_server_get_host(server);
	}

	if (ggzcore_server_is_in_room(server)) {
		room = ggzcore_server_get_cur_room(server);
		roomname = ggzcore_room_get_name(room);
		/*roomnum = ggzcore_room_get_num(room);*/
	}
	
	now = time(NULL);

	if(host)
		num=num-strlen(host);
	
	printf(SEQUENCE_SAVE);
	if(user)
	{
		output_goto(window.ws_row - 3, 0);
		output_label(_("Username"));
		printf("\e[K%s", user);
	} else {
		output_goto(window.ws_row - 3, 0);
		output_label(_("Username"));
		printf("\e[K");
	}
	
	if(host)
	{
		output_goto(window.ws_row - 3, 28);
		output_label(_("Server"));
		printf("\e[K%s", host);
		if(players_on_server > 0)
			printf(_(" (%i players)"), players_on_server);
	} else {
		output_goto(window.ws_row - 3, 28);
		output_label(_("Server"));
		printf("\e[K");
	}

	output_goto(window.ws_row - 3, window.ws_col - 19);
	output_label(_("Status"));
	printf("\e[K%s", currentstatus);
	
	if (ggzcore_server_is_in_room(server))
	{
		output_goto(window.ws_row - 2, 0);
		output_label(_("Room"));
		/*printf("\e[K %d -- %s", roomnum, roomname);*/
		printf("\e[K %s", roomname);
		if(players_in_room > 0)
			printf(_(" (%i players)"), players_in_room);
		
	} else {
		output_goto(window.ws_row - 2, 0);
		output_label(_("Room"));
		printf("\e[K");
	}

	currenttime = ggz_strdup(ctime(&now));
	currenttime[strlen(currenttime)-1] = '\0';
	displaytime[0] = currenttime[11];
	displaytime[1] = currenttime[12];
	displaytime[2] = ':';
	displaytime[3] = currenttime[14];
	displaytime[4] = currenttime[15];
	displaytime[5] = ':';
	displaytime[6] = currenttime[17];
	displaytime[7] = currenttime[18];
	displaytime[8] = '\0';
	ggz_free(currenttime);

	output_goto(window.ws_row - 2, window.ws_col - 19);
	output_label(_("Time"));
	printf("\e[K%s", displaytime);

	printf(SEQUENCE_RESTORE);
}

void output_goto(int row, int col)
{
	if(!output_enabled) return;

	printf("\e[%d;%df", row, col);
}

void output_label(char *label)
{
	if(!output_enabled) return;

	if(reverse_display)
	{
		printf("%s%c%s%s: %s",
			COLOR_BLUE, label[0],
			COLOR_BRIGHT_BLUE, &label[1],
			COLOR_BRIGHT_PINK);
	}
	else
	{
		printf("%s%c%s%s: %s",
			COLOR_BRIGHT_WHITE, label[0],
			COLOR_BRIGHT_GREEN, &label[1],
			COLOR_BRIGHT_PINK);
	}
}

void output_init(int reverse)
{
	if(!output_enabled) return;

	reverse_display = reverse;

	setbuf(stdout, NULL);

	printf("\e[2J");
	ioctl(tty_des, TIOCGWINSZ, &window);
	printf("\e[0;%dr", window.ws_row-4);

	/* Initilize and zero chat memmory */
	chat = ggz_malloc((MAX_LINES + 1) * sizeof(char*));
}

void output_resize(void)
{
	struct winsize window2;

	if(!output_enabled) return;

	ioctl(tty_des, TIOCGWINSZ, &window2);

	if(window2.ws_row != window.ws_row ||
	   window2.ws_col != window.ws_col)
	{
		output_goto(0, 0);
		printf("\e[2J");
		window = window2;
		printf("\e[0;%dr", window.ws_row-4);
		output_prompt();
		output_status();
		output_draw_text();
	}
}


void output_shutdown(void)
{
	int i;

	if(!output_enabled) return;

	printf("\e[0;%dr",window.ws_row);
	printf("\ec\e[2J");

	for (i = 0; chat[i]; i++)
		ggz_free(chat[i]);
	ggz_free(chat);
}


static void output_table_info(GGZTable *table)
{
	int i, num_seats;

	if(!output_enabled) return;

	output_text(_("Table %d : %s"), ggzcore_table_get_id(table),
		    ggzcore_table_get_desc(table));

	num_seats = ggzcore_table_get_num_seats(table);
	for (i = 0; i < num_seats; i++) {
		switch (ggzcore_table_get_nth_player_type(table, i)) {
		case GGZ_SEAT_PLAYER:
			output_text(_("-- Seat %d: %s"), i,
				    ggzcore_table_get_nth_player_name(table, i));
			break;
		case GGZ_SEAT_RESERVED:
			output_text(_("-- Seat %d: Reserved for %s"), i,
				    ggzcore_table_get_nth_player_name(table, i));
			break;
		case GGZ_SEAT_BOT:
			output_text(_("-- Seat %d: -Bot-"), i);
			break;
		case GGZ_SEAT_OPEN:
			output_text(_("-- Seat %d: -Open-"), i);
			break;
		case GGZ_SEAT_NONE:
			output_text(_("-- Not a seat"));
			break;
		default:
			output_text(_("Internal error"));
		}
	}
}
