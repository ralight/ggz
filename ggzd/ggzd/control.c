/*
 * File: control.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 10/11/99
 * Desc: Control/Port-listener part of server
 * $Id: control.c 2536 2001-10-04 23:44:50Z rgade $
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

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <dirent.h>
#include <fcntl.h>

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
#include <daemon.h>
#include <hash.h>


/* Server options */
Options opt;

/* Main data structures */
struct GameInfo game_types[MAX_GAME_TYPES];
struct GGZState state;

/* Termination signal */
sig_atomic_t term_signal;

/* Termination handler */
RETSIGTYPE term_handle(int signum)
{
	term_signal = 1;
}


int make_path(char* full, mode_t mode)
{
	const char* slash = "/";
	char* copy, *dir, *path;
	struct stat stats;

	copy = strdup(full);

	/* FIXME: check validity */
	if ( (path = calloc(sizeof(char), strlen(full)+1)) == NULL)
		err_sys_exit("malloc failed in make_path");
	
	/* Skip preceding / */
	if (copy[0] == '/')
		copy++;

	while ((dir = strsep(&copy, slash))) {
		strcat(strcat(path, "/"), dir);
		if (mkdir(path, mode) < 0
		    && (stat(path, &stats) < 0 || !S_ISDIR(stats.st_mode))) {
			free(path);
			free(copy);
			return -1;
		}
	}

	return 0;
}


void init_dirs(void)
{
	DIR* dir;
	
	if ( (dir = opendir(opt.tmp_dir)) == NULL) {
		dbg_msg(GGZ_DBG_CONFIGURATION, 
			"Couldn't open %s -- trying to create", opt.tmp_dir);
		if (make_path(opt.tmp_dir, S_IRWXU) < 0)
			err_sys_exit("Couldn't create %s", opt.tmp_dir);
	}
	else /* Everything eas OK, so close it */
		closedir(dir);

	if ( (dir = opendir(opt.data_dir)) == NULL) {
		dbg_msg(GGZ_DBG_CONFIGURATION, 
			"Couldn't open %s -- trying to create", opt.data_dir);
		if (make_path(opt.data_dir, S_IRWXU) < 0)
			err_sys_exit("Couldn't create %s", opt.data_dir);
	}
	else /* Everything eas OK, so close it */
		closedir(dir);
}


/* Perhaps these should be put into their respective files? */
void init_data(void)
{
	int i;

	pthread_rwlock_init(&state.lock, NULL);

	for (i = 0; i < MAX_GAME_TYPES; i++) {
		pthread_rwlock_init(&game_types[i].lock, NULL);
	}

	if (ggzdb_init() < 0)
		err_msg_exit("*** Database initialization failed");

	hash_initialize();

	/* Initialize random numbers for password generation */
	srandom((unsigned)time(NULL));
}


int main(int argc, const char *argv[])
{
	int main_sock, new_sock, status, flags;
	socklen_t addrlen;
	struct sockaddr_in addr;
	fd_set active_fd_set, read_fd_set;
	struct timeval tv;

       	/* Parse options */
	parse_args(argc, argv);
	parse_conf_file();
	
	dbg_msg(GGZ_DBG_CONFIGURATION, "Conf file: %s", opt.local_conf);
	dbg_msg(GGZ_DBG_CONFIGURATION, "Log level: %0X", log_info.log_types);
	dbg_msg(GGZ_DBG_CONFIGURATION, "Main Port: %d", opt.main_port);
	
	init_dirs();
	init_data();
	parse_game_files();
	parse_room_files();
	/* If the motd option is present, pre-read the file */
	if (motd_info.use_motd)
		motd_read_file();

	logfile_initialize();

	if (!opt.foreground) {
		daemon_init();

		/* FIXME: use sigaction() */
		signal(SIGTERM, term_handle);
		signal(SIGINT, term_handle);
	}
	
	signal(SIGPIPE, SIG_IGN);

	/* Create SERVER socket on main_port */
	main_sock = es_make_socket_or_die(ES_SERVER, opt.main_port, NULL);
	
	/* Make socket non-blocking */
	if ( (flags = fcntl(main_sock, F_GETFL, 0)) < 0)
		err_sys_exit("F_GETFL error");
	flags |= O_NONBLOCK;
	if (fcntl(main_sock, F_SETFL, flags) < 0)
		err_sys_exit("F_SETFL error");

	/* Start accepting connections */
	if (listen(main_sock, MAX_USERS) < 0)
		err_sys_exit("Error listening to socket");

	log_msg(GGZ_LOG_NOTICE,
		"GGZ server initialized and ready for player connections");

	FD_ZERO(&active_fd_set);
	FD_SET(main_sock, &active_fd_set);

	/* Main loop */
	while (!term_signal) {

		read_fd_set = active_fd_set;
		tv.tv_sec = log_next_update_sec();
		tv.tv_usec = 0;
		status = select((main_sock + 1), &read_fd_set, NULL, NULL, &tv);

		if (status < 0) {
			if (errno == EINTR)
				continue;
			else
				err_sys_exit("select error");
		} else if(status == 0) {
			log_generate_update();
			continue;
		}

		addrlen = sizeof(addr);
		if ( (new_sock = accept(main_sock, (struct sockaddr*)&addr, &addrlen)) < 0) {
			switch (errno) {
			case EWOULDBLOCK:
			case ECONNABORTED:
			case EINTR:
				continue;
				break;
			default:
				err_sys_exit("Error accepting connection");
			}
		} else {
			/* This is where to test for ignored IP addresses */
			player_handler_launch(new_sock);
		}
	}

	log_msg(GGZ_LOG_NOTICE, "GGZ server received termination signal");

	/* FIXME: do we need to stop all of threads? */
	ggzdb_close();

	if (!opt.foreground) {
		daemon_cleanup();
	}

	return 0;
}
