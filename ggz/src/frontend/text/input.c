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
#include "game.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define LINE_LENGTH 160

static int input_is_command(char *line);
static void input_handle_connect(char* line);
static void input_handle_list(char* line);
static void input_handle_join(char* line);
static void input_handle_join_room(char* line);
static void input_handle_join_table(char* line);
static void input_handle_desc(char* line);
static void input_handle_chat(char* line);
static void input_handle_msg(char* line);
static void input_handle_table(char* line);
static void input_handle_wall(char* line);
static void input_handle_beep(char* line);
static void input_handle_launch(char *line);
static void input_handle_exit(void);

static char delim[] = " \n";
static char command_prefix = '/';

extern GGZServer *server;

void input_command(void)
{
	char line[LINE_LENGTH];
	char *current;
	char *command;

	/* EOF means user closed session */
	if (!fgets(line, sizeof(line)/sizeof(char), stdin)) {
		game_quit();
		if (server) {
			server_disconnect();
			server_destroy();
		}
		loop_quit();
		return;
	}

	/* Get rid of newline char*/
	if (line[strlen(line)-1] == '\n')
		line[strlen(line)-1] = '\0';

	current = line;
	if (input_is_command(line)) {
		
		/* Point at command (minus the prefix char) */
		command = strsep(&current, delim);
		command++;
		
#ifdef DEBUG
		output_text("--- Command is %s", command);
#endif
		
		if (strcmp(command, "connect") == 0) {
			input_handle_connect(current);
		}
		else if (strcmp(command, "logout") == 0) {
			server_logout();
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
		else if (strcmp(command, "table") == 0) {
			input_handle_table(current);
		}
		else if (strcmp(command, "wall") == 0) {
			input_handle_wall(current);
		}
		else if (strcmp(command, "desc") == 0) {
			input_handle_desc(current);
		}
		else if (strcmp(command, "who") == 0) {
			input_handle_list("players");
		}
		else if (strcmp(command, "launch") == 0) {
			input_handle_launch(current);
		}
		else if (strcmp(command, "exit") == 0) {
			input_handle_exit();
		}
		else if (strcmp(command, "version") == 0) {
			output_text("--- Client version: %s", VERSION);
		}
		else {
			output_text("--- Unknown command, try %chelp.", command_prefix);
		}
	} else {
		/* Its a chat */
		input_handle_chat(current);
	}

	output_prompt();
}


static int input_is_command(char *line)
{
	return (line[0] == command_prefix);
}


static void input_handle_connect(char* line)
{
	int portnum;
	char *arg, *host, *port, *login, *pwd;
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
	
	arg = strsep(&line, delim);
	if (arg && strcmp(arg, "") != 0)
	{
		type = GGZ_LOGIN;
		pwd = strdup(arg);
		server_init(host, portnum, type, login, pwd);
	}else{
		server_init(host, portnum, type, login, NULL);
	}
}


static void input_handle_list(char* line)
{
	GGZRoom *room;

	/* What are we listing? */
	if (line && strcmp(line, "types") == 0) {
		if (ggzcore_server_get_num_gametypes(server) > 0)
			output_types();
		else /* Get list from server */
			ggzcore_server_list_gametypes(server, 1);
	}
	else if (line && strcmp(line, "rooms") == 0) {
		if (ggzcore_server_get_num_rooms(server) > 0)
			output_rooms();
		else /* Get list from server */
			ggzcore_server_list_rooms(server, -1, 1);
	}
	else if (line && strcmp(line, "tables") == 0) {
		room = ggzcore_server_get_cur_room(server);
		if (ggzcore_room_get_num_tables(room) > 0)
			output_tables();
		else /* Get list from server */
			ggzcore_room_list_tables(room, -1, 0);
	}
	else if (line && strcmp(line, "players") == 0) {
		room = ggzcore_server_get_cur_room(server);
		if (ggzcore_room_get_num_players(room) > 0) {
			output_players();
		}
		else /* Get list from server */
			ggzcore_room_list_players(room);
	}
	else
		output_text("List what?");
}


static void input_handle_join(char* line)
{
	char *arg;

	arg = strsep(&line, delim);
	if (!arg || strcmp(arg, "") == 0)
		return;

	output_text("Joining a %s", arg);

	/* What are we listing? */
	if (strcmp(arg, "room") == 0)
		input_handle_join_room(line);
	else if (strcmp(arg, "table") == 0)
		input_handle_join_table(line);
	else
		output_text("Join what?");
}


static void input_handle_join_room(char* line)
{
	int room;

	if (line) {
		room = atoi(line);
		ggzcore_server_join_room(server, room);
	} else {
		output_text("Join which room?");
	}
}


static void input_handle_desc(char* line)
{
	int room;
	char* desc;

	if (line) {
		room = atoi(line);
		desc = ggzcore_room_get_desc(ggzcore_server_get_nth_room(server, room));
		output_text("%s", desc);
	} else {
		output_text("Describe which room?");
	}
}


static void input_handle_chat(char *line)
{
	char *msg;
	GGZRoom *room = ggzcore_server_get_cur_room(server);
	GGZStateID state = ggzcore_server_get_state(server);

	if(state == -1 || state == GGZ_STATE_OFFLINE)
	{
		output_text("You must connect to a server first.");
	}
	else if(!room)
	{
		output_text("You must join a room first.");
	}
	else
	{
		if (line && strcmp(line, "") != 0) {
			msg = strdup(line);
			ggzcore_room_chat(room, GGZ_CHAT_NORMAL, NULL, msg);
		}
	}
}


static void input_handle_beep(char* line)
{
	char* player;
	GGZRoom *room = ggzcore_server_get_cur_room(server);

	if (line && strcmp(line, "") != 0) {
		player = strdup(line);
		ggzcore_room_chat(room, GGZ_CHAT_BEEP, player, NULL);
	}
}


static void input_handle_msg(char* line)
{
	char *player;
	char *msg;
	GGZRoom *room = ggzcore_server_get_cur_room(server);

	if (!(player = strsep(&line, delim)))
		return;
	
	if (line && strcmp(line, "") != 0) {
		msg = strdup(line);
		ggzcore_room_chat(room, GGZ_CHAT_PERSONAL, player, msg);
	}
}


static void input_handle_table(char* line)
{
	GGZRoom *room = ggzcore_server_get_cur_room(server);

	if (line[0] == '\0')
		return;

	ggzcore_room_chat(room, GGZ_CHAT_TABLE, NULL, line);
}


static void input_handle_wall(char* line)
{
	char *msg;
	GGZRoom *room = ggzcore_server_get_cur_room(server);

	msg = strdup(line);
	ggzcore_room_chat(room, GGZ_CHAT_ANNOUNCE, NULL, msg);
}


static void input_handle_launch(char *line)
{
	char *name;
	char *engine;
	char *version;
	GGZRoom *room;
	GGZGameType *type;
	GGZModule *module;

	room = ggzcore_server_get_cur_room(server);
	if (!room) {
		output_text("You must be in a room to launch a game");
		return;
	}

	type = ggzcore_room_get_gametype(room);
	if (!type) {
		output_text("No game types defined for this server");
		return;
	}
	
	name = ggzcore_gametype_get_name(type);
	engine = ggzcore_gametype_get_prot_engine(type);
	version = ggzcore_gametype_get_prot_version(type);
	output_text("Launching game of %s, (%s-%s)", name, engine, version);
	module = ggzcore_module_get_nth_by_type(name, engine, version, 0);
	if (!module) {
		output_text("No game modules defined for that game");
		output_text("Download one from %s", 
			    ggzcore_gametype_get_url(type));
		return;
	}

	game_init(module, type, -1);
}


static void input_handle_join_table(char *line)
{
	char *name;
	char *engine;
	char *version;
	GGZRoom *room;
	GGZGameType *type;
	GGZModule *module;
	int table_index;

	if (!line) {
		output_text("Join which table?");
		return;
	}

	room = ggzcore_server_get_cur_room(server);
	if (!room) {
		output_text("You must be in a room to launch a game");
		return;
	}

	type = ggzcore_room_get_gametype(room);
	if (!type) {
		output_text("No game types defined for this server");
		return;
	}
	
	name = ggzcore_gametype_get_name(type);
	engine = ggzcore_gametype_get_prot_engine(type);
	version = ggzcore_gametype_get_prot_version(type);
	output_text("Launching game of %s, (%s-%s)", name, engine, version);
	module = ggzcore_module_get_nth_by_type(name, engine, version, 0);
	if (!module) {
		output_text("No game modules defined for that game");
		output_text("Download one from %s", 
			    ggzcore_gametype_get_url(type));
		return;
	}

	table_index = atoi(line);

	game_init(module, type, table_index);
}


static void input_handle_exit(void)
{
        loop_quit();
}

