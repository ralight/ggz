/*
 * File: control.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 10/11/99
 * Desc: Control/Port-listener part of server
 * $Id: control.c 8779 2007-01-02 12:14:04Z josef $
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
#include <string.h>

#include <ggz.h>

#ifdef WITH_HOWL
#include <howl.h>
#endif
#ifdef WITH_AVAHI
#include <avahi-common/thread-watch.h>
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
#include "reconfiguration.h"
#include "room.h"
#include "table.h"
#include "util.h"
#include "meta.h"

#ifdef HAVE_INOTIFY
#include <sys/ioctl.h>
#endif
#ifdef WITH_FAM
#include <fam.h>
#endif

/* FIXME: this is in motd.c also - and we could probably use pathconf */
#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 256
#endif

/* Server options */
Options opt;

/* Main data structures */
struct GameInfo game_types[MAX_GAME_TYPES];
struct GGZState state;

/* Termination signal */
static sig_atomic_t hup_signal;
static sig_atomic_t term_signal;

/* Reconfiguration watcher */
static int reconfigure_fd;
#ifdef WITH_FAM
FAMConnection fc;
#endif

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
	/* not needed currently since anonymous sockets are used */
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
#define data_free(ptr)		   \
  	do {			   \
		ggz_free(ptr);     \
		ptr = NULL;        \
	} while(0)

	int i;
	char **args;

	/* FIXME: must destroy all threads first */
	if (term_signal || !hup_signal) return;

	motd_read_file(NULL);

	ggz_memory_check();

	if (opt.local_conf) data_free(opt.local_conf);
	if (opt.interface) data_free(opt.interface);
	if (opt.game_exec_dir) data_free(opt.game_exec_dir);
	if (opt.conf_dir) data_free(opt.conf_dir);
	if (opt.data_dir) data_free(opt.data_dir);
	if (opt.motd_file) data_free(opt.motd_file);
	if (opt.motd_web) data_free(opt.motd_web);
	if (opt.admin_name) data_free(opt.admin_name);
	if (opt.admin_email) data_free(opt.admin_email);
	if (opt.server_name) data_free(opt.server_name);

	if (opt.dbhost) data_free(opt.dbhost);
	if (opt.dbname) data_free(opt.dbname);
	if (opt.dbusername) data_free(opt.dbusername);
	if (opt.dbpassword) data_free(opt.dbpassword);
	if (opt.dbhashing) data_free(opt.dbhashing);
	if (opt.dbhashencoding) data_free(opt.dbhashencoding);

	if (opt.tls_key) data_free(opt.tls_key);
	if (opt.tls_cert) data_free(opt.tls_cert);
	if (opt.tls_password) data_free(opt.tls_password);

	if (opt.dump_file) data_free(opt.dump_file);

	if (opt.announce_metaserver) data_free(opt.announce_metaserver);
	if (opt.metausername) data_free(opt.metausername);
	if (opt.metapassword) data_free(opt.metapassword);

	/* We don't bother with locking anything... */
	for (i = 0; i < room_info.num_rooms; i++) {
		data_free(rooms[i].room);
		data_free(rooms[i].name);
		data_free(rooms[i].description);
		data_free(rooms[i].players);
		if (rooms[i].max_tables > 0)
			data_free(rooms[i].tables);
		if (rooms[i].exec_args) {
			for (args = rooms[i].exec_args; *args; args++)
				data_free(*args);
			data_free(rooms[i].exec_args);
		}
	}
	data_free(rooms);

	for (i = 0; game_types[i].exec_args && i < MAX_GAME_TYPES; i++) {
		data_free(game_types[i].version);
		data_free(game_types[i].p_engine);
		data_free(game_types[i].p_version);
		data_free(game_types[i].homepage);
		data_free(game_types[i].game);
		data_free(game_types[i].data_dir);
		data_free(game_types[i].desc);
		data_free(game_types[i].author);
		for (args = game_types[i].exec_args; *args; args++)
			data_free(*args);
		data_free(game_types[i].exec_args);

		if (game_types[i].named_bots){
			char ***args_nb;

			for (args_nb = game_types[i].named_bots;
			     *args_nb;
			     args_nb++){
				data_free((*args_nb)[1]);
				data_free((*args_nb)[0]);
				data_free((*args_nb));
			}
			data_free(game_types[i].named_bots);
		}
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
	/* FIXME: this is not used yet (link status changes etc.) */
	fprintf(stderr, "Zeroconf CALLBACK!\n");
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
	AvahiThreadedPoll *threadedpoll;
	const AvahiPoll *poll;

	threadedpoll = avahi_threaded_poll_new();
	poll = avahi_threaded_poll_get(threadedpoll);

	client = avahi_client_new(poll, AVAHI_CLIENT_NO_FAIL, NULL, NULL, NULL);
	if(!client)
	{
		fprintf(stderr, "Zeroconf: Error: could not initialize\n");
		return -1;
	}

	group = avahi_entry_group_new(client, callback, NULL);
	if(!group)
	{
		fprintf(stderr, "Zeroconf: Error: could not publish\n");
		return -1;
	}

	avahi_entry_group_add_service(group, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, 0,
		name, protocol, NULL, NULL, port, NULL);
	avahi_entry_group_commit(group);

	log_msg(GGZ_LOG_NOTICE,
		"Zeroconf: GGZ server is now known to the LAN");

	/* ...this is needed to activate the callback, for advanced zeroconf... */
	/*avahi_threaded_poll_start(threadedpoll);*/

	/* FIXME: ...this must be done before shutting down ggzd... */
	/*
	avahi_threaded_poll_stop(threadedpoll); -- if poll_start() called above
	avahi_entry_group_free(group);
	avahi_client_free(client);
	avahi_threaded_poll_free(threadedpoll);
	*/

	return 0;
#else
	fprintf(stderr, "Zeroconf: Error: server does not support zeroconf\n");
	return -1;
#endif
#endif
}

/* FIXME: split out room part to support room reconfiguration */
static void meta_announce(const char *metaserveruri, const char *username, const char *password)
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

static void reconfiguration_setup(void)
{
	char watchdir[strlen(opt.conf_dir) + 8];

	snprintf(watchdir, sizeof(watchdir), "%s/rooms", opt.conf_dir);

#ifdef HAVE_INOTIFY
	reconfigure_fd = inotify_init();
	if(reconfigure_fd <= 0)
	{
		fprintf(stderr, "Reconfiguration: Error: initialization failed\n");
		return;
	}

	/* We don't want to inherit this to game servers! */
	fcntl(reconfigure_fd, F_SETFD, FD_CLOEXEC);

	/* Test - FIXME: add etc/ggzd/rooms? */
	inotify_add_watch(reconfigure_fd, watchdir, IN_DELETE | IN_CLOSE_WRITE);

	log_msg(GGZ_LOG_NOTICE,
		"Reconfiguration: watching rooms directory for changes (%s)", watchdir);
#else
#ifdef WITH_FAM
	FAMRequest fr;
	if(FAMOpen(&fc) < 0)
	{
		fprintf(stderr, "Reconfiguration: Error: initialization failed\n");
		reconfigure_fd = -1;
		return;
	}

	reconfigure_fd = fc.fd;

	if(FAMMonitorDirectory(&fc, watchdir, &fr, NULL) < 0)
	{
		fprintf(stderr, "Reconfiguration: Error: monitoring failed\n");
		return;
	}

	log_msg(GGZ_LOG_NOTICE,
		"Reconfiguration: watching rooms directory for changes (%s)", watchdir);
#else
	reconfigure_fd = -1;
#endif
#endif
}

static void reconfiguration_handle(void)
{
#ifdef HAVE_INOTIFY
	/* FIXME: and now? need to read data! */
	int pending;
	char buf[4096];
	struct inotify_event ev;
	int offset;
	int diff;
	char *filename;

	offset = 0;

	ioctl(reconfigure_fd, FIONREAD, &pending);
	while(pending > 0)
	{
		if (pending > (int)sizeof(buf)) pending = sizeof(buf);
		pending = read(reconfigure_fd, buf, pending);

		while(pending > 0)
		{
			memcpy(&ev, &buf[offset], INOTIFY_EVENTSIZE);
			filename = ggz_malloc(ev.len + 1);
			memcpy(filename, &buf[offset + INOTIFY_EVENTSIZE], ev.len);

			diff = INOTIFY_EVENTSIZE + ev.len;
			pending -= diff;
			offset += diff;

			dbg_msg(GGZ_DBG_MISC, "* inotify: file %s mask %i", filename, ev.mask);

			if(strncmp(filename + strlen(filename) - 5, ".room", 5))
			{
				ggz_free(filename);
				continue;
			}

			/* now we've got the filename (and we know the mode) */
			if(ev.mask == IN_CLOSE_WRITE)
			{
				log_msg(GGZ_LOG_NOTICE,
					"Reconfiguration: room addition %s",
					filename);
				parse_room_change(filename);
			}
			else if(ev.mask == IN_DELETE)
			{
				log_msg(GGZ_LOG_NOTICE,
					"Reconfiguration: room deletion %s",
					filename);
				parse_room_change(filename);
			}
			else
			{
				/* ignore, we haven't ordered this */
			}

			ggz_free(filename);
		}
	}
#else
#ifdef WITH_FAM
	FAMEvent fe;
	while(FAMPending(&fc))
	{
		if(FAMNextEvent(&fc, &fe) < 0)
		{
			fprintf(stderr, "Reconfiguration: Error: handling failed\n");
			return;
		}

		dbg_msg(GGZ_DBG_MISC, "* fam: file %s code %i", fe.filename, fe.code);

		if(strncmp(fe.filename + strlen(fe.filename) - 5, ".room", 5))
		{
			continue;
		}

		if(fe.code == FAMCreated)
		{
			log_msg(GGZ_LOG_NOTICE, "Reconfiguration: room addition %s",
				fe.filename);
			parse_room_change(fe.filename);
		}
		else if(fe.code == FAMDeleted)
		{
			log_msg(GGZ_LOG_NOTICE, "Reconfiguration: room removal %s",
				fe.filename);
			parse_room_change(fe.filename);
		}
		else
		{
			/* ignore */
		}
	}
#endif
#endif
}


int main(int argc, char *argv[])
{
	int main_sock, new_sock, status, flags;
	int select_max;
	socklen_t addrlen;
	struct sockaddr_in addr;
	fd_set active_fd_set, read_fd_set;
	struct timeval tv, *tvp;
	int seconds;

	logfile_preinitialize();

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

	/* Watch configuration changes */
	if(opt.reconfigure_rooms)
		reconfiguration_setup();
	else
		reconfigure_fd = -1;

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
	select_max = main_sock;
	if (reconfigure_fd > 0) {
		FD_SET(reconfigure_fd, &active_fd_set);
		select_max = MAX(select_max, reconfigure_fd);
	}

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
		seconds = log_next_update_sec();
		if(seconds == 0)
		{
			tvp = NULL;
		}
		else
		{
			tv.tv_sec = seconds;
			tv.tv_usec = 0;
			tvp = &tv;
		}
		status = select((select_max + 1), &read_fd_set, NULL, NULL, tvp);

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

		if(FD_ISSET(main_sock, &read_fd_set)) {
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
		} else if(reconfigure_fd > 0) {
			if(FD_ISSET(reconfigure_fd, &read_fd_set)) {
				dbg_msg(GGZ_DBG_MISC, "reconfiguration monitor activated");
				reconfiguration_handle();
			}
		}
	}

	log_msg(GGZ_LOG_NOTICE, "GGZ server terminating");

	/* FIXME: do we need to stop all of threads? */
	ggzdb_close();

#ifdef WITH_FAM
	/*FAMClose(&fc);*/
#endif

	if (!opt.foreground) {
		/* This must come AFTER ggzdb_close but BEFORE cleanup_data. */
		daemon_cleanup();
	} else {
		printf("GGZ Server: terminating...\n");
	}

	cleanup_data(); /* FIXME: must destroy all threads first */

	return 0;
}
