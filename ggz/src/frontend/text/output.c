/*
 * File: output.c
 * Author: Brent Hendricks
 * Project: GGZ Text Client 
 * Date: 9/26/00
 * $Id: output.c 5378 2003-02-04 12:48:49Z dr_maux $
 *
 * Functions for display text/messages
 *
 * Copyright (C) 2000 Brent Hendricks.
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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>

#include <ggzcore.h>

#include "output.h"
#include "server.h"
#include "state.h"

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

/* Highest line number in the message buffer */
#define MAX_LINES 100

static struct winsize window;
static int tty_des;
static char **chat;
static int chat_offset = 0;
static int reverse_display = 0;

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

void output_display_help(void)
{
	output_text("--- GGZ Gaming Zone -- Help");
	output_text("--- -----------------------");
	output_text("---");
	output_text("--- /connect [<server>[:<port>]] [<nick>] [<password>]");
	output_text("---    Connect to a GGZ server");
	output_text("--- /disconnect                             Disconnect from server");
	output_text("--- /exit                                   Quit GGZ Gaming Zone");
	output_text("--- /desc <room>                            Get description of room <room>");
	output_text("--- /join room|table <num>                  Join room or table <num>");
	output_text("--- /list players|rooms|tables|types        List the requested information.");
	output_text("--- /msg <player> <msg>                     Send a msg to a player");
	output_text("--- /table <msg>                            Send a msg to your table");
	output_text("--- /beep <player>                          Beep player <player>");
	output_text("--- /launch                                 Launch a game if possible");
	output_text("--- /version                                Display the client version");
	output_text("--- /who                                    List current player in the room");
	output_text("--- /wall <msg>                             Admin only command, broadcast to all rooms");
}

void output_banner(void)
{
	output_text("Welcome to the text-only GGZ client!\n");
	output_text("--Written by Brent Hendricks & Justin Zaun  (C) 2000\n");
}

void output_prompt(void)
{
	fflush(NULL);
	output_goto(window.ws_row, 0);
	printf("\e[2K");
	output_goto(window.ws_row - 1, 0);

#ifndef HAVE_READLINE_READLINE_H
	printf("\e[2K%sGGZ%s>%s ",
		COLOR_BRIGHT_WHITE, COLOR_GREY,
		COLOR_WHITE);
	fflush(NULL);
#else
	printf("\e[2K");
	fflush(NULL);
#endif
}

void output_text(char* fmt, ...)
{
	char message [1024];	/* FIXME: Make me dynamic */
	int x;

	va_list ap;
	va_start(ap, fmt);
	vsnprintf(message, sizeof(message), fmt, ap);
	va_end(ap);

	/* Shift everything in the buffer up */
	if (chat[MAX_LINES])
		ggz_free(chat[MAX_LINES]);
	for (x = MAX_LINES; x > 0; x--)
		chat[x] = chat[x-1];
	chat[0] = ggz_strdup(message);

	output_draw_text();

#if 0
	fflush(NULL);
	printf("\e7");
	output_goto(window.ws_row - 4, 0);
	printf("\eD%s%s", message, COLOR_BLUE);
	printf("\e8");
	fflush(NULL);
#endif
}

void output_draw_text(void)
{
	int x;

	fflush(NULL);
	printf("\e7");
	output_goto(0, 0);
	for (x = window.ws_row - 4 - chat_offset; x >= 0 + chat_offset; x--)
	{
		if (chat[x])
			printf("\e[K%s\n", chat[x]);
		else
			printf("\e[K\n");
		fflush(NULL);
	}
	printf("\e8");
}

void output_chat(GGZChatType type, const char *player, const char *message)
{
	switch(type) {
	case GGZ_CHAT_BEEP:
		output_text("--- You've been beeped by %s.", player);
		printf("\007");
		break;
	case GGZ_CHAT_PERSONAL:
		output_text(">%s< %s", player, message);
		break;
	case GGZ_CHAT_ANNOUNCE:
		output_text("[%s] %s", player, message);
		break;
	case GGZ_CHAT_TABLE:
		output_text("|%s| %s", player, message);
		break;
	case GGZ_CHAT_UNKNOWN:
	case GGZ_CHAT_NORMAL:
		output_text("<%s> %s", player, message);
		break;
	}
}


void output_rooms(void)
{
	int i, num;
	GGZRoom *room;
	GGZGameType *type;

	num = ggzcore_server_get_num_rooms(server);

	for (i = 0; i < num; i++) {
		room = ggzcore_server_get_nth_room(server, i);
		type = ggzcore_room_get_gametype(room);
		if (type)
			output_text("-- Room %d : %s (%s)", i, 
				    ggzcore_room_get_name(room),
				    ggzcore_gametype_get_name(type));
		else
			output_text("-- Room %d : %s", i, 
				    ggzcore_room_get_name(room));
	}
}


void output_types(void)
{
	int i, num;
	GGZGameType *type;

	num = ggzcore_server_get_num_gametypes(server);

	for (i = 0; i < num; i++) {
		type = ggzcore_server_get_nth_gametype(server, i);
		output_text("-- Gametype %d : %s", i, 
			    ggzcore_gametype_get_name(type));
	}
}


void output_players(void)
{
	int i, num;
	GGZRoom *room;
	GGZPlayer *player;
	GGZTable *table;

	room = ggzcore_server_get_cur_room(server);
	num = ggzcore_room_get_num_players(room);

	output_text("Players in current room");
	for (i = 0; i < num; i++) {
		player = ggzcore_room_get_nth_player(room, i);
		table = ggzcore_player_get_table(player);
		if (table)
			output_text("-- %s at table %d", 
				    ggzcore_player_get_name(player),
				    ggzcore_table_get_id(table));
		else 
			output_text("-- %s", ggzcore_player_get_name(player));
	}
}


void output_tables(void)
{
	int i, num_tables;
	GGZRoom *room;
	GGZTable *table;

	room = ggzcore_server_get_cur_room(server);
	num_tables = ggzcore_room_get_num_tables(room);

	if (num_tables > 0) {
		output_text("Tables in current room:");
		for (i = 0; i < num_tables; i++) {
			table = ggzcore_room_get_nth_table(room, i);
			output_table_info(table);
		}
	}
	else
		output_text("No tables");
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
	
	fflush(NULL);
	printf("\e7"); /* Save cursor */
	if(user)
	{
		output_goto(window.ws_row - 3, 0);
		output_label("Username");
		printf("\e[K%s", user);
	} else {
		output_goto(window.ws_row - 3, 0);
		output_label("Username");
		printf("\e[K");
	}
	
	if(host)
	{
		output_goto(window.ws_row - 3, 28);
		output_label("Server");
		printf("\e[K%s", host);
	} else {
		output_goto(window.ws_row - 3, 28);
		output_label("Server");
		printf("\e[K");
	}

	output_goto(window.ws_row - 3, window.ws_col - 19);
	output_label("Status");
	printf("\e[K%s", currentstatus);
	
	if (ggzcore_server_is_in_room(server))
	{
		output_goto(window.ws_row - 2, 0);
		output_label("Room");
		/*printf("\e[K %d -- %s", roomnum, roomname);*/
		printf("\e[K %s", roomname);
		
	} else {
		output_goto(window.ws_row - 2, 0);
		output_label("Room");
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
	output_label("Time");
	printf("\e[K%s", displaytime);

	printf("\e8"); /* Restore cursor */
	fflush(NULL);
}

void output_goto(int row, int col)
{
	printf("\e[%d;%df", row, col);
}

void output_label(char *label)
{
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
	reverse_display = reverse;

	fflush(NULL);
	printf("\e[2J");
	ioctl(tty_des, TIOCGWINSZ, &window);
	printf("\e[0;%dr", window.ws_row-4);
	fflush(NULL);

	/* Initilize and zero chat memmory */
	chat = ggz_malloc((MAX_LINES + 1) * sizeof(char*));
}

void output_resize(void)
{
	struct winsize window2;

	fflush(NULL);
	ioctl(tty_des, TIOCGWINSZ, &window2);
	fflush(NULL);

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

	fflush(NULL);
	printf("\e[0;%dr",window.ws_row);
	printf("\ec\e[2J");
	fflush(NULL);

	for (i = 0; chat[i]; i++)
		ggz_free(chat[i]);
	ggz_free(chat);
}


static void output_table_info(GGZTable *table)
{
	int i, num_seats;

	output_text("Table %d : %s", ggzcore_table_get_id(table),
		    ggzcore_table_get_desc(table));

	num_seats = ggzcore_table_get_num_seats(table);
	for (i = 0; i < num_seats; i++) {
		switch (ggzcore_table_get_nth_player_type(table, i)) {
		case GGZ_SEAT_PLAYER:
			output_text("-- Seat %d: %s", i,
				    ggzcore_table_get_nth_player_name(table, i));
			break;
		case GGZ_SEAT_RESERVED:
			output_text("-- Seat %d: Reserved for %s", i,
				    ggzcore_table_get_nth_player_name(table, i));
			break;
		case GGZ_SEAT_BOT:
			output_text("-- Seat %d: -Bot-", i);
			break;
		case GGZ_SEAT_OPEN:
			output_text("-- Seat %d: -Open-", i);
			break;
		case GGZ_SEAT_NONE:
			output_text("-- Not a seat");
			break;
		default:
			output_text("Internal error");
		}
	}
}
