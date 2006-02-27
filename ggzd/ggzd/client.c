/*
 * File: client.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 4/26/02
 * Desc: Functions for handling client connections
 * $Id: client.c 7878 2006-02-27 12:16:13Z josef $
 *
 * Desc: Functions for handling players.  These functions are all
 * called by the player handler thread.  Since this thread is the only
 * one which can write to the player structure we do not need to lock
 * it when reading from it.
 *
 * Copyright (C) 2002 Brent Hendricks.
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
# include <config.h>		/* Site specific config */
#endif

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "client.h"
#include "err_func.h"
#include "hash.h"
#include "net.h"
#include "table.h"
#include "util.h"


static void* client_thread_init(void *arg_ptr);
static GGZClient* client_new(int fd);
static void client_loop(GGZClient* client);
static void client_free(GGZClient *client);
static void client_create_channel(GGZClient *client);

/** 
 * Launches a new dedicated thread for handling this client connection 
 *
 * @param sock : fd for communicating with the client
 */
void client_handler_launch(int sock)
{
	pthread_t thread;
	int *arg_ptr;
	int status;

	/* Temporary storage to pass fd.  This must be malloc'd so that it
	   will stay intact.  It is deallocated at the beginning of
	   client_thread_init. */
	arg_ptr = ggz_malloc(sizeof(*arg_ptr));
	*arg_ptr = sock;

	status = pthread_create(&thread, NULL, client_thread_init, arg_ptr);
	if (status != 0) {
		errno = status;
		err_sys_exit("pthread_create error");
	}
}


/** 
 * Initial function executed by a new client handler thread.  It sends
 * the initial server messages to the client, and creates the new
 * client structure.
 *
 * @param arg_ptr pointer to the client fd.
 *
 * @return thread exit status (NULL on success) */
static void* client_thread_init(void *arg_ptr)
{
	int sock, status;
	GGZClient *client;
	const char *tmp;

	/* Get our arguments out of the arg buffer */
	sock = *((int *)arg_ptr);
	ggz_free(arg_ptr);

	/* Detach thread since no one needs to join us */
	status = pthread_detach(pthread_self());
	if (status != 0) {
		errno = status;
		err_sys_exit("pthread_detach error");
	}

	/* Initialize client data */
	client = client_new(sock);

	/* Find out where the client comes from */
	tmp = ggz_getpeername(sock, opt.perform_lookups);
	if (!tmp) {
		client->addr[0] = '\0';
	} else {
		strncpy(client->addr, tmp, sizeof(client->addr));
	}
	ggz_free(tmp);

	/* Send server ID */
	if (net_send_serverid(client->net, opt.server_name, opt.tls_use) < 0) {
		client_free(client);
		pthread_exit(NULL);
	}
	
	pthread_rwlock_wrlock(&state.lock);
	if (state.players == MAX_USERS) {
		pthread_rwlock_unlock(&state.lock);
		net_send_server_full(client->net, opt.server_name);
		log_msg(GGZ_LOG_NOTICE,
			"SERVER_FULL - Rejected connection from %s",
			client->addr);
		client_free(client);
		pthread_exit(NULL);
	}
	state.players++;
	pthread_rwlock_unlock(&state.lock);

	dbg_msg(GGZ_DBG_CONNECTION, "New client connected from %s", 
		client->addr);

	/* FIXME: use a new file for each client */

	net_set_dump_file(client->net, opt.dump_file);

	/* Main client event loop */
	client_loop(client);
	client_free(client);

	pthread_rwlock_wrlock(&state.lock);
	state.players--;
	pthread_rwlock_unlock(&state.lock);

	return (NULL);
}


/**
 * Create and initialize new GGZClient structure.
 *
 * @param sock: file descriptor of client's connection
 *
 * @return pointer to new GGZClient object
 */
GGZClient* client_new(int fd)
{
	GGZClient *client;

	/* Allocate new client structure */
	client = ggz_malloc(sizeof(GGZClient));

	client->type = GGZ_CLIENT_GENERIC;
	client->net = net_new(fd, client);
	client->thread = pthread_self();
	client->data = NULL;
	client->session_over = 0;

	return client;
}


/**
 * Main client event loop.  
 *
 * @param client : pointer to client structure managed by this thread
 */
static void client_loop(GGZClient* client)
{
	int status = 0, fd;
	fd_set active_fd_set;
	sigset_t set;

	/* Get socket from netIO object */
	fd = net_get_fd(client->net);
	FD_ZERO(&active_fd_set);
	FD_SET(fd, &active_fd_set);

	sigemptyset(&set);
	sigaddset(&set, PLAYER_EVENT_SIGNAL);

	while (!client->session_over) {
		fd_set read_fd_set = active_fd_set;
		struct timeval timer, *select_tv;
		GGZPlayer *player = client->data; /* If GGZ_CLIENT_PLAYER. */

		/* Figure out how long until the next polling action.
		 *
		 * For players, polls include pings and room updates.  These
		 * are timed accurately to the nearest second.  For other
		 * client types, there are no polls. */
		if (client->type == GGZ_CLIENT_PLAYER) {
			ggztime_t now = get_current_time();
			ggztime_t wait_time;

			if (!player->is_ping_sent) {
				/* How long til we send the next ping? */
				wait_time = player->next_ping_time - now;
			} else if (player->lag_class < 5) {
				/* Ping in progress */
				wait_time = player->sent_ping_time
				  + opt.lag_class_time[player->lag_class - 1]
				  - now;
			} else {
				wait_time = 10000;
			}
			if (player->is_room_update_needed) {
				wait_time = MIN(wait_time,
					player->next_room_update_time - now);
			}

			/* Never wait a negative amount of time! */
			wait_time = MAX(wait_time, 0);

			/* Having done the exact calculations, we now wait
			 * just a little longer to make sure we wait
			 * at least long enough. */
			wait_time += 0.01;

			timer = ggztime_to_timeval(wait_time);

			select_tv = &timer;
		} else {
			select_tv = NULL;
		}

		/*
		 * FIXME: This implementation won't scale well.
		 *
		 * Select is fundamentally an O(fd_max) operation.  With an
		 * increasing number of players this means each select in
		 * each player thread will take O(players) time, and thus
		 * ggzd overall will take O(players^2) time for each step.
		 *
		 * This is a problem for the other select calls (in table.c
		 * and control.c) as well, but in all cases the limiting
		 * factor is the number of players.  Note it is only a
		 * problem because we have one thread and one select call
		 * per player.
		 *
		 * Fixing this may be as simple as using a different function
		 * to monitor the socket.  Since we're only watching one
		 * socket it should be easy!  Another possibility is
		 * distributed GGZ: where multiple GGZD processes, possibly
		 * running on different machines, interoperate to provide
		 * one "server".
		 *
		 * Since the actual overhead of select is very small, I
		 * suspect we won't have a problem with this unless there
		 * are a _lot_ of players.
		 */
		status = select(fd + 1, &read_fd_set, NULL, NULL, select_tv);
		if (status <= 0) {
			if (status != 0 && errno != EINTR)
				err_sys_exit("select error in client_loop()");
		}

		/* Check for message from client */
		if (status > 0 && FD_ISSET(fd, &read_fd_set)) {
			pthread_sigmask(SIG_BLOCK, &set, NULL);
			if (net_read_data(client->net) == GGZ_REQ_DISCONNECT
			    || client->session_over) {
				/* Break with the signals still blocked. */
				break;
			}
			pthread_sigmask(SIG_UNBLOCK, &set, NULL);
		}

		/* Process player events */
		if (client->type == GGZ_CLIENT_PLAYER) {
			pthread_kill(pthread_self(), PLAYER_EVENT_SIGNAL);
		}
	} /* while (!client->session_over) */

	/* Don't want signals interrupting after this point */
	pthread_sigmask(SIG_BLOCK, &set, NULL);

	dbg_msg(GGZ_DBG_CONNECTION, "Client loop finished");

	/* Force disconnect, but log out players first */
	if (client->type == GGZ_CLIENT_PLAYER) {
		player_logout(client->data);
	}
	net_disconnect(client->net);

	return;
}


void client_create_channel(GGZClient *client)
{
	GGZPlayer *player;
	int fd;

	dbg_msg(GGZ_DBG_CONNECTION, "Forming direct game connection");

	/* FIXME: this is just a proof of concept hack */
	dbg_msg(GGZ_DBG_CONNECTION, "Direct ID: %s", (char*)client->data);
	fd = net_get_fd(client->net);
	player = hash_player_lookup(client->data);
	if (player) {
		dbg_msg(GGZ_DBG_CONNECTION, "Found player %s", player->name);
		player->game_fd = fd;
		pthread_rwlock_unlock(&player->lock);
		dbg_msg(GGZ_DBG_CONNECTION, "Direct connection on fd %d", fd);
	}
	else 
		dbg_msg(GGZ_DBG_CONNECTION, "No player %s", (char*)client->data);
}


void client_set_type(GGZClient *client, GGZClientType type)
{
	client->type = type;
	switch (type) {
	case GGZ_CLIENT_PLAYER:
		client->data = player_new(client);
		pthread_setspecific(player_key, client->data);
		return;

	case GGZ_CLIENT_CHANNEL:
		/* FIXME: do something here */
		return;

	case GGZ_CLIENT_GENERIC:
		break;
	}

	/* If we get here, it's an error. */
	err_msg("client_set_type: illegal type %d.", type);
}


void client_end_session(GGZClient *client)
{
	/* If it was a CHANNEL connection, create the channel before ending */
	if (client->type == GGZ_CLIENT_CHANNEL) {
		client_create_channel(client);
	}
	client->session_over = 1;
}


static void client_free(GGZClient *client)
{
	net_free(client->net);
	switch (client->type) {
	case GGZ_CLIENT_PLAYER:
		player_free(client->data);
		break;
	case GGZ_CLIENT_CHANNEL:
		/* The data is allocated in _net_handle_channel */
		ggz_free(client->data);
		break;
	case GGZ_CLIENT_GENERIC:
		/* nothing */
		break;
	}
	ggz_free(client);
}

