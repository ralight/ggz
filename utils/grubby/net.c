/* net.c
 *
 * A GRUB ia A (G)GZ (R)eally (U)seful (B)ot
 *
 * Original: Rich Gade
 * ReWrite:  Justin Zaun
 * New ggzcore API: Josef Spillner (May 20, 2001)
 *
 * May 23rd, 2000
 */

#include <easysock.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <ggzcore.h>
#include "chat.h"
#include "datatypes.h"
#include "log.h"
#include "net.h"
#include "support.h"

extern struct Grubby grubby;

GGZRoom *activeroom;
GGZServer *activeserver;

static GGZHookReturn net_connect_ok(GGZServerEvent id, void*, void*);
static GGZHookReturn net_login_ok(GGZServerEvent id, void*, void*);
static GGZHookReturn net_chat_msg(GGZRoomEvent id, void*, void*);
static GGZHookReturn net_chat_prvmsg(GGZRoomEvent id, void*, void*);
static GGZHookReturn net_room_enter(GGZRoomEvent id, void*, void*);
static GGZHookReturn net_room_leave(GGZRoomEvent id, void*, void*);
static GGZHookReturn net_failed(GGZServerEvent id, void*, void*);
static GGZHookReturn net_entered(GGZServerEvent id, void*, void*);
static GGZHookReturn net_roomlist(GGZServerEvent id, void*, void*);

void net_register_hooks(void);

void net_send_msg(char *message)
{
	ggzcore_room_chat(activeroom, GGZ_CHAT_NORMAL, NULL, message);
}

void net_send_prvmsg(char *to, char *message)
{
	ggzcore_room_chat(activeroom, GGZ_CHAT_PERSONAL, to, message);
}

void net_send_chat_insert_name( char *from, char *msg )
{
        char *p;
        char out[1024];
        int percent_s=0;
                
        for( p=msg; *p!='\0'; p++ )
                if( *p == '%' && *(p+1) == 's' )
                        percent_s++;
 
        if( percent_s )
                sprintf( out, msg, grubby.name );
        else
                strcpy( out, msg );
        
        net_send_prvmsg( from, out );
}

void net_change_room( int room_num )
{
	if(ggzcore_server_join_room(activeserver, room_num) == -1)
		printf("Error! Couldn't change to room.\n");
}

void net_connect(void)
{
	printf ("  |- Connecting to Server\n");
	ggzcore_server_set_hostinfo(activeserver, grubby.host, grubby.port);
	if(ggzcore_server_connect(activeserver) == -1)
	{
		printf("Error: Connection failed.\n");
		exit(-1);
	}
}


/*
 * Callbacks for GGZCORE
 */

void net_register_hooks(void)
{
	activeserver = ggzcore_server_new();
		
	ggzcore_server_add_event_hook(activeserver, GGZ_LOGGED_IN, net_login_ok);
	ggzcore_server_add_event_hook(activeserver, GGZ_CONNECTED, net_connect_ok);
	ggzcore_server_add_event_hook(activeserver, GGZ_ENTERED, net_entered);
	ggzcore_server_add_event_hook(activeserver, GGZ_ROOM_LIST, net_roomlist);
	
	ggzcore_server_add_event_hook(activeserver, GGZ_ENTER_FAIL, net_failed);
	ggzcore_server_add_event_hook(activeserver, GGZ_CONNECT_FAIL, net_failed);
	ggzcore_server_add_event_hook(activeserver, GGZ_LOGIN_FAIL, net_failed);
}

static GGZHookReturn net_failed(GGZServerEvent id, void* event_data, void* user_data)
{
	printf("Error: An error (%i) occured. Grubby feels very sorry.\n", id);
	exit(-1);
}

static GGZHookReturn net_entered(GGZServerEvent id, void* event_data, void* user_data)
{
	//int i;
	
	printf ("  |- Entered room\n");

	//for(i = 0; i < ggzcore_server_get_num_rooms(activeserver); i++)
	//{
		//activeroom = ggzcore_server_get_nth_room(activeserver, i);
		activeroom = ggzcore_server_get_cur_room(activeserver);
		ggzcore_room_add_event_hook(activeroom, GGZ_ROOM_ENTER, net_room_enter);
		ggzcore_room_add_event_hook(activeroom, GGZ_ROOM_LEAVE, net_room_leave);
		ggzcore_room_add_event_hook(activeroom, GGZ_CHAT, net_chat_msg);
		ggzcore_room_add_event_hook(activeroom, GGZ_PRVMSG, net_chat_prvmsg);
	//}

	printf ("  |- Grubby is ready :)\n");
	
	return GGZ_HOOK_OK;
}

static GGZHookReturn net_roomlist(GGZServerEvent id, void* event_data, void* user_data)
{
	net_change_room(ggzcore_conf_read_int("GRUBBYSETTINGS", "AUTOJOIN", -1));

	return GGZ_HOOK_OK;
}

static GGZHookReturn net_login_ok(GGZServerEvent id, void* event_data, void* user_data)
{
	printf ("  |- Login OK\n");

	ggzcore_server_list_rooms(activeserver, -1, 0);

	return GGZ_HOOK_OK;
}

static GGZHookReturn net_connect_ok(GGZServerEvent id, void* event_data, void* user_data)
{
	printf ("  |- Logging in\n");

	while(!ggzcore_server_is_online(activeserver)) ggzcore_server_read_data(activeserver);
	
	ggzcore_server_set_logininfo(activeserver, GGZ_LOGIN_GUEST, grubby.name, "");
	ggzcore_server_login(activeserver); 

	return GGZ_HOOK_OK;
}

static GGZHookReturn net_chat_msg(GGZRoomEvent id, void* event_data, void* user_data)
{
        char* player;
        char* message;
        
	player = ((char**)(event_data))[0];
	message = ((char**)(event_data))[1];

	log_write(player, message);
	chat_parse(player, message);
	return GGZ_HOOK_OK;
}

static GGZHookReturn net_chat_prvmsg(GGZRoomEvent id, void* event_data, void* user_data)
{
        char* player;
        char* message;
	char* fixed;
        
        player = ((char**)(event_data))[0];
        message = ((char**)(event_data))[1];
	fixed = malloc((strlen(message)+strlen(grubby.name)+1)*sizeof(char*));
	fixed = strcat(fixed,  grubby.name);
	fixed = strcat(fixed,  " ");
	fixed = strcat(fixed, message);

	log_write(player, fixed);
	chat_parse(player, fixed);

	free(fixed);
	return GGZ_HOOK_OK;
}

static GGZHookReturn net_room_enter(GGZRoomEvent id, void* event_data, void* user_data)
{
	char out[1024];
	int i, num_msg;
	char **messages;
	char **alerts;
	int num_alerts;

	i = support_check_known(event_data);

	/* Unknown Person */
	if(i == -1)
	{
		sprintf(out, "Hi %s, I'm %s.", (char*)event_data, grubby.name);
		net_send_prvmsg(event_data, out);
		sprintf(out, "I haven't seen you before, please take a second to see what I can do. Type: %s, help", grubby.name);
		net_send_prvmsg(event_data, out);

		ggzcore_conf_write_int(event_data,"SEEN", time(NULL));
		ggzcore_conf_write_int(event_data,"LASTSEEN", time(NULL));
	} else {
		sprintf(out, "Welcome back, %s.", support_get_name(event_data));
		net_send_prvmsg(event_data, out);
		ggzcore_conf_write_int(event_data,"LASTSEEN", time(NULL));
		ggzcore_conf_read_list(event_data, "MESSAGES", &num_msg, &messages);
		if( num_msg > 0 )
		{
			sprintf(out, "You have %d messages waiting.", num_msg/2);
			net_send_prvmsg(event_data, out);
		}
	}

	/* Check if I should alert them */
	ggzcore_conf_read_list(event_data, "ALERTS", &num_alerts, &alerts);
	for(i=0; i<num_alerts; i++)
	{
		sprintf(out, "%s, %s is online.", support_get_name(alerts[i]), support_get_name(event_data));
		net_send_prvmsg(alerts[i], out);
		break;
	}

	if(grubby.logfile != NULL)
		net_send_prvmsg(event_data, "This room is being logged to file.");

	log_write("-->", event_data);
	return GGZ_HOOK_OK;
}


static GGZHookReturn net_room_leave(GGZRoomEvent id, void* event_data, void* user_data)
{
	ggzcore_conf_write_int(event_data,"LASTSEEN", time(NULL));
	log_write("<--", event_data);
	return GGZ_HOOK_OK;
}
