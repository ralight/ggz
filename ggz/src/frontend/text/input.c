/*
 * File: input.c
 * Author: Brent Hendricks
 * Project: GGZ Text Client 
 * Date: 9/26/00
 *
 * Functions for inputing commands from the user
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
#include "input.h"
#include "output.h"

#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define LINE_LENGTH 80

static void input_handle_connect(char* line);
static void input_handle_list(char* line);
static void input_handle_join(char* line);
static void input_handle_chat(char* line);
static void input_handle_msg(char* line);
static void input_handle_beep(char* line);

static char delim[] = " \n";
static char command_prefix = '/';

extern char *Username;
extern char *Server;
extern char *Room;

int input_command(short events)
{
	char line[LINE_LENGTH];
	char *buffer;
	char *command;

	if (events & POLLIN) {
		if (!fgets(line, sizeof(line)/sizeof(char), stdin))
			return -1;
		
		if (line[0] == command_prefix)
		{
			buffer = strtok(line, delim);
			command = &buffer[1];;

			if (strcmp(command, "connect") == 0) {
				input_handle_connect((char*)line);
			}
			else if (strcmp(command, "disconnect") == 0) {
				ggzcore_event_trigger(GGZ_USER_LOGOUT, NULL, NULL);
			}
			else if (strcmp(command, "list") == 0) {
				input_handle_list((char*)line);
			}
			else if (strcmp(command, "join") == 0) {
				input_handle_join((char*)line);
			}
			else if (strcmp(command, "help") == 0) {
				output_display_help();;
			}
			else if (strcmp(command, "beep") == 0) {
				input_handle_beep((char*)line);
			}
			else if (strcmp(command, "msg") == 0) {
				input_handle_msg((char*)line);
			}
		} else {
			/* Its a chat */
			input_handle_chat(line);
		}
		output_prompt(1);
	}
	return 0;
}


static void input_handle_connect(char* line)
{
	GGZProfile *profile;
	char* arg;

	if (!(profile = calloc(1, sizeof(GGZProfile))))
		ggzcore_error_sys_exit("malloc() failed in input_handle_connect");

	arg = strtok(NULL, delim);
	if (arg && strcmp(arg, "") != 0)
	{
		Server = strdup (arg);
		profile->host = strdup(arg);
	} else {
		Server = strdup ("localhost");
		profile->host = strdup("localhost");
	}
	
	arg = strtok(NULL, delim);
	if (arg && strcmp(arg, "") != 0)
		profile->port = atoi(arg);
	else
		profile->port = 5688;

	profile->type = GGZ_LOGIN_GUEST;
	profile->login = strdup(getenv("LOGNAME"));
	Username = strdup(getenv("LOGNAME"));

	/* FIXME: provide a destroy function that frees the appropriate mem */
	ggzcore_event_trigger(GGZ_USER_LOGIN, profile, NULL);
}


static void input_handle_list(char* line)
{
	char* arg;

	/* What are we listing (default to rooms) */
	arg = strtok(NULL, delim);
	if (!arg || strcmp(arg, "rooms") == 0)
		ggzcore_event_trigger(GGZ_USER_LIST_ROOMS, NULL, NULL);
	else if (strcmp(arg, "types") == 0)
		ggzcore_event_trigger(GGZ_USER_LIST_TYPES, NULL, NULL);
	else if (strcmp(arg, "tables") == 0)
		ggzcore_event_trigger(GGZ_USER_LIST_TABLES, NULL, NULL);
	else if (strcmp(arg, "players") == 0)
		ggzcore_event_trigger(GGZ_USER_LIST_PLAYERS, NULL, NULL);
}


static void input_handle_join(char* line)
{
	char* arg;
	int* room;
	
	room = malloc(sizeof(int));

	/* What are we listing (default to rooms) */
	arg = strtok(NULL, delim);
	*room = atoi(arg);
	Room = strdup(arg);

	ggzcore_event_trigger(GGZ_USER_JOIN_ROOM, room, free);
}


static void input_handle_chat(char *line)
{
	char *arg;
	char *msg;

	arg = strtok(line, "\n");
	if(arg)
	{
		msg = strdup(arg);
		ggzcore_event_trigger(GGZ_USER_CHAT, msg, free);
	}
}


static void input_handle_beep(char* line)
{
	char* arg;
	char* player;

	arg = strtok(NULL, delim);
	if (arg) {
		player = strdup(arg);
		ggzcore_event_trigger(GGZ_USER_CHAT_BEEP, player, free);
	}
}


static void input_handle_msg(char* line)
{
	char **data;
	char *arg;


	if (!(data = calloc(2, sizeof(char*))))
		ggzcore_error_sys_exit("calloc() failed in input_handle_msg");

	if (!(arg = strtok(NULL, delim)))
		return;
	
	data[0] = strdup(arg);
	
	arg = strtok(NULL, delim);
	if (arg) {
		data[1] = strdup(arg);
		ggzcore_event_trigger(GGZ_USER_CHAT_PRVMSG, data, free);
	}
}


