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
#include "ggzcore.h"
#include "input.h"
#include "output.h"
#include "server.h"
#include "loop.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define LINE_LENGTH 160

static void input_handle_connect(char* line);
static void input_handle_list(char* line);
static void input_handle_join(char* line);
static void input_handle_desc(char* line);
static void input_handle_chat(char* line);
static void input_handle_msg(char* line);
static void input_handle_beep(char* line);
static void input_handle_exit(void);

static char delim[] = " \n";
static char command_prefix = '/';

extern GGZServer *server;

void input_command(void)
{
	char line[LINE_LENGTH];
	char *current;
	char *command;

	if (!fgets(line, sizeof(line)/sizeof(char), stdin)) {
		loop_quit();
		return;
	}

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
			server_disconnect();
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
		else if (strcmp(command, "desc") == 0) {
			input_handle_desc(current);
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

	output_prompt();
}


static void input_handle_connect(char* line)
{
	int portnum;
	char *arg, *host, *port, *login;
	GGZLoginType type;

	arg = strsep(&line, delim);
	host = strsep(&arg, ":\n");
	if (host && strcmp(host, "") != 0)
	{
		/* Check for port */
		port = strsep(&arg, ":\n");
		if (port && strcmp(port, "") != 0)
			portnum = atoi(port);
		else 
			portnum = 5688;
	} else {
		host = strdup("localhost");
		portnum = 5688;
	}
	
	type = GGZ_LOGIN_GUEST;

	arg = strsep(&line, delim);
	if (arg && strcmp(arg, "") != 0)
		login = strdup(arg);
	else
		login = strdup(getenv("LOGNAME"));
	
	server_init(host, portnum, type, login, NULL);
}


static void input_handle_list(char* line)
{
	/* What are we listing? */
	if (strcmp(line, "types") == 0)
		ggzcore_event_enqueue(GGZ_USER_LIST_TYPES, NULL, NULL);
	else if (strcmp(line, "tables") == 0)
		ggzcore_event_enqueue(GGZ_USER_LIST_TABLES, NULL, NULL);
	else if (strcmp(line, "players") == 0) {
		if (ggzcore_player_get_num() >= 1)
			output_players();
		else 
			ggzcore_event_enqueue(GGZ_USER_LIST_PLAYERS, NULL, NULL);
	}
	else if (strcmp(line, "rooms") == 0) {
		if (ggzcore_server_get_num_rooms(server) >= 1)
			output_rooms();
		else 
			ggzcore_server_list_rooms(server, -1, 1);
	}
}


static void input_handle_join(char* line)
{
	int room;
	
	room = atoi(line);
	ggzcore_server_join_room(server, room);
}


static void input_handle_desc(char* line)
{
	int room;
	char* desc;
	
	room = atoi(line);
	desc = ggzcore_server_get_room_desc(server, room);
	output_text(desc);
}


static void input_handle_chat(char *line)
{
	char *msg;

	if (strcmp(line, "") != 0) {
		msg = strdup(line);
		ggzcore_server_chat(server, GGZ_CHAT_NORMAL, NULL, msg);
	}
}


static void input_handle_beep(char* line)
{
	char* player;

	if (strcmp(line, "") != 0) {
		player = strdup(line);
		ggzcore_server_chat(server, GGZ_CHAT_BEEP, player, NULL);
	}
}


static void input_handle_msg(char* line)
{
	char *player;
	char *msg;

	if (!(player = strsep(&line, delim)))
		return;
	
	if (line && strcmp(line, "") != 0) {
		msg = strdup(line);
		ggzcore_server_chat(server, GGZ_CHAT_PERSONAL, player, msg);
	}
}

static void input_handle_exit(void)
{
        ggzcore_event_process_all();
        ggzcore_destroy();
        output_shutdown();
        exit(1);
}
