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

static struct winsize window;
static int tty_des;

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
	output_text("--- /connect <server>    Connect to a GGZ server");
	output_text("--- /disconnect          Disconnect from server");
	output_text("--- /join <room>         Join room number <room>");
	output_text("--- /beep <player>       Beep player <player>");
	output_text("--- /msg <player> <msg>  Send a msg to a player");
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

void output_prompt(int status)
{
	fflush(NULL);
	output_goto(window.ws_row, 0);
	printf("\e[2K");
	if (status == 1)
	{
		output_goto(window.ws_row - 1, 0);
		printf("\e[2K%sGGZ%s>%s ",
			COLOR_BRIGHT_WHITE, COLOR_GREY,
			COLOR_WHITE);
	} else {
		output_goto(window.ws_row - 1, 0);
		printf("%sGGZ%s>%s ",
			COLOR_BRIGHT_WHITE, COLOR_GREY,
			COLOR_WHITE);
	}
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

void output_status()
{
	int num;
	time_t now;		/* time */
	char *currenttime;	/* String formatted time */
	char displaytime[9];	/* What we display */
	char *user = NULL, *server = NULL, *room = NULL;
	char currentstatus[11];

	switch((int)ggzcore_state_get_id())
	{
		case GGZ_STATE_OFFLINE:
			strcpy(currentstatus, "Offline");
			break;
		case GGZ_STATE_CONNECTING:
			strcpy(currentstatus, "Connecting");
			break;
		case GGZ_STATE_ONLINE:
			strcpy(currentstatus, "Online");
			break;
		case GGZ_STATE_LOGGING_IN:
			strcpy(currentstatus, "Logging In");
			break;
		case GGZ_STATE_LOGGED_IN:
			strcpy(currentstatus, "Logged In");
			break;
		case GGZ_STATE_ENTERING_ROOM:
			strcpy(currentstatus, "--> Room");
			break;
		case GGZ_STATE_IN_ROOM:
			strcpy(currentstatus, "Chatting");
			break;
		case GGZ_STATE_JOINING_TABLE:
			strcpy(currentstatus, "--> Table");
			break;
		case GGZ_STATE_AT_TABLE:
			strcpy(currentstatus, "Playing");
			break;
		case GGZ_STATE_LEAVING_TABLE:
			strcpy(currentstatus, "<-- Table");
			break;
		case GGZ_STATE_LOGGING_OUT:
			strcpy(currentstatus, "Logging Out");
			break;
	}
	
	if (ggzcore_state_is_online()) {
		user = ggzcore_state_get_profile_login();
		server = ggzcore_state_get_profile_host();
	}
	
	if (ggzcore_state_is_in_room())
		room = ggzcore_room_get_name(ggzcore_state_get_room());
	
	now = time(NULL);

	if(server)
		num=num-strlen(server);
	
	fflush(NULL);
	printf("\e7"); /* Save cursor */
	if(user)
	{
		output_goto(window.ws_row - 3, 0);
		printf("%sU%ssername:\e[K %s%s",
			COLOR_BRIGHT_WHITE, COLOR_BRIGHT_GREEN,
			COLOR_BRIGHT_PINK, user);
	} else {
		output_goto(window.ws_row - 3, 0);
		printf("%sU%ssername:\e[K ",
			COLOR_BRIGHT_WHITE, COLOR_BRIGHT_GREEN);
	}
	
	if(server)
	{
		output_goto(window.ws_row - 3, 28);
		printf("%sS%server:\e[K %s%s",
			COLOR_BRIGHT_WHITE, COLOR_BRIGHT_GREEN,
			COLOR_BRIGHT_PINK, server);
	} else {
		output_goto(window.ws_row - 3, 28);
		printf("%sS%server:\e[K",
			COLOR_BRIGHT_WHITE, COLOR_BRIGHT_GREEN);
	}

	output_goto(window.ws_row - 3, window.ws_col - 19);
	printf("%sS%status:\e[K %s%s",
		COLOR_BRIGHT_WHITE, COLOR_BRIGHT_GREEN,
		COLOR_BRIGHT_PINK, currentstatus);
	
	if (ggzcore_state_is_in_room())
	{
		output_goto(window.ws_row - 2, 0);
		printf("%sR%soom:\e[K %s%d - %s",
			COLOR_BRIGHT_WHITE, COLOR_BRIGHT_GREEN,
			COLOR_BRIGHT_PINK, ggzcore_state_get_room(),
			room);
	} else {
		output_goto(window.ws_row - 2, 0);
		printf("%sR%soom:\e[K",
			COLOR_BRIGHT_WHITE, COLOR_BRIGHT_GREEN);
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
	printf("%sT%sime:\e[K %s%s",
			COLOR_BRIGHT_WHITE, COLOR_BRIGHT_GREEN,
			COLOR_BRIGHT_PINK, displaytime);

	printf("\e8"); /* Restore cursor */
	fflush(NULL);
}

void output_goto(int row, int col)
{
	printf("\e[%d;%df", row, col);
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

