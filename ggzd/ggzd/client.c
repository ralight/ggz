/*
 * File: client.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 4/26/02
 * Desc: Functions for handling client connections
 * $Id: client.c 4575 2002-09-16 04:36:11Z jdorje $
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA */


#include "config.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "client.h"
#include "err_func.h"
#include "hash.h"
#include "net.h"
#include "table.h"


/* Server wide data structures*/
extern struct GameInfo game_types[MAX_GAME_TYPES];
extern struct GGZState state;
extern Options opt;

/* Used for banned IP addresses */
static uint32_t *banned_nets_a;
static uint32_t *banned_nets_m;
static int banned_nets_c;
static GGZReturn client_check_ip_ban_list(struct in_addr *sin);
static void* client_thread_init(void *arg_ptr);
static GGZClient* client_new(int fd);
static void client_loop(GGZClient* client);
static void client_free(GGZClient *client);
static void client_create_channel(GGZClient *client);

/* Timeout for server resync */
#define GGZ_RESYNC_SEC  0
#define GGZ_RESYNC_USEC 500000


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
	struct sockaddr_in addr;
	GGZClient *client;
	int addrlen = sizeof(addr);
	struct hostent hostbuf, *hp;
	char tmphstbuf[1024];
	int rc, herr;

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
	
	/* Get the client's IP address and store it */
	getpeername(sock, (struct sockaddr*)&addr, &addrlen);

	rc = -1;
	if (opt.perform_lookups) {
		rc = gethostbyaddr_r(&addr.sin_addr, sizeof(struct in_addr),
				     AF_INET, &hostbuf, tmphstbuf, 1024, &hp, &herr);
		/* Note if we get an error we don't bother expanding the */
		/* buffer or so forth. Is the hostname vs. IP that important? */
		if(rc == 0) {
			strncpy(client->addr, hostbuf.h_name, sizeof(client->addr));
			client->addr[sizeof(client->addr)-1] = '\0';
		}
	}
	if (rc < 0)
		inet_ntop(AF_INET, &addr.sin_addr, client->addr, sizeof(client->addr));
 
	if (client_check_ip_ban_list(&addr.sin_addr) != GGZ_OK) {
		log_msg(GGZ_LOG_SECURITY,"IPBAN Connection not allowed from %s",
			client->addr);
		client_free(client);
		pthread_exit(NULL);
	}
	
	/* Send server ID */
	if (net_send_serverid(client->net, opt.server_name) < 0) {
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
#if 1
	net_set_dump_file(client->net, "ggzd.protocol");
#endif

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
	int status = 0, fd_max, fd;
	fd_set active_fd_set, read_fd_set;
	struct timeval timer;

	/* Get socket from netIO object */
	fd = net_get_fd(client->net);
	FD_ZERO(&active_fd_set);
	FD_SET(fd, &active_fd_set);
	
	while (!client->session_over) {

		/* Process player events */
		if (client->type == GGZ_CLIENT_PLAYER) {
			if ( (status = player_updates(client->data)) < 0)
				break;
		}

		read_fd_set = active_fd_set;
		fd_max = fd + 1;
		
		/* Setup timeout for select*/
		timer.tv_sec = GGZ_RESYNC_SEC;
		timer.tv_usec = GGZ_RESYNC_USEC;
		
		status = select(fd_max, &read_fd_set, NULL, NULL, &timer);
		
		if (status <= 0) {
			if (status == 0 || errno == EINTR)
				continue;
			else
				err_sys_exit("select error in client_loop()");
		}

		/* Check for message from client */
		if (FD_ISSET(fd, &read_fd_set)) {
			if ( (status = net_read_data(client->net)) < 0)
				break;
		}
		
	} /* while (!client->session_over) */

	dbg_msg(GGZ_DBG_CONNECTION, "Client loop finished");

	if (client->type == GGZ_CLIENT_PLAYER) {
		player_logout(client->data);
		net_disconnect(client->net);
	}
	
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
		break;
	case GGZ_CLIENT_CHANNEL:
		/* FIXME: do something here */
		break;
	default:
		break;
	}
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
	if (client->data)
		ggz_free(client->data);
	ggz_free(client);
}


void client_set_ip_ban_list(int count, char **list)
{
	int i;
	char *addr, *mask;
	struct in_addr sin;

	if((banned_nets_c = count) == 0)
		return;

	banned_nets_a = ggz_malloc(count * sizeof(uint32_t));
	banned_nets_m = ggz_malloc(count * sizeof(uint32_t));

	/* Convert the list of IP/Mask specs to internal format */
	/* If it's invalid, just make the address part all zeros */
	for(i=0; i<count; i++) {
		addr = list[i];
		for(mask=addr; *mask!='\0' && *mask!='/'; mask++)
			;
		if(*mask == '\0')
			mask = "255.255.255.255";
		else {
			*mask = '\0';
			mask++;
		}
		if(inet_aton(addr, &sin) != 0) {
			banned_nets_a[i] = sin.s_addr;
			if(inet_aton(mask, &sin) != 0)
				banned_nets_m[i] = sin.s_addr;
			else
				banned_nets_a[i] = 0;
		} else
			banned_nets_a[i] = 0;
	}
}


static GGZReturn client_check_ip_ban_list(struct in_addr *sina)
{
	int i;
	for(i=0; i<banned_nets_c; i++) {
		if(banned_nets_a[i] != 0
                   && (sina->s_addr & banned_nets_m[i]) == banned_nets_a[i])
			return GGZ_ERROR;
	}

	return GGZ_OK;
}
