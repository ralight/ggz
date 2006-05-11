/*
 * File: control.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 10/11/99
 * Desc: Control/Port-listener part of server
 * $Id: control.c 8034 2006-05-11 10:20:38Z josef $
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

#ifdef HAVE_CONFIG_H
# include <config.h>		/* Site specific config */
#endif

#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include <ggz.h>

#ifdef WITH_HOWL
#include <howl.h>
#endif
#ifdef WITH_AVAHI
#include <avahi-common/simple-watch.h>
#include <avahi-client/publish.h>
#endif

#include "client.h"
#include "chat.h"
#include "daemon.h"
#include "datatypes.h"
#include "err_func.h"
#include "ggzd.h"
#include "ggzdb.h"
#include "hash.h"
#include "motd.h"
#include "parse_opt.h"
#include "players.h"
#include "room.h"
#include "table.h"
#include "util.h"
#include "meta.h"


/* Server options */
Options opt;

/* Main data structures */
struct GameInfo game_types[MAX_GAME_TYPES];
struct GGZState state;

/* Termination signal */
static sig_atomic_t hup_signal;
static sig_atomic_t term_signal;

/* Hangup handler.  Eventually admins will be able to raise() this
   over the network. */
static RETSIGTYPE hup_handle(int signum)
{
	/* Don't repeat the HUP message. */
	if (hup_signal) return;

	hup_signal = 1;
	log_msg(GGZ_LOG_NOTICE, "Server received HUP");
	chat_server_2_player(NULL,
			     "This server is shutting down.  No new "
			     "players will be able to join it.");
}

/* Termination handler */
static RETSIGTYPE term_handle(int signum)
{
	term_signal = 1;
}


/*
 * Make sure all of our needed directories exist.  The first time
 * running, they'll probably need to be created.
 */
static void init_dirs(void)
{
	check_path(opt.tmp_dir);
	check_path(opt.data_dir);
}

/* Perhaps these should be put into their respective files? */
static void init_data(void)
{
	pthread_rwlock_init(&state.lock, NULL);

	if (ggzdb_init() != GGZ_OK)
		err_msg_exit("*** Database initialization failed");

	hash_initialize();

	/* Initialize random numbers for password generation */
	srandom((unsigned)time(NULL));
}

static void cleanup_data(void)
{
#define data_free(ptr) \
  do {                 \
    ggz_free(ptr);     \
    ptr = NULL;        \
  } while(0)

	int i;

	/* FIXME: must destroy all threads first */
	if (term_signal || !hup_signal) return;

	motd_read_file(NULL);

	data_free(opt.game_dir);
	data_free(opt.tmp_dir);
	data_free(opt.conf_dir);
	data_free(opt.data_dir);
	if (opt.motd_file)
		data_free(opt.motd_file);
	data_free(opt.admin_name);
	data_free(opt.admin_email);
	data_free(opt.server_name);

	data_free(opt.dbhost);
	data_free(opt.dbname);
	data_free(opt.dbusername);
	data_free(opt.dbpassword);
	data_free(opt.dbhashing);

	/* We don't bother with locking anything... */
	for (i = 0; i < room_info.num_rooms; i++) {
		data_free(rooms[i].name);
		data_free(rooms[i].description);
		data_free(rooms[i].players);
		if (rooms[i].max_tables > 0)
			data_free(rooms[i].tables);
	}
	data_free(rooms);

	for (i = 0; game_types[i].name && i < MAX_GAME_TYPES; i++) {
		char **args;
		data_free(game_types[i].version);
		data_free(game_types[i].p_engine);
		data_free(game_types[i].p_version);
		data_free(game_types[i].data_dir);
		data_free(game_types[i].desc);
		data_free(game_types[i].author);
		data_free(game_types[i].homepage);
		for (args = game_types[i].exec_args; *args; args++)
			data_free(*args);
		data_free(game_types[i].exec_args);
	}

	data_free(log_info.log_fname);
#ifdef DEBUG
	data_free(log_info.dbg_fname);
#endif

	ggz_conf_cleanup();
	ggz_debug_cleanup(GGZ_CHECK_MEM);

	/* No more debug logging after this! */
#undef data_free
}

#ifdef WITH_AVAHI
static void callback(AvahiEntryGroup *g, AvahiEntryGroupState state, void* userdata)
{
	fprintf(stderr, "CALLBACK!\n");
}
#endif

static int zeroconf_publish(const char *name, const char *protocol, int port)
{
#ifdef WITH_HOWL
	sw_discovery session;
	sw_discovery_oid oid;
	int ret;

	ret = sw_discovery_init(&session);
	if(ret != SW_OKAY)
	{
		fprintf(stderr, "Zeroconf: Error: could not initialize\n");
		return -1;
	}

	ret = sw_discovery_publish(session, 0, name, protocol, NULL, NULL,
		port, NULL, 0, NULL, NULL, &oid);
	if(ret != SW_OKAY)
	{
		fprintf(stderr, "Zeroconf: Error: could not publish\n");
		return -1;
	}
	else
	{
		log_msg(GGZ_LOG_NOTICE,
			"Zeroconf: GGZ server is now known to the LAN");
	}

	return 0;
#else
#if WITH_AVAHI
	AvahiClient *client;
	AvahiEntryGroup *group;
	AvahiSimplePoll *simplepoll;
	const AvahiPoll *poll;

	/*
	poll.userdata = NULL;
	poll.watch_new = NULL;
	poll.watch_update = NULL;
	poll.watch_get_events = NULL;
	poll.watch_free = NULL;
	poll.timeout_new = NULL;
	poll.timeout_update = NULL;
	poll.timeout_free = NULL;
	*/

	simplepoll = avahi_simple_poll_new();
	poll = avahi_simple_poll_get(simplepoll);
	fprintf(stderr, "DEBUG: POLL: %p\n", poll);

	/* FIXME: avahi_simple_poll_free() */

	client = avahi_client_new(poll, AVAHI_CLIENT_NO_FAIL, NULL, NULL, NULL);
	if(!client)
	{
		fprintf(stderr, "Zeroconf: Error: could not initialize\n");
		return -1;
	}

	fprintf(stderr, "DEBUG: CLIENT: %p\n", client);
	group = avahi_entry_group_new(client, callback, NULL);
	fprintf(stderr, "DEBUG: GROUP: %p\n", group);
	avahi_entry_group_add_service(group, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, 0,
		name, protocol, NULL, NULL, port, NULL);
	avahi_entry_group_commit(group);
	avahi_entry_group_free(group);
	avahi_client_free(client);

	return 0;
#else
	fprintf(stderr, "Zeroconf: Error: server does not support zeroconf\n");
	return -1;
#endif
#endif
}

void meta_announce(const char *metaserveruri, const char *username, const char *password)
{
	URI uri;
	ServerEntry *server;
	char hostname[HOST_NAME_MAX + 1];
	char roomname[128];
	int ret;
	int i;
	int type;
	char *game;
	char *gameversion;

	gethostname(hostname, sizeof(hostname));

	uri.protocol = "ggz";
	uri.host = hostname;
	uri.user = NULL;
	uri.port = 5688;
	uri.path = NULL;

	server = meta_server_new(uri);

	meta_server_attribute(server, "version", VERSION);
	meta_server_attribute(server, "preference", "100");
	meta_server_attribute(server, "location", NULL);
	meta_server_attribute(server, "speed", "0");

	ret = meta_add(server, "ggz", "connection",
		metaserveruri, username, password);
	if(!ret) fprintf(stderr, "Metaserver: Error: publishing failed\n");
	else log_msg(GGZ_LOG_NOTICE,
		"Metaserver: GGZ server is now known to the metaserver");

	meta_server_free(server);

	for(i = 0; i < room_info.num_rooms; i++)
	{
		RoomStruct room = rooms[i];
		type = room.game_type;
		if(type != -1)
		{
			GameInfo gametype = game_types[type];
			game = gametype.name;
			gameversion = gametype.version;
		}
		else
		{
			game = "ggz";
			gameversion = VERSION;
		}

		snprintf(roomname, sizeof(roomname), "/#%s", room.name);

		uri.protocol = "ggz";
		uri.host = hostname;
		uri.user = NULL;
		uri.port = 5688;
		uri.path = roomname;

		server = meta_server_new(uri);

		meta_server_attribute(server, "description", room.description);
		meta_server_attribute(server, "version", gameversion);

		ret = meta_add(server, game, "gameroom",
			metaserveruri, username, password);
		if(!ret) fprintf(stderr,
			"Metaserver: Error: publishing failed for room %s\n", room.name);

		meta_server_free(server);
	}
	log_msg(GGZ_LOG_NOTICE,
		"Metaserver: GGZ game rooms are now known to the metaserver");
}


int main(int argc, char *argv[])
{
	int main_sock, new_sock, status, flags;
	socklen_t addrlen;
	struct sockaddr_in addr;
	fd_set active_fd_set, read_fd_set;
	struct timeval tv;

	/* Parse options */
	parse_args(argc, argv);
	parse_conf_file();
	motd_read_file(opt.motd_file);

	dbg_msg(GGZ_DBG_CONFIGURATION, "Conf file: %s", opt.local_conf);
	dbg_msg(GGZ_DBG_CONFIGURATION, "Log level: %0X", log_info.log_types);
	dbg_msg(GGZ_DBG_CONFIGURATION, "Main Port: %d", opt.main_port);
	dbg_msg(GGZ_DBG_CONFIGURATION, "Encryption in use: %d", opt.tls_use);

	init_dirs();
	init_data();
	parse_game_files();
	parse_room_files();

	logfile_initialize();

	if (!opt.foreground)
		daemon_init();

	/* FIXME: use sigaction() */
	signal(SIGTERM, term_handle);
	signal(SIGINT, term_handle);
	signal(SIGHUP, hup_handle);
	signal(SIGPIPE, SIG_IGN);

	signal(PLAYER_EVENT_SIGNAL, player_handle_event_signal);
	pthread_key_create(&player_key, NULL);
	signal(TABLE_EVENT_SIGNAL, table_handle_event_signal);
	pthread_key_create(&table_key, NULL);

	/* Setup TLS */
	ggz_tls_init(opt.tls_cert, opt.tls_key, opt.tls_password);

	/* Announce server if requested */
	if(opt.announce_lan)
		zeroconf_publish("GGZ Gaming Zone", "_ggz._tcp.", opt.main_port);

	if(opt.announce_metaserver)
		meta_announce(opt.announce_metaserver, opt.metausername, opt.metapassword);

	/* Create SERVER socket on main_port */
	main_sock = ggz_make_socket(GGZ_SOCK_SERVER, opt.main_port, opt.interface);
	if (main_sock < 0) {
		fprintf(stderr, "Could not bind to port\n");
		err_msg_exit("Could not bind to port");
	}

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

	if (opt.foreground)
		printf("GGZ Server: started\n");

	FD_ZERO(&active_fd_set);
	FD_SET(main_sock, &active_fd_set);

	/* Main loop */
	while (!term_signal) {

		if (main_sock < 0) {
			/* After we receive a HUP, we've closed the main
			   socket so we no longer accept connections.  Now
			   we just loop until all players eventually leave. */
			int players;

			/* Is rd-locking necessary?  Or does the main
			   thread own this lock? */
			pthread_rwlock_rdlock(&state.lock);
			players = state.players;
			pthread_rwlock_unlock(&state.lock);

			if (players == 0) break;

			/* FIXME: it's bad to poll waiting for all players
			   to leave.  It's also bad to hard-code 5 seconds
			   here. */
			sleep(5);

			if (log_next_update_sec() <= 0)
				log_generate_update();

			continue;
		}

		read_fd_set = active_fd_set;
		tv.tv_sec = log_next_update_sec();
		tv.tv_usec = 0;
		status = select((main_sock + 1), &read_fd_set, NULL, NULL, &tv);

		if (status < 0) {
			if (errno == EINTR) {
				if (hup_signal && main_sock >= 0) {
					/* Once we get a HUP we want to ignore
					   new connections.  You might think
					   it would be clever to deflect new
					   connections with some sort of XML
					   message.  But actually we probably
					   just want to start up a new server
					   on the same port. */
					close(main_sock);
					main_sock = -1;
				}
				continue;
			} else
				err_sys_exit("select error in main()");
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
			client_handler_launch(new_sock);
		}
	}

	log_msg(GGZ_LOG_NOTICE, "GGZ server terminating");

	/* FIXME: do we need to stop all of threads? */
	ggzdb_close();

	if (!opt.foreground) {
		/* This must come AFTER ggzdb_close but BEFORE cleanup_data. */
		daemon_cleanup();
	} else {
		printf("GGZ Server: terminating...\n");
	}

	cleanup_data(); /* FIXME: must destroy all threads first */

	return 0;
}
