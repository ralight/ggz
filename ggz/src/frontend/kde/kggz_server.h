#ifndef KGGZ_SERVER_H
#define KGGZ_SERVER_H

/* The GGZ core client functions */
#include <ggzcore.h>

class KGGZ_Server
{
public:
        static void init();
	static void shutdown();
	static void connect(const char *host, int port, const char *username);
	static void disconnect();
	static void loop();

	static void server_login_ok(GGZEventID id, void *event_data, void *user_data);
	static void server_login_fail(GGZEventID id, void *event_data, void *user_data);
	static void server_logout(GGZEventID id, void *event_data, void *user_data);
	static void server_connect_fail(GGZEventID id, void *event_data, void *user_data);
	static void server_chat_msg(GGZEventID id, void *event_data, void *user_data);
	static void server_chat_prvmsg(GGZEventID id, void *event_data, void *user_data);
	static void server_chat_beep(GGZEventID id, void *event_data, void *user_data);
	static void server_chat_announce(GGZEventID id, void *event_data, void *user_data);
};

#endif