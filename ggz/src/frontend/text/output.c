/*
 * File: output.c
 * Author: Brent Hendricks
 * Project: GGZ Text Client 
 * Date: 9/26/00
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
#include <ggzcore.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include "output.h"
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

extern GGZServer *server;

static struct winsize window;
static int tty_des;

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
	output_text("--- GNU Gaming Zone -- Help");
	output_text("--- -----------------------");
	output_text("---");
	output_text("--- /beep <player>                          Beep player <player>");
	output_text("--- /connect [<server>[:<port>]] [<nick>]   Connect to a GGZ server");
	output_text("--- /disconnect                             Disconnect from server");
	output_text("--- /exit                                   Quit GNU Gaming Zone");
	output_text("--- /desc <room>                            Get description of room <room>");
	output_text("--- /join <room>                            Join room number <room>");
	output_text("--- /msg <player> <msg>                     Send a msg to a player");
	output_text("--- /version                                Display the client version");
	output_text("--- /who                                    List current player in the room");
}

void output_banner(void)
{
	fflush(NULL);
	printf("\e[f\n");
	printf("Welcome to the text-only GGZ client!\n");
	printf("--Written by Brent Hendricks & Justin Zaun  (C) 2000\n");
	printf("\n");
	fflush(NULL);
}

void output_prompt(void)
{
	fflush(NULL);
	output_goto(window.ws_row, 0);
	printf("\e[2K");
	output_goto(window.ws_row - 1, 0);
	printf("\e[2K%sGGZ%s>%s ",
		COLOR_BRIGHT_WHITE, COLOR_GREY,
		COLOR_WHITE);
	fflush(NULL);
}

void output_text(char* fmt, ...)
{
	char message [1024];	/* FIXME: Make me dynamic */

	va_list ap;
	va_start(ap, fmt);
	vsprintf(message, fmt, ap);
	va_end(ap);

	fflush(NULL);
	printf("\e7");
	output_goto(window.ws_row - 4, 0);
	printf("\eD%s%s", message, COLOR_BLUE);
	printf("\e8");
	fflush(NULL);
}

void output_chat(ChatTypes type, char *player, char *message)
{
	switch(type)
	{
		case CHAT_MSG:
			output_text("<%s> %s", player, message);
			break;
		case CHAT_PRVMSG:
			output_text(">%s< %s", player, message);
			break;
		case CHAT_ANNOUNCE:
			output_text("[%s] %s", player, message);
			break;
	}
}


void output_rooms(void)
{
	int i;
	char** names = NULL;

	if (!(names = ggzcore_server_get_room_names(server)))
		return;  

	for (i = 0; names[i]; i++)
		output_text("-- Room %d : %s", i, names[i]);

	free(names);
}


void output_players(void)
{
	int i;
	char** names;
	GGZRoom *room;

	room = ggzcore_server_get_cur_room(server);

	if (!(names = ggzcore_room_get_player_names(room)))
		return;  

	output_text("Players in current room");
	for (i = 0; names[i]; i++)
		output_text("-- %s", names[i]);

	free(names);
}


void output_status(void)
{
	int num, roomnum = -1;
	time_t now;		/* time */
	char *currenttime;	/* String formatted time */
	char displaytime[9];	/* What we display */
	char *user = NULL, *host = NULL, *roomname = NULL;
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
		roomnum = ggzcore_room_get_num(room);
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
		printf("\e[K %d -- %s", roomnum, roomname);
	} else {
		output_goto(window.ws_row - 2, 0);
		output_label("Room");
		printf("\e[K");
	}

	currenttime = strdup(ctime(&now));
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
	free(currenttime);

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
	printf("%s%c%s%s: %s",
		COLOR_BRIGHT_WHITE, label[0],
		COLOR_BRIGHT_GREEN, &label[1],
		COLOR_BRIGHT_PINK);
}

void output_init(void)
{
	fflush(NULL);
	printf("\e[2J");
	ioctl(tty_des, TIOCGWINSZ, &window);
	printf("\e[0;%dr", window.ws_row-4);
	fflush(NULL);
}

void output_shutdown(void)
{
	fflush(NULL);
	printf("\e[0;%dr",window.ws_row);
	printf("\ec\e[2J");
	fflush(NULL);
}

