/*
 * File: main.c
 * Author: Jason Short
 * Project: GGZ Command-line Client
 * Date: 1/7/02
 * $Id: main.c 4493 2002-09-09 05:43:00Z jdorje $
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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <ggzcore.h>

/*
 * The implementation here is currently quite a bit of a hack.
 * I intend to clean it up if and when I (1) add new commands and
 * (2) understand ggzcore a bit better.  --JDS
 */

#define GGZ_CMD_ANNOUNCE_CMD "announce"

typedef enum {
	GGZ_CMD_ANNOUNCE
} CommandType;

typedef struct {
	char *host;
	int port;

	GGZLoginType login_type;
	char *login;
	char *passwd;
	CommandType command;
	void *data;
} GGZCommand;

GGZCommand command;

GGZServer *server = NULL;
int server_fd = -1;
int in_room = 0;

static void print_help(char *exec_name)
{
	fprintf(stderr,
		"Usage: %s <host>[:<port>] <login> <passwd> <command> "
		"[<command opts> ...]\n",
		exec_name);
	fprintf(stderr,
		"  Commands include:\n"
		"    " GGZ_CMD_ANNOUNCE_CMD
		" <message> - announce message from room 0.\n");
	fprintf(stderr,
		"  Bugs and issues:\n"
		"    - Only one command is currently supported.\n");

}

/* Parses the command-line arguments into a GGZCommand
   structure.  Returns 0 on success, negative on failure. */
static int parse_arguments(int argc, char **argv, GGZCommand * cmd)
{
	char *cmd_name, *port_num;

	if (argc < 5) {
		print_help(argv[0]);
		return -1;
	}

	/* argv[0] -> command name. */

	cmd->host = argv[1];
	
	port_num = strchr(cmd->host, ':');
	if (port_num) {
		*port_num = '\0';
		port_num++;
		sscanf(port_num, "%d", &cmd->port);
	} else
		cmd->port = 5688;

	/* We only allow standard logins, no guest ones. */
	cmd->login_type = GGZ_LOGIN;
	cmd->login = argv[2];
	cmd->passwd = argv[3];

	cmd_name = argv[4];
	if (!strcmp(cmd_name, "announce")) {
		cmd->command = GGZ_CMD_ANNOUNCE;
		if (argc < 6) {
			print_help(argv[0]);
			return -1;
		}
		cmd->data = argv[5];
	} else {
		print_help(argv[0]);
		return -2;
	}

	return 0;
}

static void wait_for_input(int fd)
{
	fd_set my_fd_set;
	int status;

	assert(fd >= 0);

	do {
		FD_ZERO(&my_fd_set);
		FD_SET(fd, &my_fd_set);

		status = select(fd + 1, &my_fd_set, NULL, NULL, NULL);
		if (status < 0) {
			fprintf(stderr, "Select error while blocking.");
			exit(-1);
		}
	} while (status < 0);
}

static GGZHookReturn server_failure(GGZServerEvent id,
				    void *event_data, void *user_data)
{
	fprintf(stderr, "GGZ failure: event %d.\n", id);
	exit(-1);
}

static GGZHookReturn server_connected(GGZServerEvent id,
				      void *event_data, void *user_data)
{
	fprintf(stderr, "Connected to server.\n");
	server_fd = ggzcore_server_get_fd(server);
	return GGZ_HOOK_OK;
}

static GGZHookReturn server_negotiated(GGZServerEvent id,
				       void *event_data, void *user_data)
{
	fprintf(stderr, "Server negotiated.\n");
	ggzcore_server_login(server);
	return GGZ_HOOK_OK;
}

static GGZHookReturn server_logged_in(GGZServerEvent id,
				      void *event_data, void *user_data)
{
	fprintf(stderr, "Logged in to server.\n");

	ggzcore_server_list_rooms(server, 0, 0);

	return GGZ_HOOK_OK;
}

static GGZHookReturn server_room_entered(GGZServerEvent id,
					 void *event_data, void *user_data)
{
	fprintf(stderr, "Entered room 0.\n");
	in_room = 1;
	return GGZ_HOOK_OK;
}

static void exec_command(GGZCommand * cmd)
{
	server = ggzcore_server_new();
	ggzcore_server_set_hostinfo(server, cmd->host, cmd->port);
	ggzcore_server_set_logininfo(server, cmd->login_type,
				     cmd->login, cmd->passwd);

	/* Register necessary events. */
	ggzcore_server_add_event_hook(server, GGZ_CONNECT_FAIL,
				      server_failure);
	ggzcore_server_add_event_hook(server, GGZ_CONNECTED,
				      server_connected);
	ggzcore_server_add_event_hook(server, GGZ_NEGOTIATED,
				      server_negotiated);
	ggzcore_server_add_event_hook(server, GGZ_NEGOTIATE_FAIL,
				      server_failure);
	ggzcore_server_add_event_hook(server, GGZ_LOGGED_IN,
				      server_logged_in);
	ggzcore_server_add_event_hook(server, GGZ_LOGIN_FAIL, server_failure);

	ggzcore_server_add_event_hook(server, GGZ_NET_ERROR, server_failure);
	ggzcore_server_add_event_hook(server, GGZ_PROTOCOL_ERROR,
				      server_failure);

	ggzcore_server_connect(server);

	do {
		wait_for_input(server_fd);
		ggzcore_server_read_data(server, ggzcore_server_get_channel(server));
	} while (ggzcore_server_get_num_rooms(server) <= 0);


	/* Now we're ready to execute the command(s), but how we do it
	   depends on the command itself.  Right now this is hard-wired
	   for the ANNOUNCE command. */
	assert(cmd->command == GGZ_CMD_ANNOUNCE);
	ggzcore_server_add_event_hook(server, GGZ_ENTERED,
				      server_room_entered);
	ggzcore_server_add_event_hook(server, GGZ_ENTER_FAIL, server_failure);
	if (ggzcore_server_join_room(server, 0) < 0) {
		fprintf(stderr, "Server join room failed.  "
			"There are %d rooms.\n",
			ggzcore_server_get_num_rooms(server));
		exit(-1);
	}

	do {
		wait_for_input(server_fd);
		ggzcore_server_read_data(server, ggzcore_server_get_channel(server));
	} while (!in_room);

	assert(command.data);
	ggzcore_room_chat(ggzcore_server_get_cur_room(server),
			  GGZ_CHAT_ANNOUNCE, NULL, command.data);
	fprintf(stderr, "Sending announcement.\n");

	/* FIXME: we don't officially disconnect, we just close
	   the communication! */
}

int main(int argc, char **argv)
{
	if (parse_arguments(argc, argv, &command) < 0) {
		return -1;
	}

	exec_command(&command);
	return 0;
}
