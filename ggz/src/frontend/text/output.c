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
extern char *Username;
extern char *Server;
extern char *Room;

void output_display_help(void)
{
	output_chat(2, "---", "GNU Gaming Zone -- Help");
	output_chat(2, "---", "-----------------------");
	output_chat(2, "---", " ");
	output_chat(2, "---", "/connect <server>    Connect to a GGZ server");
	output_chat(2, "---", "/disconnect          Disconnect from server");
	output_chat(2, "---", "/join <room>         Join room number <room>");
	output_chat(2, "---", "/beep <player>       Beep player <player>");
	output_chat(2, "---", "/msg <player> <msg>  Send a msg to a player");
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
	printf("\e[%d;0f\e[2K",window.ws_row);
	if (status == 1)
		printf("\e[%d;0f\e[2K\e[1m\e[37mGGZ\e[30m>\e[0m\e[36m ",window.ws_row-1);
	else
		printf("\e[%d;0f\e[1m\e[37mGGZ\e[30m>\e[0m\e[36m ",window.ws_row-1);
	fflush(NULL);
}

void output_chat(int type, char* player, char* fmt, ...)
{
	char message[1024];	/* FIXME: Set to chat length given */
				/* by server			   */
	va_list ap;
	va_start(ap, fmt);
	vsprintf(message, fmt, ap);
	va_end(ap);

	fflush(NULL);
	printf("\e7");
	if(type == 0)
	{
		printf("\e[%d;0f\eD\e[0m\e[37m<%s> %s",window.ws_row-4, player, message);
	} else if (type == 1) {
		printf("\e[%d;0f\eD\e[0m\e[37m>%s< %s",window.ws_row-4, player, message);
	} else if (type == 2) {
		printf("\e[%d;0f\eD\e[0m\e[37m%s  %s",window.ws_row-4, player, message);
	} else if (type == 3) {
		printf("\e[%d;0f\eD\e[0m\e[37m[%s] %s",window.ws_row-4, player, message);
	}
	printf("\e8");
	fflush(NULL);
}

void output_status()
{
	int num;
	time_t now;		/* time */
	char *currenttime;	/* String formatted time */

	now = time(NULL);

	num = window.ws_col - 8;
	if(Server)
		num=num-strlen(Server);
	
	fflush(NULL);
	printf("\e7");
	if(Username)
		printf("\e[%d;0f\e[1m\e[37mU\e[32msername:\e[K \e[36m%s", window.ws_row-3, Username);
	else
		printf("\e[%d;0f\e[1m\e[37mU\e[32msername:\e[K ", window.ws_row-3);
	if(Server)
		printf("\e[%d;%df\e[1m\e[37mS\e[32merver:\e[K \e[36m%s", window.ws_row-3, num, Server);
	else
		printf("\e[%d;%df\e[1m\e[37mS\e[32merver:\e[K ", window.ws_row-3, num);
	if(Room)
		printf("\e[%d;0f\e[1m\e[37mR\e[32moom:\e[K \e[36m%s", window.ws_row-2, Room);
	else
		printf("\e[%d;0f\e[1m\e[37mR\e[32moom:\e[K ", window.ws_row-2);

	currenttime = strdup(ctime(&now));
	currenttime[strlen(currenttime)-1] = '\0';
	printf("\e[%d;%df\e[1m\e[37mT\e[32mime:\e[K \e[36m%s", window.ws_row-2, window.ws_col-strlen(currenttime)-6, currenttime);

	printf("\e8");
	free(currenttime);
	fflush(NULL);
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

