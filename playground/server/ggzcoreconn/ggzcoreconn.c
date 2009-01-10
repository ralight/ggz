/* This test tool has been derived from ggz-cmd. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include <ggz.h>
#include <ggzcore.h>

#define DBG_MAIN "ggzcoreconn"

typedef struct {
	char *uri;
	int tls;
	int async;
	int reconnect;
} GGZConnectionVariety;

static GGZServer *server = NULL;

static void wait_for_input(int sfd, int afd)
{
	fd_set my_fd_set;
	int status;
	struct timeval timeout = {.tv_sec = 10, .tv_usec = 0};

	FD_ZERO(&my_fd_set);
	if(sfd != -1)
		FD_SET(sfd, &my_fd_set);
	if(afd != -1)
		FD_SET(afd, &my_fd_set);

	int maxfd = sfd;
	if(afd > sfd)
		maxfd = afd;

	status = select(maxfd + 1, &my_fd_set, NULL, NULL, &timeout);
	if(status < 0)
		ggz_error_sys_exit("Select error while blocking.");

	if(sfd != -1) {
		if(!FD_ISSET(sfd, &my_fd_set)) {
			fprintf(stderr, "Connection to server timed out.\n");
			exit(-1);
		}
	}

	if(afd != -1) {
		GGZAsyncEvent ev = ggz_async_event();
		if(ev.type == GGZ_ASYNC_RESOLVER) {
			printf("ASYNC: Resolver returned address %s:%i\n", ev.address, ev.port);
		} else if(ev.type == GGZ_ASYNC_CONNECT) {
			printf("ASYNC: Connection established on socket %i\n", ev.socket);
		} else {
			printf("ASYNC: Noop\n");
		}
	}
}

static GGZHookReturn server_failure(GGZServerEvent id,
				    const void *event_data,
				    const void *user_data)
{
	const char *message = NULL;
	if ((id != GGZ_CONNECT_FAIL)
	&& (id != GGZ_NET_ERROR)) {
		const GGZErrorEventData *msg = event_data;
		message = msg->message;
	} else {
		message = event_data;
	}

	ggz_debug(DBG_MAIN, "GGZ failure: event %d.", id);
	fprintf(stderr,
		"ggz-cmd: Could not connect to server: %s\n", message);
	exit(-1);
}

static GGZHookReturn server_connected(GGZServerEvent id,
				      const void *event_data,
				      const void *user_data)
{
	ggz_debug(DBG_MAIN, "Connected to server.");
	return GGZ_HOOK_OK;
}

static GGZHookReturn server_negotiated(GGZServerEvent id,
				       const void *event_data,
				       const void *user_data)
{
	ggz_debug(DBG_MAIN, "Server negotiated.");
	ggzcore_server_login(server);
	return GGZ_HOOK_OK;
}

static GGZHookReturn server_logged_in(GGZServerEvent id,
				      const void *event_data,
				      const void *user_data)
{
	ggz_debug(DBG_MAIN, "Logged in to server.");

	ggzcore_server_list_rooms(server, 0);

	return GGZ_HOOK_OK;
}

#if 0
static GGZHookReturn server_room_entered(GGZServerEvent id,
					 const void *event_data,
					 const void *user_data)
{
	ggz_debug(DBG_MAIN, "Entered room 0.");
//	in_room = 1;
	return GGZ_HOOK_OK;
}
#endif

static void coreconnect(GGZConnectionVariety connvar)
{
	GGZConnectionPolicy policy;
	GGZLoginType logintype;

	if(connvar.tls)
		policy = GGZ_CONNECTION_SECURE_REQUIRED;
	else
		policy = GGZ_CONNECTION_SECURE_OPTIONAL;

	ggz_uri_t uri = ggz_uri_from_string(connvar.uri);

	logintype = GGZ_LOGIN_GUEST;
	if(uri.password)
		logintype = GGZ_LOGIN;
	if(!uri.port)
		uri.port = 5688;

	server = ggzcore_server_new();
	ggzcore_server_set_hostinfo(server, uri.host, uri.port, policy);
	ggzcore_server_set_logininfo(server, logintype, uri.user, uri.password, NULL);

	ggz_uri_free(uri);

	ggzcore_server_add_event_hook(server, GGZ_CONNECT_FAIL, server_failure);
	ggzcore_server_add_event_hook(server, GGZ_CONNECTED, server_connected);
	ggzcore_server_add_event_hook(server, GGZ_NEGOTIATED, server_negotiated);
	ggzcore_server_add_event_hook(server, GGZ_NEGOTIATE_FAIL, server_failure);
	ggzcore_server_add_event_hook(server, GGZ_LOGGED_IN, server_logged_in);
	ggzcore_server_add_event_hook(server, GGZ_LOGIN_FAIL, server_failure);
	ggzcore_server_add_event_hook(server, GGZ_NET_ERROR, server_failure);
	ggzcore_server_add_event_hook(server, GGZ_PROTOCOL_ERROR, server_failure);

	ggzcore_server_connect(server);

	do {
		int sfd = ggzcore_server_get_fd(server);
		int afd = ggz_async_fd();

		wait_for_input(sfd, afd);
		if(sfd != -1)
			ggzcore_server_read_data(server, sfd);
	} while (ggzcore_server_get_num_rooms(server) <= 0);
}

static void initialize_debugging(void)
{
	const char* debug_types[] = {DBG_MAIN,
		GGZCORE_DBG_CONF, GGZCORE_DBG_GAME,
		GGZCORE_DBG_HOOK, GGZCORE_DBG_MODULE,
		GGZCORE_DBG_NET, GGZCORE_DBG_POLL,
		GGZCORE_DBG_ROOM, GGZCORE_DBG_SERVER,
		GGZCORE_DBG_STATE, GGZCORE_DBG_TABLE,
		GGZCORE_DBG_XML, NULL};

	ggz_debug_init(debug_types, NULL);
}

static GGZConnectionVariety parse(int argc, char **argv)
{
	GGZConnectionVariety connvar = {
		.uri = NULL,
		.tls = 0,
		.async = 0,
		.reconnect = 0
	};

	struct option opts[] = {
		{"tls", 0, &connvar.tls, 1},
		{"reconnect", 0, &connvar.reconnect, 1},
		{"async", 0, &connvar.async, 1},
		{0, 0, 0, }
	};
	int index;
	int valid = 1;

	while(1) {
		int ret = getopt_long(argc, argv, "", opts, &index);
		if(ret == -1) {
			break;
		} else if(ret == 0) {
			/* connvar configuration */
		} else {
			valid = 0;
		}
	}

	if(optind == argc - 1) {
		connvar.uri = argv[optind];
	} else {
		printf("ERROR: Missing URL.\n");
		valid = 0;
	}

	if(!valid) {
		printf("Syntax: ggzcoreconn [--tls] [--reconnect] [--async] <GGZ URI>\n");
		connvar.uri = NULL;
	} else {
		printf("Connection: tls=%i async=%i reconnect=%i uri=%s\n",
			connvar.tls, connvar.async, connvar.reconnect, connvar.uri);
	}

	return connvar;
}

int main(int argc, char **argv)
{
	initialize_debugging();

	GGZConnectionVariety connvar = parse(argc, argv);
	if(!connvar.uri)
		return -1;

	GGZOptions opt;
	opt.flags = 0;
	if(connvar.async)
		opt.flags |= GGZ_OPT_THREADED_IO;
	if(connvar.reconnect)
		opt.flags |= GGZ_OPT_RECONNECT;
	ggzcore_init(opt);

	coreconnect(connvar);

	ggzcore_destroy();

	return 0;
}
