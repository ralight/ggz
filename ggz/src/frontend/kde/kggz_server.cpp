/* Class declaration */
#include "kggz_server.h"

#include "kggz_chatwidget.h"
#include "kggz_connect.h"
#include "kggz_roomsmenu.h"
#include "kggz_motd.h"
#include "kggz_users.h"

/* System includes */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <kapp.h>
#ifdef USE_SERVER
#include <dirent.h>
#include <unistd.h>
#include <signal.h>
#endif

int m_connected;
int m_running;
int m_roomslist;
int m_userslist;

char memsavehost[1024];
char memsaveusername[1024];
char memsavepassword[1024];

void KGGZ_Server::init()
{
	GGZOptions opt;
	char userconf[128];

	if(m_running) return;

	opt.flags = GGZ_OPT_PARSER;
	opt.global_conf = "/etc/ggz/ggz.conf";
	sprintf(userconf, "%s/.ggz/kggz.rc", getenv("HOME"));
	opt.user_conf = strdup(userconf);
	/*opt.local_conf = NULL;*/ // this has been removed

	if(ggzcore_init(opt) != 0)
	{
		printf("kggz_server::init: ggzcore could not be initialized.\n");
		exit(-1);
	}

	ggzcore_event_add_callback(GGZ_SERVER_CONNECT, server_connect);
	ggzcore_event_add_callback(GGZ_SERVER_CONNECT_FAIL, server_connect_fail);
	ggzcore_event_add_callback(GGZ_SERVER_LOGIN, server_login_ok);
	ggzcore_event_add_callback(GGZ_SERVER_LOGIN_FAIL, server_login_fail);
	ggzcore_event_add_callback(GGZ_SERVER_LOGOUT, server_logout);
	ggzcore_event_add_callback(GGZ_SERVER_MOTD, server_motd);
	ggzcore_event_add_callback(GGZ_SERVER_CHAT_FAIL, server_chat_fail);
	ggzcore_event_add_callback(GGZ_SERVER_CHAT_MSG, server_chat_msg);
	ggzcore_event_add_callback(GGZ_SERVER_CHAT_ANNOUNCE, server_chat_announce);
	ggzcore_event_add_callback(GGZ_SERVER_CHAT_PRVMSG, server_chat_prvmsg);
	ggzcore_event_add_callback(GGZ_SERVER_CHAT_BEEP, server_chat_beep);
	ggzcore_event_add_callback(GGZ_SERVER_ERROR, server_chat_beep);
	ggzcore_event_add_callback(GGZ_SERVER_ROOM_ENTER, server_room_enter);
	ggzcore_event_add_callback(GGZ_SERVER_ROOM_LEAVE, server_room_leave);
	ggzcore_event_add_callback(GGZ_SERVER_ROOM_JOIN, server_room_join);
	ggzcore_event_add_callback(GGZ_SERVER_ROOM_JOIN_FAIL, server_room_join_fail);
	ggzcore_event_add_callback(GGZ_SERVER_LIST_PLAYERS, server_list_players);
	ggzcore_event_add_callback(GGZ_SERVER_LIST_ROOMS, server_list_rooms);

	m_connected = FALSE;
	m_running = TRUE;
	m_roomslist = FALSE;
}

void KGGZ_Server::shutdown()
{
	if (!m_running) return;
	m_running = FALSE;
	ggzcore_event_poll(NULL, 0, 0);
	//ggzcore_destroy(); // that brought up an error
}

void KGGZ_Server::connect(const char *host, int port, const char *username, const char *password, int loginmode)
{
	GGZProfile *profile;

	if(m_connected) return;

	if(!(profile = (GGZProfile*)calloc(1, sizeof(GGZProfile))))
		printf("ALERT: calloc failed in %s on line %i\n", __FILE__, __LINE__); //?? useful

	profile->type = GGZ_LOGIN_GUEST;
	//profile->host = strdup(host);		// !!! MEMORY LEAK?!
	strcpy(memsavehost, host);
	strcpy(memsaveusername, username);
	strcpy(memsavepassword, password);
	profile->host = memsavehost;
	profile->login = memsaveusername;
	profile->port = port;
	profile->type = (GGZLoginType)loginmode; // compilers aren't smart enough
	profile->password = memsavepassword;
	//profile->login = strdup(username);
	printf("*pre-connect\n");
	ggzcore_event_enqueue(GGZ_USER_LOGIN, profile, NULL);
	printf("*connect\n");
}

void KGGZ_Server::disconnect()
{
	printf("*pre-disconnect\n");
	if(!m_connected) return;
	m_connected = FALSE;
	printf("*disconnect\n");
	ggzcore_event_enqueue(GGZ_USER_LOGOUT, NULL, NULL);
	KGGZ_RoomsMenu::removeAll();
}

void KGGZ_Server::changeRoom(int room)
{
	ggzcore_event_enqueue(GGZ_USER_JOIN_ROOM, (void*)room, NULL);
}

#ifdef USE_SERVER
// findprocess:
// find a unix system process by the given name
// returns pid if process already runs, 0 otherwise; and -1 on error
int KGGZ_Server::findProcess(char *cmdline)
{
	DIR *d;
	struct dirent *e;
	FILE *f;
	char dir[512];
	int ret;
	char cmd[256];

	d = opendir("/proc/");
	if(d == NULL) return -1;
	ret = 0;
	while((!ret) && (e = readdir(d)))
	{
		strcpy(dir, "/proc/");
		strcat(dir, e->d_name);
		strcat(dir, "/cmdline");
		f = fopen(dir, "r");
		if(f != NULL)
		{
			fscanf(f, "%s", &cmd);
			if(strcmp(cmd, cmdline) == 0) ret = atoi(e->d_name);
			fclose(f);
		}
	}
	closedir(d);
	return ret;
}

// launches the ggzd server
// returns -1 on error, -2 if already running, child pid else
int KGGZ_Server::launchServer()
{
	int result;
	const char *ggzd = "ggzd";
	char *const ggzdarg[] = {"ggzd", NULL};

	printf("Starting GGZD...\n");
	if(findProcess("ggzd") > 0)
	{
		printf("GGZd already running!\n");
		return -2;
	}
	result = fork();
	if(result == -1) /* external validation */
	{
		printf("Undefined error!\n");
	}
	else if(result == 0)
	{
		result = execvp(ggzd, ggzdarg);
		printf("execvp-Result: %s\n", result);
	}
	else
	{
		// parent process; sleep a while to allow server to startup
		while(findProcess("ggzd") == 0)
			sleep(1);
	}
	return result;
}

// tries to kill all running ggzd processes
// sends sigterm first, like the kill(1) command
// returns -1 on failure, 1 on success, and 0 if only a sigkill was successful
int KGGZ_Server::killServer()
{
	pid_t pid;
	int ret;
	int counter;
	int sigterm;

	pid = 1;
	counter = 0;
	sigterm = 1;
	printf("Going to kill ggzd...\n");
	while(pid > 0)
	{
		pid = findProcess("ggzd");
		if(pid > 0)
		{
			printf("Found one (%i) ;)\n", pid);
			ret = kill(pid, SIGTERM);
			printf("SIGTERM-Ret is: %i\n", ret);
			if(ret != 0)
			{
				printf("Thou bastard, don't want? I'll show you...\n");
				ret = kill(pid, SIGTERM);
				printf("SIGKILL-Ret is: %i\n", ret);
				sigterm = 0;
			}
			counter++;
		}
	}
	if(counter) return sigterm;
	return -1;
}
#endif

void KGGZ_Server::server_login_ok(GGZEventID id, void *event_data, void *user_data)
{
	char *user = NULL, *room = NULL, *server = NULL;

	printf("*login ok\n");
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
	m_connected = TRUE;
	ggzcore_event_enqueue(GGZ_USER_LIST_ROOMS, NULL, NULL);
	printf("*auto-list-rooms\n");
	//ggzcore_event_enqueue(GGZ_USER_JOIN_ROOM, (void*)1, NULL);
	//printf("*auto-room-change\n");
}

void KGGZ_Server::server_login_fail(GGZEventID id, void *event_data, void *user_data)
{
	printf("!! (2)\n");
	KGGZ_Connect::success(FALSE);
	m_connected = FALSE;
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
	char *c;

	printf("!! (99)\n");
	player = ((char**)(event_data))[0];
	message = ((char**)(event_data))[1];
	printf("%s wrote: %s\n", player, message);

	c = message;
	if((c[0] == '/') && (c[1] == 'm') && (c[2] == 'e') && (c[3] == ' '))
		KGGZ_Chatwidget::inforeceive(player, message);
	else
		if(strcmp(player, ggzcore_state_get_profile_login()) == 0)
			KGGZ_Chatwidget::receiveown(player, message);
		else
			KGGZ_Chatwidget::receive(player, message);
}

void KGGZ_Server::server_chat_prvmsg(GGZEventID id, void *event_data, void *user_data)
{
	printf("!! (5)\n");
}

void KGGZ_Server::server_chat_beep(GGZEventID id, void *event_data, void *user_data)
{
	printf("!! (6)\n");
	KGGZ_Chatwidget::adminreceive(QString("You have been beeped!"));
	XBell(kapp->getDisplay(), 100);
}

void KGGZ_Server::server_chat_announce(GGZEventID id, void *event_data, void *user_data)
{
	printf("!! (7)\n");
}

void KGGZ_Server::server_connect(GGZEventID id, void *event_data, void *user_data)
{
	printf("!! (8)\n");
}

void KGGZ_Server::server_motd(GGZEventID id, void *event_data, void *user_data)
{
	char **motd;
	int i;

	printf("!! (9 - MOTD!)\n");

	motd = (char**)event_data;
	for(int i = 0; motd[i] != NULL; i++)
		KGGZ_Motd::append(motd[i]);
}

void KGGZ_Server::server_chat_fail(GGZEventID id, void *event_data, void *user_data)
{
	printf("!! (10)\n");
}

void KGGZ_Server::server_room_enter(GGZEventID id, void *event_data, void *user_data)
{
	static char *player = NULL;

	if(player) free(player);
	player = strdup((char*)event_data);
	printf("!! (11)\n");
	KGGZ_Users::add(player);
	KGGZ_Chatwidget::adminreceive(QString(player) + QString(" enters the room."));
}

void KGGZ_Server::server_room_leave(GGZEventID id, void *event_data, void *user_data)
{
	static char *player = NULL;

	if(player) free(player);
	player = strdup((char*)event_data);
	printf("!! (12)\n");
	KGGZ_Users::remove(player);
	KGGZ_Chatwidget::adminreceive(QString(player) + QString(" leaves the room."));
}

void KGGZ_Server::server_room_join(GGZEventID id, void *event_data, void *user_data)
{
	printf("!! (13)\n");
	if(!m_userslist) ggzcore_event_enqueue(GGZ_USER_LIST_PLAYERS, NULL, NULL);
}

void KGGZ_Server::server_room_join_fail(GGZEventID id, void *event_data, void *user_data)
{
	printf("!! (14)\n");
}

void KGGZ_Server::server_list_players(GGZEventID id, void *event_data, void *user_data)
{
	char **names;
	int i;

	printf("!! (15)\n");
	if(!(names = ggzcore_player_get_names())) return;
	if(m_userslist) KGGZ_Chatwidget::adminreceive("List of players:");
	for(i = 0; names[i]; i++)
	{
		if(!m_userslist) KGGZ_Users::add(names[i]);
		else KGGZ_Chatwidget::adminreceive(QString("-- ") + QString(names[i]));
	}
	free(names);
	m_userslist = TRUE;
}

void KGGZ_Server::server_list_rooms(GGZEventID id, void *event_data, void *user_data)
{
	char **names;
	int i;

	printf("!! (16)\n");
	if(!(names = ggzcore_room_get_names())) return;
	if(m_roomslist) KGGZ_Chatwidget::adminreceive("List of rooms:");
	for(i = 0; names[i]; i++)
	{
		if(!m_roomslist) KGGZ_RoomsMenu::add(names[i]);
		else KGGZ_Chatwidget::adminreceive(QString("-- ") + QString(names[i]));
	}

	free(names);
	m_roomslist = TRUE;
}

void showstatus(int id)
{
	static int x;
	char *currentstatus = NULL;
	char *user = NULL, *room = NULL;

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
			//KGGZ_Users::add(user); - this is not good, it causes a doubled entry
			break;
		case GGZ_STATE_IN_ROOM:
			currentstatus = "Chatting";
			if (ggzcore_state_is_in_room())
			{
				room = ggzcore_room_get_name(ggzcore_state_get_room());
				KGGZ_Chatwidget::adminreceive(QString("Now in room ") + QString(room));
			}
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
	ggzcore_event_poll(NULL, 0, 0);
	//ggzcore_event_process_all(); // not this one, this is evil and costs me sunday afternoons
	showstatus((int)ggzcore_state_get_id());
}
