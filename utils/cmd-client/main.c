/*
 * File: main.c
 * Author: Jason Short
 * Project: GGZ Command-line Client
 * Date: 1/7/02
 * $Id: main.c 10848 2009-01-10 22:17:45Z josef $
 *
 * Main program code for ggz-cmd program.
 *
 * Copyright (C) 2002 GGZ Development Team.
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
#  include "config.h"
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> /* For strcasecmp */
#include <sys/types.h>
#include <sys/time.h>
#ifdef HAVE_WINSOCK_H
#  include <winsock.h>
#endif

#include <ggz.h>
#include <ggzcore.h>
#include <ggzcore_mainloop.h>

#ifdef ENABLE_NLS
#  include <locale.h>
#  include <libintl.h>
#  define _(x) gettext(x)
#else
#  define _(x) x
#endif
#define N_(x) x

#define DBG_MAIN "main"

/*
 * The implementation here is currently quite a bit of a hack.
 * I intend to clean it up if and when I (1) add new commands and
 * (2) understand ggzcore a bit better.  --JDS
 */

#define GGZ_CMD_ANNOUNCE_CMD "announce"
#define GGZ_CMD_CHECKONLINE_CMD "checkonline"
#define GGZ_CMD_CHECKNAGIOS_CMD "checknagios"
#define GGZ_CMD_BATCH_CMD "batch"

#define STATUS_CRITICAL 2
#define STATUS_WARNING 1
#define STATUS_OK 0

typedef enum {
	GGZ_CMD_ANNOUNCE,
	GGZ_CMD_CHECKONLINE,
	GGZ_CMD_CHECKNAGIOS,
	GGZ_CMD_BATCH
} CommandType;

typedef struct {
	char *host;
	int port;
	int tls;

	GGZLoginType login_type;
	char *login;
	char *passwd;
	CommandType command;
	void *data;
} GGZCommand;

GGZCommand command;
/* FIXME: add user_data to ggzcore_mainloop? */
GGZCommand *global_cmd;

static GGZServer *server = NULL;
static int in_room = 0;
static int nagiosexit = 0;

static void exec_command(GGZCommand * cmd);

static int exitcode(int code)
{
	if(!nagiosexit)
	{
		if(code == STATUS_OK) code = 0;
		else code = -1;
	}
	return code;
}

static FILE *errorstream(FILE *stream)
{
	if(nagiosexit)
	{
		if(stream == stderr) stream = stdout;
	}
	return stream;
}

static void print_help(char *exec_name)
{
	fprintf(stderr,
		_("Usage: %s [<host>[:<port>][:tls] <login> <passwd>] <command> "
		"[<command opts> ...]\n"),
		exec_name);
	fprintf(stderr,
		_("  Commands include:\n"
		"    " GGZ_CMD_ANNOUNCE_CMD
		" <message> - announce message from room 0\n"
		"    " GGZ_CMD_CHECKONLINE_CMD
		" - check server status\n"
		"    " GGZ_CMD_CHECKNAGIOS_CMD
		" - nagios check for server status\n"
		"    " GGZ_CMD_BATCH_CMD
		" <batchfile> - execute commands from batchfile\n"));

}

/* Parses the command-line arguments into a GGZCommand
   structure.  Returns 0 on success, negative on failure. */
static int parse_arguments(int argc, char **argv, GGZCommand * cmd)
{
	char *cmd_name, *port_num;
	int ret, myret;
	int myargc, i;
	char **myargv;
	FILE *f;
	char *token;
	char line[1024];

	nagiosexit = 0;

	if (argc < 5) {
		if((argc == 3) && (!strcmp(argv[1], GGZ_CMD_BATCH_CMD))) {
			ret = -3;
			f = fopen(argv[2], "r");
			if(f)
			{
				ret = 0;
				while(fgets(line, sizeof(line), f) != NULL)
				{
					myargc = 1;
					myargv = ggz_malloc(2 * sizeof(char*));
					myargv[0] = ggz_strdup(argv[0]);
					myargv[1] = NULL;
					line[strlen(line) - 1] = 0;
					token = strtok(line, " ");
					if(!token) break;
					while(token)
					{
						myargv = ggz_realloc(myargv, (myargc + 2) * sizeof(char*));
						myargv[myargc] = ggz_strdup(token);
						myargv[myargc + 1] = NULL;
						myargc++;
						token = strtok(NULL, " ");
					}

					/*printf("* Command: ");
					for(i = 0; i < myargc; i++)
						printf(" %s", myargv[i]);
					printf("\n");*/

					myret = parse_arguments(myargc, myargv, cmd);
					if(myret == 0) exec_command(cmd);
					ret += myret;
					for(i = 0; i < myargc; i++)
						ggz_free(myargv[i]);
					ggz_free(myargv);
				}
				fclose(f);
			}
			cmd->command = GGZ_CMD_BATCH;
			return ret;
		}
		print_help(argv[0]);
		return -1;
	}

	/* argv[0] -> command name. */

	cmd->host = argv[1];
	cmd->tls = 0;

	port_num = strchr(cmd->host, ':');
	if (port_num) {
		*port_num = '\0';
		port_num++;
		if (!strcmp(port_num, "tls")) {
			cmd->port = 5688;
			cmd->tls = 1;
		} else {
			sscanf(port_num, "%d", &cmd->port);
		}
	} else {
		cmd->port = 5688;
	}

	/* We only allow standard logins, no guest ones. */
	cmd->login_type = GGZ_LOGIN;
	cmd->login = argv[2];
	cmd->passwd = argv[3];

	if(!strcmp(cmd->passwd, "")) {
		cmd->login_type = GGZ_LOGIN_GUEST;
	}

	cmd_name = argv[4];
	if (!strcasecmp(cmd_name, GGZ_CMD_ANNOUNCE_CMD)) {
		cmd->command = GGZ_CMD_ANNOUNCE;
		if (argc < 6) {
			print_help(argv[0]);
			return -1;
		}
		cmd->data = argv[5];
	} else if (!strcasecmp(cmd_name, GGZ_CMD_CHECKONLINE_CMD)){
		cmd->command = GGZ_CMD_CHECKONLINE;
		cmd->data = NULL;
	} else if (!strcasecmp(cmd_name, GGZ_CMD_CHECKNAGIOS_CMD)){
		cmd->command = GGZ_CMD_CHECKNAGIOS;
		cmd->data = NULL;
		nagiosexit = 1;
	} else {
		print_help(argv[0]);
		return -2;
	}

	return 0;
}

static GGZHookReturn server_failure(GGZServerEvent id,
				    const void *event_data,
				    const void *user_data)
{
	const GGZErrorEventData *msg = event_data;
	const char *message = msg->message;

	ggz_debug(DBG_MAIN, "GGZ failure: event %d.", id);
	fprintf(errorstream(stderr),
		_("ggz-cmd: Could not enter room: %s\n"), message);
	exit(exitcode(STATUS_CRITICAL));
}

static GGZHookReturn server_room_entered(GGZServerEvent id,
					 const void *event_data,
					 const void *user_data)
{
	ggz_debug(DBG_MAIN, "Entered room 0.");

	in_room = 1;

	assert(command.data);
	ggzcore_room_chat(ggzcore_server_get_cur_room(server),
			  GGZ_CHAT_ANNOUNCE, NULL, command.data);
	ggz_debug(DBG_MAIN, "Sending announcement.");

	return GGZ_HOOK_OK;
}

static GGZHookReturn server_roomlist(GGZServerEvent id,
				    const void *event_data,
				    const void *user_data)
{
	GGZCommand *cmd = (GGZCommand*)user_data;

	/* Now we're ready to execute the command(s), but how we do it
	   depends on the command itself.  Right now this is hard-wired
	   for the ANNOUNCE command. */
	/*assert(cmd->command == GGZ_CMD_ANNOUNCE);*/
	if(cmd->command == GGZ_CMD_ANNOUNCE)
	{
		ggzcore_server_add_event_hook(server, GGZ_ENTERED, server_room_entered);
		ggzcore_server_add_event_hook(server, GGZ_ENTER_FAIL, server_failure);
		if (ggzcore_server_join_room(server, 0) < 0) {
			ggz_debug(DBG_MAIN, "Server join room failed.  "
				  "There are %d rooms.",
				  ggzcore_server_get_num_rooms(server));
			exit(exitcode(STATUS_WARNING));
		}
	}

	if(cmd->command == GGZ_CMD_CHECKNAGIOS)
	{
		printf(_("ggz-cmd: Everything OK\n"));
	}

	/* FIXME: we don't officially disconnect, we just close
	   the communication! */

	return GGZ_HOOK_OK;
}

static void mainloopfunc(GGZCoreMainLoopEvent id, const char *message, GGZServer *server)
{
	if(id == GGZCORE_MAINLOOP_READY) {
		//printf("READY: %s\n", message);
		ggzcore_server_add_event_hook_full(server, GGZ_ROOM_LIST, server_roomlist, global_cmd);
		ggzcore_server_list_rooms(server, 0);
	} else if(id == GGZCORE_MAINLOOP_WAIT) {
		//fprintf(stderr, "WARNING: %s\n", message);
		exit(exitcode(STATUS_WARNING));
	} else if(id == GGZCORE_MAINLOOP_ABORT) {
		//fprintf(stderr, "ERROR: %s\n", message);
		exit(exitcode(STATUS_CRITICAL));
	}
}

static void exec_command(GGZCommand * cmd)
{
	GGZCoreMainLoop mainloop;

	mainloop.func = mainloopfunc;
	mainloop.reconnect = 1;
	mainloop.loop = 0;
	mainloop.async = 0;
	mainloop.debug = 0;

	mainloop.tls = cmd->tls;
	if(cmd->login_type == GGZ_LOGIN_GUEST)
		mainloop.uri = ggz_strbuild("%s@%s:%i",
			cmd->login, cmd->host, cmd->port);
	else
		mainloop.uri = ggz_strbuild("%s:%s@%s:%i",
			cmd->login, cmd->passwd, cmd->host, cmd->port);

	global_cmd = cmd;

	(void)ggzcore_mainloop_start(mainloop);
}

static void initialize_debugging(void)
{
#ifdef DEBUG
	/* FIXME: We should be able to use GGZCoreMainLoop.debug but
	there's no way to add one type to that list. */
	const char* debug_types[] = {DBG_MAIN,
				     GGZCORE_DBG_CONF, GGZCORE_DBG_GAME,
				     GGZCORE_DBG_HOOK, GGZCORE_DBG_MODULE,
				     GGZCORE_DBG_NET, GGZCORE_DBG_POLL,
				     GGZCORE_DBG_ROOM, GGZCORE_DBG_SERVER,
				     GGZCORE_DBG_STATE, GGZCORE_DBG_TABLE,
				     GGZCORE_DBG_XML, NULL};

	ggz_debug_init(debug_types, NULL);
#endif
}

int main(int argc, char **argv)
{
#ifdef ENABLE_NLS
	bindtextdomain("ggz-cmd", LOCALEDIR);
	textdomain("ggz-cmd");
	setlocale(LC_ALL, "");
#endif

	initialize_debugging();

	if (parse_arguments(argc, argv, &command) < 0) {
		return -1;
	}

	if(command.command != GGZ_CMD_BATCH) exec_command(&command);

	return exitcode(STATUS_OK);
}
