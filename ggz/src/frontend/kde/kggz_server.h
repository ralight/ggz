#ifndef KGGZ_SERVER_H
#define KGGZ_SERVER_H

/* The GGZ core client functions */
#include <ggzcore.h>

/* workaround for foolish g++ error message */
#define GGZEventID unsigned int

class KGGZ_Server
{
public:
	/* server control functions */
        static void init();
	static void shutdown();
	static void connect(const char *host, int port, const char *username, const char *password, int loginmode);
	static void disconnect();
	static void loop();
#ifdef USE_SERVER
	static int launchServer();
	static int killServer();
#endif

	/* user functions */
	static void changeRoom(int room);
#ifdef USE_SERVER
	static int findProcess(char *cmdline);
#endif

	/* Callbacks */
	static void server_connect(GGZEventID id, void *event_data, void *user_data);
	static void server_connect_fail(GGZEventID id, void *event_data, void *user_data);
	static void server_login_ok(GGZEventID id, void *event_data, void *user_data);
	static void server_login_fail(GGZEventID id, void *event_data, void *user_data);
	static void server_logout(GGZEventID id, void *event_data, void *user_data);
	static void server_motd(GGZEventID id, void *event_data, void *user_data);
	static void server_chat_fail(GGZEventID id, void *event_data, void *user_data);
	static void server_chat_msg(GGZEventID id, void *event_data, void *user_data);
	static void server_chat_prvmsg(GGZEventID id, void *event_data, void *user_data);
	static void server_chat_beep(GGZEventID id, void *event_data, void *user_data);
	static void server_chat_announce(GGZEventID id, void *event_data, void *user_data);
	static void server_room_enter(GGZEventID id, void *event_data, void *user_data);
	static void server_room_leave(GGZEventID id, void *event_data, void *user_data);
	static void server_room_join(GGZEventID id, void *event_data, void *user_data);
	static void server_room_join_fail(GGZEventID id, void *event_data, void *user_data);
	static void server_list_players(GGZEventID id, void *event_data, void *user_data);
	static void server_list_rooms(GGZEventID id, void *event_data, void *user_data);
};

#endif