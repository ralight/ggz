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


#define LINE_LENGTH 160

static void input_handle_connect(char* line);
static void input_handle_list(char* line);
static void input_handle_join(char* line);
static void input_handle_chat(char* line);
static void input_handle_msg(char* line);
static void input_handle_beep(char* line);
static void input_handle_exit(void);

static char delim[] = " \n";
static char command_prefix = '/';


int input_command(short events)
{
	char line[LINE_LENGTH];
	char *current;
	char *command;

	if (events & POLLIN) {
		if (!fgets(line, sizeof(line)/sizeof(char), stdin))
			return -1;

		/* Get rid of newline char*/
		if (line[strlen(line)-1] == '\n')
			line[strlen(line)-1] = '\0';

		current = line;
		if (line[0] == command_prefix)
		{
			/* Point at command (minus the prefix char) */
			command = strsep(&current, delim);
			command++;
			
#ifdef DEBUG
			output_text("--- Command is %s", command);
#endif
			
			if (strcmp(command, "connect") == 0) {
				input_handle_connect(current);
			}
			else if (strcmp(command, "disconnect") == 0) {
				ggzcore_event_trigger(GGZ_USER_LOGOUT, NULL, NULL);
			}
			else if (strcmp(command, "list") == 0) {
				input_handle_list(current);
			}
			else if (strcmp(command, "join") == 0) {
				input_handle_join(current);
			}
			else if (strcmp(command, "help") == 0) {
				output_display_help();;
			}
			else if (strcmp(command, "beep") == 0) {
				input_handle_beep(current);
			}
			else if (strcmp(command, "msg") == 0) {
				input_handle_msg(current);
			}
			else if (strcmp(command, "who") == 0) {
				input_handle_list("players");
			}
			else if (strcmp(command, "exit") == 0) {
				input_handle_exit();
			}
			else if (strcmp(command, "version") == 0) {
				output_text("--- Client version: %s", VERSION);
			}
		} else {
			/* Its a chat */
			input_handle_chat(current);
		}
		output_prompt(1);
	}
	return 0;
}


static void input_handle_connect(char* line)
{
	GGZProfile *profile;
	char *arg, *server, *port;

	if (!(profile = calloc(1, sizeof(GGZProfile))))
		ggzcore_error_sys_exit("malloc() failed in %s", __FILE__);

	arg = strsep(&line, delim);
	server = strsep(&arg, ":\n");
	if (server && strcmp(server, "") != 0)
	{
		profile->host = strdup(server);
		/* Check for port */
		port = strsep(&arg, ":\n");
		if (port && strcmp(port, "") != 0)
			profile->port = atoi(port);
		else 
			profile->port = 5688;
	} else {
		profile->host = strdup("localhost");
		profile->port = 5688;
	}
	
	profile->type = GGZ_LOGIN_GUEST;

	arg = strsep(&line, delim);
	if (arg && strcmp(arg, "") != 0)
		profile->login = strdup(arg);
	else
		profile->login = strdup(getenv("LOGNAME"));
	

	/* FIXME: provide a destroy function that frees the appropriate mem */
	ggzcore_event_trigger(GGZ_USER_LOGIN, profile, NULL);
}


static void input_handle_list(char* line)
{
	/* What are we listing? */
	if (strcmp(line, "types") == 0)
		ggzcore_event_trigger(GGZ_USER_LIST_TYPES, NULL, NULL);
	else if (strcmp(line, "tables") == 0)
		ggzcore_event_trigger(GGZ_USER_LIST_TABLES, NULL, NULL);
	else if (strcmp(line, "players") == 0) {
		if (ggzcore_player_get_num() >= 1)
			output_players();
		else 
			ggzcore_event_trigger(GGZ_USER_LIST_PLAYERS, NULL, NULL);
	}
	else if (strcmp(line, "rooms") == 0) {
		if (ggzcore_room_get_num() >= 1)
			output_rooms();
		else 
			ggzcore_event_trigger(GGZ_USER_LIST_ROOMS, NULL, NULL);
	}
}


static void input_handle_join(char* line)
{
	int room;
	
	room = atoi(line);
	ggzcore_event_trigger(GGZ_USER_JOIN_ROOM, (void*)room, NULL);
}


static void input_handle_chat(char *line)
{
	char *msg;

	if (strcmp(line, "") != 0) {
		msg = strdup(line);
		ggzcore_event_trigger(GGZ_USER_CHAT, msg, free);
	}
}


static void input_handle_beep(char* line)
{
	char* player;

	if (strcmp(line, "") != 0) {
		player = strdup(line);
		ggzcore_event_trigger(GGZ_USER_CHAT_BEEP, player, free);
	}
}


static void input_handle_msg(char* line)
{
	char **data;
	char *arg;

	if (!(data = calloc(2, sizeof(char*))))
		ggzcore_error_sys_exit("calloc() failed in input_handle_msg");

	if (!(arg = strsep(&line, delim)))
		return;
	
	data[0] = strdup(arg);
	
	if (line && strcmp(line, "") != 0) {
		data[1] = strdup(line);
		ggzcore_event_trigger(GGZ_USER_CHAT_PRVMSG, data, free);
	}
}

static void input_handle_exit(void)
{
        ggzcore_event_process_all();
        ggzcore_destroy();
        output_shutdown();
        exit(1);
}
