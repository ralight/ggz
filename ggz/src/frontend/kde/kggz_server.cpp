/* Class declaration */
#include "kggz_server.h"

#include "kggz_chatwidget.h"
#include "kggz_connect.h"

/* System includes */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int m_connected;

void KGGZ_Server::init()
{
	GGZOptions opt;

	opt.flags = GGZ_OPT_PARSER;
	opt.global_conf = "/etc/kggz.rc";
	opt.user_conf = "~./kggzrc";
	opt.local_conf = NULL;

	ggzcore_init(opt);

	ggzcore_event_connect(GGZ_SERVER_LOGIN, server_login_ok);
	ggzcore_event_connect(GGZ_SERVER_LOGIN_FAIL, server_login_fail);
	ggzcore_event_connect(GGZ_SERVER_LOGOUT, server_logout);
	ggzcore_event_connect(GGZ_SERVER_CONNECT_FAIL, server_connect_fail);
	ggzcore_event_connect(GGZ_SERVER_CHAT_MSG, server_chat_msg);
	ggzcore_event_connect(GGZ_SERVER_CHAT_ANNOUNCE, server_chat_announce);
	ggzcore_event_connect(GGZ_SERVER_CHAT_PRVMSG, server_chat_prvmsg);
	ggzcore_event_connect(GGZ_SERVER_CHAT_BEEP, server_chat_beep);
}

void KGGZ_Server::shutdown()
{
	ggzcore_event_process_all();
	ggzcore_destroy();
}

void KGGZ_Server::connect(const char *host, int port, const char *username)
{
	GGZProfile *profile;

	if(m_connected) return;

	if(!(profile = (GGZProfile*)calloc(1, sizeof(GGZProfile))))
		printf("ALERT: calloc failed in %s on line %i\n", __FILE__, __LINE__); //?? useful

	profile->type = GGZ_LOGIN_GUEST;
	profile->host = strdup(host);		// !!! MEMORY LEAK?!
	profile->port = port;
	profile->login = strdup(username);
	ggzcore_event_trigger(GGZ_USER_LOGIN, profile, NULL);
	printf("*connect\n");
}

void KGGZ_Server::disconnect()
{
	if(!m_connected) return;
	printf("*disconnect\n");
	ggzcore_event_trigger(GGZ_USER_LOGOUT, NULL, NULL);
	m_connected = 0;
}

void KGGZ_Server::server_login_ok(GGZEventID id, void *event_data, void *user_data)
{
	char *user = NULL, *room = NULL, *server = NULL;

	if (ggzcore_state_is_online()) {
		user = ggzcore_state_get_profile_login();
		server = ggzcore_state_get_profile_host();
	}
	if (ggzcore_state_is_in_room())
		room = ggzcore_room_get_name(ggzcore_state_get_room());

	printf("!! (1)\n");
	printf("%s in %s on %s!\n", user, room, server);
	KGGZ_Chatwidget::adminreceive(QString("Connected: Server running on ") + QString(server));
	KGGZ_Connect::success(TRUE);
	m_connected = 1;
	ggzcore_event_trigger(GGZ_USER_JOIN_ROOM, (void*)1, NULL);
	printf("*auto-room-change\n");
}

void KGGZ_Server::server_login_fail(GGZEventID id, void *event_data, void *user_data)
{
	printf("!! (2)\n");
	KGGZ_Connect::success(FALSE);
	m_connected = 0;
}

void KGGZ_Server::server_logout(GGZEventID id, void *event_data, void *user_data)
{
	printf("!! (3)\n");
}

void KGGZ_Server::server_connect_fail(GGZEventID id, void *event_data, void *user_data)
{
	printf("!! (4)\n");
	KGGZ_Connect::success(FALSE);
}

void KGGZ_Server::server_chat_msg(GGZEventID id, void *event_data, void *user_data)
{
	char* player;
	char* message;

	printf("!! (99)\n");
	player = ((char**)(event_data))[0];
	message = ((char**)(event_data))[1];
	printf("%s wrote: %s\n", player, message);
	KGGZ_Chatwidget::receive(player, message);
}

void KGGZ_Server::server_chat_prvmsg(GGZEventID id, void *event_data, void *user_data)
{
	printf("!! (5)\n");
}

void KGGZ_Server::server_chat_beep(GGZEventID id, void *event_data, void *user_data)
{
	printf("!! (6)\n");
}

void KGGZ_Server::server_chat_announce(GGZEventID id, void *event_data, void *user_data)
{
	printf("!! (7)\n");
}

void showstatus(int id)
{
	static int x;
	char *currentstatus = NULL;
	char *user = NULL;

	if(x == id) return;
	x = id;

	switch(id)
	{
		case GGZ_STATE_OFFLINE:
			currentstatus = "Offline";
			break;
		case GGZ_STATE_CONNECTING:
			currentstatus = "Connecting";
			break;
		case GGZ_STATE_ONLINE:
			currentstatus = "Online";
			break;
		case GGZ_STATE_LOGGING_IN:
			currentstatus = "Logging In";
			break;
		case GGZ_STATE_LOGGED_IN:
			currentstatus = "Logged In";
			break;
		case GGZ_STATE_ENTERING_ROOM:
			currentstatus = "--> Room";
			user = ggzcore_state_get_profile_login();
			KGGZ_Chatwidget::adminreceive(QString(user) + QString(" enters the room."));
			break;
		case GGZ_STATE_IN_ROOM:
			currentstatus = "Chatting";
			break;
		case GGZ_STATE_JOINING_TABLE:
			currentstatus = "--> Table";
			break;
		case GGZ_STATE_AT_TABLE:
			currentstatus = "Playing";
			break;
		case GGZ_STATE_LEAVING_TABLE:
			currentstatus = "<-- Table";
			break;
		case GGZ_STATE_LOGGING_OUT:
			currentstatus = "Logging Out";
			user = ggzcore_state_get_profile_login();
			KGGZ_Chatwidget::adminreceive(QString(user) + QString(" has quit the GGZ."));
			break;
	}
	printf("status: %s\n", currentstatus);
}

void KGGZ_Server::loop()
{
	ggzcore_event_process_all();
	showstatus((int)ggzcore_state_get_id());
}
