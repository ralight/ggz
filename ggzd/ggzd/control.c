/*
 * File: control.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 10/11/99
 * Desc: Control/Port-listener part of server
 *
 * Copyright (C) 1999 Brent Hendricks.
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


#include <config.h>		/* Site specific config */

#include <sys/socket.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#include <easysock.h>
#include <datatypes.h>
#include <err_func.h>
#include <players.h>
#include <parse_opt.h>
#include <chat.h>

/* Server options */
Options opt;

/* Main data structures */
struct GameTypes game_types;
struct GameTables tables;
struct Users players;


void init_data(void)
{

	int i;
	GameInfo spades;

	pthread_rwlock_init(&game_types.lock, NULL);
	pthread_rwlock_init(&tables.lock, NULL);
	pthread_rwlock_init(&players.lock, NULL);

	for (i = 0; i < MAX_USERS; i++)
		players.info[i].fd = -1;

	for (i = 0; i < MAX_TABLES; i++)
		tables.info[i].type_index = -1;
	
	
	/* FIXME: Temporary hack.  This info should be loaded from a file
	 * or something
	 */
	strncpy(spades.name, "NetSpades", MAX_GAME_NAME_LEN);
	strncpy(spades.version, "0.0.1", MAX_GAME_VER_LEN);
	strncpy(spades.desc,
		"NetSpades is a multiuser networked spades game",
		MAX_GAME_DESC_LEN);
	strncpy(spades.author, "Brent Hendricks", MAX_GAME_AUTH_LEN);
	strncpy(spades.homepage, "http://www.ece.rice.edu/~brentmh/spades", 
		MAX_GAME_WEB_LEN);
	spades.num_play_allow = (char) (PLAY_ALLOW_FOUR);
	spades.comp_allow =
		(char) (COMP_ALLOW_ZERO | COMP_ALLOW_ONE | COMP_ALLOW_TWO |
			COMP_ALLOW_THREE);
	spades.options_size = 12;
	spades.enabled = 1;
	spades.launch = NULL;
	strncpy(spades.path, "../game_servers/spades/spades", MAX_PATH_LEN);

	game_types.info[0] = spades;
	game_types.count++;

	chat_init();
}


void daemon_init(const char *pname, int facility)
{

	int pid;

	/* Daemonize */
	if ((pid = fork()) < 0)
		err_sys_exit("fork failed");
	else if (pid != 0)
		exit(0);

	setsid();
        /*chdir("/");*/
	umask(0);
        close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

}


int main(int argc, char *argv[])
{

	int main_sock, new_sock;
	
	/* Parse options */
	parse_args(argc, argv);
	parse_conf_file();

	/* Defaults */
	if(!opt.main_port)
		opt.main_port = 1174;

	dbg_msg("Conf file: %s", opt.local_conf);
	dbg_msg("Log level: %d", opt.log_level);
	dbg_msg("Main Port: %d", opt.main_port);

#ifndef DEBUG
	daemon_init(argv[0], 0);
#endif

	init_data();

#if 0				/* FIXME: need to write these */
	setup_sighandlers();
	load_game_mods();
	setup_config_socket();
#endif



	/* Create SERVER socket on main_port */
	main_sock = es_make_socket_or_die(ES_SERVER, opt.main_port, NULL);
	
	/* Start accepting connections */
	if (FAIL(listen(main_sock, MAX_USERS)))
		err_sys_exit("Error listening to socket");

	/* Main loop */
	for (;;) {

		if (FAIL(new_sock = accept(main_sock, NULL, NULL))) {
			if (errno == EINTR)
				continue;
			else
				err_sys_exit("Error accepting connection");
		} else
			player_handler_launch(new_sock);
		
	}		


#if 0				/* FIXME: Not implemented yet */
	cleanup();
#endif

	return 0;

}
