#include "net.h"
#include <ggzcore.h>
#include <stdlib.h>

/* Globals */
int status = NET_NOOP;
GGZServer *server = NULL;
GGZRoom *room = NULL;

/* Prototypes */
GGZHookReturn net_hook_connect(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_login(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_roomlist(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_enter(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_fail(unsigned int id, void *event_data, void *user_data);

void net_init()
{
	GGZOptions opt;
	int ret;

	opt.flags = GGZ_OPT_MODULES | GGZ_OPT_PARSER;
	opt.debug_file = "/tmp/doobadoo";
	opt.debug_levels = 0;
	ret = ggzcore_init(opt);
}

void net_connect(const char *host, int port)
{
	net_init();

	server = ggzcore_server_new();

	ggzcore_server_add_event_hook(server, GGZ_CONNECTED, net_hook_connect);
	ggzcore_server_add_event_hook(server, GGZ_CONNECT_FAIL, net_hook_fail);
	ggzcore_server_add_event_hook(server, GGZ_NEGOTIATE_FAIL, net_hook_fail);
	ggzcore_server_add_event_hook(server, GGZ_LOGIN_FAIL, net_hook_fail);
	ggzcore_server_add_event_hook(server, GGZ_ENTER_FAIL, net_hook_fail);
	ggzcore_server_add_event_hook(server, GGZ_PROTOCOL_ERROR, net_hook_fail);
	ggzcore_server_add_event_hook(server, GGZ_NET_ERROR, net_hook_fail);
	ggzcore_server_add_event_hook(server, GGZ_ENTERED, net_hook_enter);
	ggzcore_server_add_event_hook(server, GGZ_LOGGED_IN, net_hook_login);
	ggzcore_server_add_event_hook(server, GGZ_ROOM_LIST, net_hook_roomlist);

	ggzcore_server_set_hostinfo(server, host, port);
	ggzcore_server_connect(server);
}

void net_join(int room)
{
	ggzcore_server_join_room(server, room);
}

int net_status()
{
	int ret;

	if(ggzcore_server_data_is_pending(server))
		ggzcore_server_read_data(server);

	ret = status;
	if(status == NET_GOTREADY) status = NET_NOOP;
	if(status == NET_LOGIN) status = NET_NOOP;
	return ret;
}

GGZHookReturn net_hook_connect(unsigned int id, void *event_data, void *user_data)
{
	/*nasty ggzcore bug?*/
	while((!ggzcore_server_is_online(server)) && (status == NET_NOOP)) ggzcore_server_read_data(server);

	if(status == NET_NOOP)
	{
		ggzcore_server_set_logininfo(server, GGZ_LOGIN_GUEST, "grubby", "");
		ggzcore_server_login(server);
	}
	return GGZ_HOOK_OK;
}

GGZHookReturn net_hook_login(unsigned int id, void *event_data, void *user_data)
{
	ggzcore_server_list_rooms(server, -1, 0);
	return GGZ_HOOK_OK;
}

GGZHookReturn net_hook_roomlist(unsigned int id, void *event_data, void *user_data)
{
	status = NET_LOGIN;
	return GGZ_HOOK_OK;
}

GGZHookReturn net_hook_enter(unsigned int id, void *event_data, void *user_data)
{
	status = NET_GOTREADY;
	return GGZ_HOOK_OK;
}

GGZHookReturn net_hook_fail(unsigned int id, void *event_data, void *user_data)
{
	status = NET_ERROR;
	return GGZ_HOOK_OK;
}

