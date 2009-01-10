#include "ggzcore_mainloop.h"

#include <unistd.h>
#include <errno.h>

#include <ggz.h>
#include <stdio.h> // tmp

#define DBG_MAINLOOP "GGZCORE:MAINLOOP"

static void ggzcore_mainloop_quit(GGZCoreMainLoop *mainloop, int retval)
{
	mainloop->status = retval;

	ggz_debug(DBG_MAINLOOP, "Cleaned up, ready for quitting now.");
}

static void ggzcore_mainloop_callback(GGZCoreMainLoop *mainloop, GGZCoreMainLoopEvent id, const char *message)
{
	ggz_debug(DBG_MAINLOOP, "Callback message: %s.", message);

	if(id == GGZCORE_MAINLOOP_ABORT)
		ggzcore_mainloop_quit(mainloop, -1);
	if((id == GGZCORE_MAINLOOP_READY) && (!mainloop->loop))
		ggzcore_mainloop_quit(mainloop, 0);

	if(mainloop->func)
		(mainloop->func)(id, message, mainloop->server);
}

static void ggzcore_mainloop_event(GGZCoreMainLoop *mainloop, int sfd, int afd, int timeout)
{
	fd_set my_fd_set;
	int status;
	struct timeval stdtimeout = {.tv_sec = 10, .tv_usec = 0};
	struct timeval *timeoutptr = &stdtimeout;

	if(!timeout)
		timeoutptr = NULL;

	FD_ZERO(&my_fd_set);
	if(sfd >= 0)
		FD_SET(sfd, &my_fd_set);
	if(afd >= 0)
		FD_SET(afd, &my_fd_set);

	int maxfd = sfd;
	if(afd > sfd)
		maxfd = afd;

	status = select(maxfd + 1, &my_fd_set, NULL, NULL, timeoutptr);
	if(status < 0)
		if(errno != EINTR)
			ggz_error_sys_exit("Select error while blocking");

	if(sfd >= 0) {
		if(!FD_ISSET(sfd, &my_fd_set)) {
			ggzcore_mainloop_callback(mainloop, GGZCORE_MAINLOOP_ABORT, "Connection to server timed out");
		}
	}

	if(afd >= 0) {
		GGZAsyncEvent ev = ggz_async_event();
		if(ev.type == GGZ_ASYNC_RESOLVER) {
			ggz_debug(DBG_MAINLOOP, "ASYNC: Resolver returned address %s:%i", ev.address, ev.port);
		} else if(ev.type == GGZ_ASYNC_CONNECT) {
			ggz_debug(DBG_MAINLOOP, "ASYNC: Connection established on socket %i", ev.socket);
		} else {
			ggz_debug(DBG_MAINLOOP, "ASYNC: Noop");
		}
	}
}

static GGZHookReturn ggzcore_mainloop_server_failure(GGZServerEvent id, const void *event_data, const void *user_data)
{
	ggz_debug(DBG_MAINLOOP, "GGZ failure: event %d.", id);

	const char *message = NULL;
	if((id != GGZ_CONNECT_FAIL)
	&& (id != GGZ_NET_ERROR)) {
		const GGZErrorEventData *msg = event_data;
		message = msg->message;
	} else {
		message = event_data;
	}

	GGZCoreMainLoop *mainloop = (GGZCoreMainLoop*)user_data;

	if(ggzcore_server_get_state(mainloop->server) == GGZ_STATE_RECONNECTING) {
		ggz_debug(DBG_MAINLOOP, "ERROR: Connection broke temporarily: %s.", message);

		ggzcore_mainloop_callback(mainloop, GGZCORE_MAINLOOP_WAIT, "Connection broke temporarily");
	} else {
		ggz_debug(DBG_MAINLOOP, "ERROR: Could not connect to server, or other issue: %s.", message);

		ggzcore_mainloop_callback(mainloop, GGZCORE_MAINLOOP_ABORT, "Connection could not be established");
	}

	return GGZ_HOOK_OK;
}

static GGZHookReturn ggzcore_mainloop_server_connected(GGZServerEvent id, const void *event_data, const void *user_data)
{
	ggz_debug(DBG_MAINLOOP, "Connected to server.");

	return GGZ_HOOK_OK;
}

static GGZHookReturn ggzcore_mainloop_server_negotiated(GGZServerEvent id, const void *event_data, const void *user_data)
{
	ggz_debug(DBG_MAINLOOP, "Server negotiated.");

	GGZCoreMainLoop *mainloop = (GGZCoreMainLoop*)user_data;

	int rc = ggzcore_server_login(mainloop->server);
	if(rc) {
		ggz_debug(DBG_MAINLOOP, "ERROR: Login process could not be initiated.");

		ggzcore_mainloop_callback(mainloop, GGZCORE_MAINLOOP_ABORT, "Login could not be established");
	}

	return GGZ_HOOK_OK;
}

static GGZHookReturn ggzcore_mainloop_server_logged_in(GGZServerEvent id, const void *event_data, const void *user_data)
{
	ggz_debug(DBG_MAINLOOP, "Logged in to server.");

	GGZCoreMainLoop *mainloop = (GGZCoreMainLoop*)user_data;

	ggzcore_mainloop_callback(mainloop, GGZCORE_MAINLOOP_READY, "Connection established and login succeeded");

	return GGZ_HOOK_OK;
}

static void ggzcore_mainloop_initialize_debugging(void)
{
	const char* debug_types[] = {DBG_MAINLOOP,
		GGZCORE_DBG_CONF, GGZCORE_DBG_GAME,
		GGZCORE_DBG_HOOK, GGZCORE_DBG_MODULE,
		GGZCORE_DBG_NET, GGZCORE_DBG_POLL,
		GGZCORE_DBG_ROOM, GGZCORE_DBG_SERVER,
		GGZCORE_DBG_STATE, GGZCORE_DBG_TABLE,
		GGZCORE_DBG_XML, NULL};

	ggz_debug_init(debug_types, NULL);
}

int ggzcore_mainloop_start(GGZCoreMainLoop mainloop)
{
	GGZConnectionPolicy policy;
	GGZLoginType logintype;

	mainloop.status = -2;

	if(mainloop.debug)
		ggzcore_mainloop_initialize_debugging();

	GGZOptions opt;
	opt.flags = 0;
	if(mainloop.async)
		opt.flags |= GGZ_OPT_THREADED_IO;
	if(mainloop.reconnect)
		opt.flags |= GGZ_OPT_RECONNECT;
	ggzcore_init(opt);

	if(mainloop.tls)
		policy = GGZ_CONNECTION_SECURE_REQUIRED;
	else
		policy = GGZ_CONNECTION_SECURE_OPTIONAL;

	ggz_uri_t uri = ggz_uri_from_string(mainloop.uri);

	logintype = GGZ_LOGIN_GUEST;
	if(uri.password)
		logintype = GGZ_LOGIN;
	if(!uri.port)
		uri.port = 5688;

	GGZServer *server = ggzcore_server_new();
	ggzcore_server_set_hostinfo(server, uri.host, uri.port, policy);
	ggzcore_server_set_logininfo(server, logintype, uri.user, uri.password, NULL);

	ggz_uri_free(uri);

	ggzcore_server_add_event_hook_full(server, GGZ_CONNECT_FAIL, ggzcore_mainloop_server_failure, &mainloop);
	ggzcore_server_add_event_hook_full(server, GGZ_CONNECTED, ggzcore_mainloop_server_connected, &mainloop);
	ggzcore_server_add_event_hook_full(server, GGZ_NEGOTIATED, ggzcore_mainloop_server_negotiated, &mainloop);
	ggzcore_server_add_event_hook_full(server, GGZ_NEGOTIATE_FAIL, ggzcore_mainloop_server_failure, &mainloop);
	ggzcore_server_add_event_hook_full(server, GGZ_LOGGED_IN, ggzcore_mainloop_server_logged_in, &mainloop);
	ggzcore_server_add_event_hook_full(server, GGZ_LOGIN_FAIL, ggzcore_mainloop_server_failure, &mainloop);
	ggzcore_server_add_event_hook_full(server, GGZ_NET_ERROR, ggzcore_mainloop_server_failure, &mainloop);
	ggzcore_server_add_event_hook_full(server, GGZ_PROTOCOL_ERROR, ggzcore_mainloop_server_failure, &mainloop);

	mainloop.server = server;

	ggzcore_server_connect(server);

	while(mainloop.status == -2) {
		int sfd = ggzcore_server_get_fd(server);
		int afd = ggz_async_fd();

		int timeout = 1;
		if(ggzcore_server_is_online(server))
			timeout = 0;

		ggzcore_mainloop_event(&mainloop, sfd, afd, timeout);
		if(sfd != -1)
			ggzcore_server_read_data(server, sfd);
	}

	if(mainloop.server) {
		ggzcore_server_disconnect(mainloop.server);
		ggzcore_server_free(mainloop.server);
		mainloop.server = NULL;
	}
	ggzcore_destroy();

	return mainloop.status;
}
