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
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <signal.h>

#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include <easysock.h>
#include <ggzd.h>
#include <datatypes.h>
#include <err_func.h>
#include <players.h>
#include <parse_opt.h>
#include <motd.h>
#include <room.h>
#include <ggzdb.h>
#include <table.h>

/* Server options */
Options opt;

/* Main data structures */
struct GameTypes game_types;
struct GameTables tables;
struct Users players;


/* Perhaps these should be put into their respective files? */
void init_data(void)
{
	int i;

	pthread_rwlock_init(&game_types.lock, NULL);
	pthread_rwlock_init(&tables.lock, NULL);
	pthread_rwlock_init(&players.lock, NULL);

	for (i = 0; i < MAX_USERS; i++)
		players.info[i].fd = -1;

	for (i = 0; i < MAX_TABLES; i++)
		tables.info[i].type_index = -1;
	
	/* This is not necessary since game_types is in BSS */
	for (i = 0; i < MAX_GAME_TYPES; i++)
		game_types.info[i].enabled = 0;

	if(ggzdb_init() < 0)
		err_msg_exit("*** Database initialization failed");

	/* Initialize random numbers for password generation */
	srandom((unsigned)time(NULL));
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
	/* Should chdir() to gamedir?*/
        /*chdir("/");*/
	umask(0);
        close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

}


int main(int argc, const char *argv[])
{
	int main_sock, new_sock;
	struct sockaddr_in addr;
	int addrlen;
	
	/* Parse options */
	parse_args(argc, argv);
	parse_conf_file();
	
	dbg_msg(GGZ_DBG_CONFIGURATION, "Conf file: %s", opt.local_conf);
	dbg_msg(GGZ_DBG_CONFIGURATION, "Log level: %0X", log_info.log_types);
	dbg_msg(GGZ_DBG_CONFIGURATION, "Main Port: %d", opt.main_port);
	
	init_data();
	parse_game_files();
	parse_room_files();
	/* If the motd option is present, pre-read the file */
	if (motd_info.use_motd)
		motd_read_file();

	logfile_initialize();

#ifndef DEBUG
	daemon_init(argv[0], 0);
#endif

	/* FIXME: Need to set more signal handlers */	
#ifndef DEBUG
	signal(SIGPIPE, SIG_IGN);
#endif

	/* Create SERVER socket on main_port */
	main_sock = es_make_socket_or_die(ES_SERVER, opt.main_port, NULL);
	
	/* Start accepting connections */
	if (listen(main_sock, MAX_USERS) < 0)
		err_sys_exit("Error listening to socket");

	log_msg(GGZ_LOG_NOTICE,
		"GGZ server initialized and ready for player connections");

	/* Main loop */
	for (;;) {
		addrlen = sizeof(addr);
		if ( (new_sock = accept(main_sock, &addr, &addrlen)) < 0) {
			if (errno == EINTR)
				continue;
			else
				err_sys_exit("Error accepting connection");
		} else {
			/* This is where to test for ignored IP addresses */
			player_handler_launch(new_sock);
		}
	}

	/* FIXME: Do we need some sort of cleanup? */
	return 0;
}
