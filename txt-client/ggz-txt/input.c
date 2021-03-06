/*
 * ggz-txt - Command-line core client for the GGZ Gaming Zone.
 * This application is part of the ggz-txt-client package.
 *
 * input.c: Functions for inputing commands from the user.
 *
 * Copyright (C) 2000 Brent Hendricks
 * Copyright (C) 2001-2008 GGZ Gaming Zone Development Team
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
#include "support.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#ifdef HAVE_READLINE_READLINE_H
#include <readline/readline.h>
#endif
#ifdef HAVE_READLINE_HISTORY_H
#include <readline/history.h>
#endif

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
static void input_handle_log(char *line);
static void input_handle_admin(char* line);

static char delim[] = " \n";
static char command_prefix = '/';

void input_command(void)
{
	input_commandline(NULL);
}

void input_commandhandler(char *current)
{
	char *command;

	if (input_is_command(current)) {
		/* Point at command (minus the prefix char) */
		command = strsep(&current, delim);
		command++;

		output_debug("--- Command is %s", command);

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
			output_display_help();
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
		else if (strcmp(command, "log") == 0) {
			input_handle_log(current);
		}
		else if ((strcmp(command, "exit") == 0)
		|| (strcmp(command, "quit") == 0)) {
			input_handle_exit();
		}
		else if (strcmp(command, "admin") == 0) {
			input_handle_admin(current);
		}
		else if (strcmp(command, "version") == 0) {
			output_text(_("--- Client version: %s"), VERSION);
		}
		else {
			output_text(_("--- Unknown command, try %chelp."), command_prefix);
		}
	} else {
		/* Its a chat */
		input_handle_chat(current);
	}
}

void input_commandline(char *text)
{
#ifndef HAVE_READLINE_READLINE_H
	char line[LINE_LENGTH];
#else
	char *rline;
	char prompt[32];
#endif
	char *current;

#ifdef HAVE_READLINE_READLINE_H
	output_setprompt(prompt, sizeof(prompt));
	if(!text) rline = readline(prompt);
	else rline = text;
	if(!rline) {
		game_quit();
		loop_quit();
		return;
	}
# ifdef HAVE_READLINE_HISTORY_H
	add_history(rline);
# endif
	current = rline;
#else
	/* EOF means user closed session */
	if (!fgets(line, sizeof(line) / sizeof(char), stdin)) {
		game_quit();
		loop_quit();
		return;
	}

	/* Get rid of newline char*/
	if (line[strlen(line)-1] == '\n')
		line[strlen(line)-1] = '\0';

	current = line;
#endif

	input_commandhandler(current);

#ifdef HAVE_READLINE_READLINE_H
	free(rline);
#endif

	output_prompt();
}


static int input_is_command(char *line)
{
	return (line[0] == command_prefix);
}


static void input_handle_connect(char* line)
{
	int portnum;
	char *arg, *host, *port, *login, *pwd, *tls, *email;
	int usetls;
	GGZLoginType type;

	server_workinprogress(COMMAND_CONNECT, 1);

	usetls = 0;

	arg = strsep(&line, delim);
	host = strsep(&arg, ":\n");
	if (host && strcmp(host, "") != 0) {
		/* Check for port */
		port = strsep(&arg, ":\n");
		if (port && !strcmp(port, "tls")) {
			portnum = 5688;
			usetls = 1;
		} else if (port && strcmp(port, "") != 0) {
			portnum = atoi(port);
		} else {
			portnum = 5688;
		}
		tls = strsep(&arg, ":\n");
		if(tls && !strcmp(tls, "tls")) {
			usetls = 1;
		}
	} else {
		host = "localhost";
		portnum = 5688;
	}

	type = GGZ_LOGIN_GUEST;
	pwd = NULL;
	email = NULL;

	arg = strsep(&line, delim);
	if (arg && strcmp(arg, "") != 0)
		login = arg;
	else
		login = getenv("LOGNAME");

	arg = strsep(&line, delim);
	if (arg && strcmp(arg, "") != 0)
	{
		type = GGZ_LOGIN;
		pwd = arg;
	}

	arg = strsep(&line, delim);
	if (arg && strcmp(arg, "") != 0)
	{
		type = GGZ_LOGIN_NEW;
		email = arg;
		output_text(_("Registering new login %s..."), login);
	}

	server_init(host, portnum, type, login, pwd, usetls, email);
}


static void input_handle_list(char* line)
{
	GGZRoom *room;
	GGZStateID state = ggzcore_server_get_state(server);

	if((int)state == -1 || state == GGZ_STATE_OFFLINE)
	{
		output_text(_("You must connect to a server first."));
		return;
	}

	/* What are we listing? */
	if (line && strcmp(line, "types") == 0) {
		if (ggzcore_server_get_num_gametypes(server) > 0)
			output_types();
		else { /* Get list from server */
			server_progresswait();
			server_workinprogress(COMMAND_LIST, 1);
			ggzcore_server_list_gametypes(server, 1);
		}
	}
	else if (line && strcmp(line, "rooms") == 0) {
		if (ggzcore_server_get_num_rooms(server) > 0)
			output_rooms(0);
		else { /* Get list from server */
			server_progresswait();
			server_workinprogress(COMMAND_LIST, 1);
			ggzcore_server_list_rooms(server, 1);
		}
	}
	else if (line && strcmp(line, "allrooms") == 0) {
		if (ggzcore_server_get_num_rooms(server) > 0)
			output_rooms(1);
		else { /* Get list from server */
			server_progresswait();
			server_workinprogress(COMMAND_LIST, 1);
			ggzcore_server_list_rooms(server, 1);
		}
	}
	else if (line && strcmp(line, "tables") == 0) {
		room = ggzcore_server_get_cur_room(server);
		if(!room) {
			output_text(_("You must join a room first."));
			return;
		} else {
			if (ggzcore_room_get_num_tables(room) > 0)
				output_tables();
			else { /* Get list from server */
				server_progresswait();
				server_workinprogress(COMMAND_LIST, 1);
				ggzcore_room_list_tables(room);
			}
		}
	}
	else if (line && strcmp(line, "players") == 0) {
		room = ggzcore_server_get_cur_room(server);
		if(!room) {
			output_text(_("You must join a room first."));
			return;
		} else {
			if (ggzcore_room_get_num_players(room) > 0) {
				output_players();
			}
			else { /* Get list from server */
				server_progresswait();
				server_workinprogress(COMMAND_LIST, 1);
				ggzcore_room_list_players(room);
			}
		}
	}
	else
		output_text(_("List what?"));
}


static void input_handle_join(char* line)
{
	char *arg;

	arg = strsep(&line, delim);
	if (!arg || strcmp(arg, "") == 0) {
		output_text(_("Join what?"));
		return;
	}

	output_text(_("Joining a %s"), arg);

	/* What are we listing? */
	if (strcmp(arg, "room") == 0)
		input_handle_join_room(line);
	else if (strcmp(arg, "table") == 0)
		input_handle_join_table(line);
	else
		output_text(_("Join what?"));
}


static GGZRoom *input_room(const char *line)
{
	GGZRoom *room;
	int i;

	if(!line) return NULL;
	for(i = 0; i < ggzcore_server_get_num_rooms(server); i++)
	{
		room = ggzcore_server_get_nth_room(server, i);
		if (!ggz_strcasecmp(ggzcore_room_get_name(room), line)) {
			return room;
		}
	}

	/* FIXME: we should return an error if a room name was given but not found */
	return ggzcore_server_get_nth_room(server, atoi(line));
}


static void input_handle_join_room(char* line)
{
	GGZRoom *room;
	GGZStateID state = ggzcore_server_get_state(server);
	int ret;

	if((int)state == -1 || state == GGZ_STATE_OFFLINE)
	{
		output_text(_("You must connect to a server first."));
		return;
	}

	if (line) {
		room = input_room(line);
		server_progresswait();
		server_workinprogress(COMMAND_JOIN, 1);
		ret = ggzcore_server_join_room(server, room);
		if(ret < 0) {
			server_workinprogress(COMMAND_JOIN, 0);
			output_text(_("Room %s does not exist."), line);
		}
	} else {
		output_text(_("Join which room?"));
	}
}


static void input_handle_desc(char* line)
{
	GGZRoom *room;
	const char* desc;

	if (line) {
		room = input_room(line);
		desc = ggzcore_room_get_desc(room);
		output_text("%s", desc);
	} else {
		output_text(_("Describe which room?"));
	}
}


static void input_handle_chat(char *line)
{
	char *msg;
	GGZRoom *room = ggzcore_server_get_cur_room(server);
	GGZStateID state = ggzcore_server_get_state(server);

	if (line && strcmp(line, "") != 0) {
		if((int)state == -1 || state == GGZ_STATE_OFFLINE)
		{
			output_text(_("You must connect to a server first."));
		}
		else if(!room)
		{
			output_text(_("You must join a room first."));
		}
		else
		{
			msg = ggz_strdup(line);
			server_progresswait();
			ggzcore_room_chat(room, GGZ_CHAT_NORMAL, NULL, msg);
		}
	}
}


static void input_handle_beep(char* line)
{
	char* player;
	GGZRoom *room = ggzcore_server_get_cur_room(server);

	if (line && strcmp(line, "") != 0) {
		player = ggz_strdup(line);
		server_progresswait();
		ggzcore_room_chat(room, GGZ_CHAT_BEEP, player, NULL);
	}
}


static void input_handle_msg(char* line)
{
	char *player;
	char *msg;
	GGZRoom *room = ggzcore_server_get_cur_room(server);
	GGZStateID state = ggzcore_server_get_state(server);

	if (!(player = strsep(&line, delim)))
	{
		output_text(_("Error: Missing player name."));
		output_text(_("Syntax: /msg <player> <msg>"));
		return;
	}

	if (line && strcmp(line, "") != 0) {
		if((int)state == -1 || state == GGZ_STATE_OFFLINE)
		{
			output_text(_("You must connect to a server first."));
		}
		else if(!room)
		{
			output_text(_("You must join a room first."));
		}
		else
		{
			msg = ggz_strdup(line);
			server_progresswait();
			ggzcore_room_chat(room, GGZ_CHAT_PERSONAL, player, msg);
		}
	} else {
		output_text(_("Error: Missing message."));
		output_text(_("Syntax: /msg <player> <msg>"));
	}
}


static void input_handle_table(char* line)
{
	GGZRoom *room = ggzcore_server_get_cur_room(server);

	if (!line) {
		output_text(_("Send message to which table?"));
		return;
	}

	server_progresswait();
	ggzcore_room_chat(room, GGZ_CHAT_TABLE, NULL, line);
}


static void input_handle_wall(char* line)
{
	char *msg;
	GGZRoom *room = ggzcore_server_get_cur_room(server);

	msg = ggz_strdup(line);
	server_progresswait();
	ggzcore_room_chat(room, GGZ_CHAT_ANNOUNCE, NULL, msg);
}


static GGZModule *input_get_module(const char *modnamepref)
{
	const char * name;
	const char * engine;
	const char * version;
	const char * modname;
	GGZRoom *room;
	GGZGameType *type;
	GGZModule *module, *tmp;
	int i, num;
	GGZModuleEnvironment env;

	room = ggzcore_server_get_cur_room(server);
	if (!room) {
		output_text(_("You must be in a room to launch a game."));
		return NULL;
	}

	type = ggzcore_room_get_gametype(room);
	if (!type) {
		output_text(_("No game types defined for this room. "
			    "Maybe try 'list types'?"));
		return NULL;
	}

	name = ggzcore_gametype_get_name(type);
	engine = ggzcore_gametype_get_prot_engine(type);
	version = ggzcore_gametype_get_prot_version(type);
	output_text(_("Launching game of %s, (%s-%s)"), name, engine, version);

	module = NULL;
	num = ggzcore_module_get_num_by_type(name, engine, version);
	for (i = 0; i < num; i++) {
		tmp = ggzcore_module_get_nth_by_type(name, engine, version, i);
		env = ggzcore_module_get_environment(tmp);
		modname = ggzcore_module_get_name(tmp);
		if (env == GGZ_ENVIRONMENT_CONSOLE) module = tmp;
		if ((getenv("DISPLAY")) && ((env == GGZ_ENVIRONMENT_XWINDOW) ||
			(env == GGZ_ENVIRONMENT_XFULLSCREEN))) module = tmp;
		if((module == tmp) && (modnamepref) && (!ggz_strcmp(modnamepref, modname)))
			break;
	}

	if (!module) {
		output_text(_("No suitable game modules (out of %i) defined for that game."), num);
		output_text(_("Download one from %s."),
			    ggzcore_gametype_get_url(type));
		return NULL;
	}

	output_text(_("Found %i game modules, using frontend '%s'."),
		num, ggzcore_module_get_frontend(module));

	return module;
}

static void input_handle_launch(char *line)
{
	GGZRoom *room;
	GGZGameType *type;
	GGZModule *module;

	server_progresswait();

	module = input_get_module(line);
	if(!module) return;

	room = ggzcore_server_get_cur_room(server);
	type = ggzcore_room_get_gametype(room);

	server_workinprogress(COMMAND_JOIN, 1);
	game_init(module, type, -1);

	if (ggzcore_module_get_environment(module) == GGZ_ENVIRONMENT_CONSOLE)
	{
		output_enable(0);
		loop_remove_fd(STDIN_FILENO);
	}
}


static void input_handle_join_table(char *line)
{
	GGZRoom *room;
	GGZGameType *type;
	GGZModule *module;
	int table_index;

	if (!line) {
		output_text(_("Join which table?"));
		return;
	}

	server_progresswait();

	module = input_get_module(NULL);
	if(!module) return;

	room = ggzcore_server_get_cur_room(server);
	type = ggzcore_room_get_gametype(room);

	/* FIXME: we should allow names to be given for tables if they're unique */
	/* an return an error if a table name was given but not found */
	table_index = atoi(line);
	if (!ggzcore_room_get_table_by_id(room, table_index)) {
		output_text(_("That table doesn't exist! "
			    "Maybe try '/list tables'?"));
		return;
	}

	server_workinprogress(COMMAND_JOIN, 1);
	game_init(module, type, table_index);

	if (ggzcore_module_get_environment(module) == GGZ_ENVIRONMENT_CONSOLE)
	{
		output_enable(0);
		loop_remove_fd(STDIN_FILENO);
	}
}


static void input_handle_exit(void)
{
	loop_quit();
}

static void input_handle_log(char *line)
{
	output_log_init(line);
}

static void input_handle_admin(char* line)
{
	GGZRoom *room;
	char *command, *player, *reason;
	GGZStateID state = ggzcore_server_get_state(server);

	if((int)state == -1 || state == GGZ_STATE_OFFLINE)
	{
		output_text(_("You must connect to a server first."));
		return;
	}

	if (!(command = strsep(&line, delim)))
		return;

	if (!(player = strsep(&line, delim)))
		return;

	reason = NULL;
	if (line && strcmp(line, "") != 0) {
		reason = line;
	}

	room = ggzcore_server_get_cur_room(server);

	/* Type of administrative command */
	if (strcmp(command, "gag") == 0) {
		ggzcore_room_admin(room, GGZ_ADMIN_GAG, player, NULL);
	}
	else if (strcmp(command, "ungag") == 0) {
		ggzcore_room_admin(room, GGZ_ADMIN_UNGAG, player, NULL);
	}
	else if (strcmp(command, "kick") == 0) {
		if (reason) {
			ggzcore_room_admin(room, GGZ_ADMIN_KICK, player, reason);
		}
		else {
			output_text(_("Kicking a player requires a reason."));
		}
	}
	else
		output_text(_("Unknown admin command."));
}

