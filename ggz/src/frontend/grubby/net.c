/* net.c
 *
 * A GRUB ia A (G)GZ (R)eally (U)seful (B)ot
 *
 * Original: Rich Gade
 * ReWrite:  Justin Zaun
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

static GGZHookReturn net_login_ok(GGZEventID id, void*, void*);
static GGZHookReturn net_chat_msg(GGZEventID id, void*, void*);
static GGZHookReturn net_chat_prvmsg(GGZEventID id, void*, void*);
static GGZHookReturn net_room_enter(GGZEventID id, void*, void*);
static GGZHookReturn net_room_leave(GGZEventID id, void*, void*);
void net_register_hooks(void);

void net_send_msg(char *message)
{
	ggzcore_event_enqueue(GGZ_USER_CHAT, message, NULL);
}

void net_send_prvmsg(char *from, char *message)
{
	char **data;
	
	data = calloc(2, sizeof(char*));
	data[0] = strdup(from);
	data[1] = strdup(message);

	ggzcore_event_enqueue(GGZ_USER_CHAT_PRVMSG, data, free);
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
	ggzcore_event_enqueue(GGZ_USER_JOIN_ROOM, (void*)room_num, NULL);
}

void net_connect(void)
{
	GGZProfile *profile;

        /* Connect up to the GGZ server */
        profile = calloc(1, sizeof(GGZProfile));
        profile->host = strdup(grubby.host);
        profile->port = grubby.port;
        profile->type = GGZ_LOGIN_GUEST;
        profile->login = strdup(grubby.name);

        ggzcore_event_enqueue(GGZ_USER_LOGIN, profile, NULL);
        
        printf ("  |- Connecting to Server\n");
}



/*
 * Callbacks for GGZCORE
 */

void net_register_hooks(void)
{
	ggzcore_event_add_hook(GGZ_SERVER_LOGIN, net_login_ok);
	ggzcore_event_add_hook(GGZ_SERVER_CHAT_MSG, net_chat_msg);
	ggzcore_event_add_hook(GGZ_SERVER_CHAT_PRVMSG, net_chat_prvmsg);
	ggzcore_event_add_hook(GGZ_SERVER_ROOM_ENTER, net_room_enter);
	ggzcore_event_add_hook(GGZ_SERVER_ROOM_LEAVE, net_room_leave);
}


static GGZHookReturn net_login_ok(GGZEventID id, void* event_data, void* user_data)
{
	net_change_room(ggzcore_conf_read_int("GRUBBYSETTINGS", "AUTOJOIN", -1));
	return GGZ_HOOK_OK;
}

static GGZHookReturn net_chat_msg(GGZEventID id, void* event_data, void* user_data)
{
        char* player;
        char* message;
        
        player = ((char**)(event_data))[0];
        message = ((char**)(event_data))[1];

	log_write(player, message);
	chat_parse(player, message);
	return GGZ_HOOK_OK;
}

static GGZHookReturn net_chat_prvmsg(GGZEventID id, void* event_data, void* user_data)
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

static GGZHookReturn net_room_enter(GGZEventID id, void* event_data, void* user_data)
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


static GGZHookReturn net_room_leave(GGZEventID id, void* event_data, void* user_data)
{
	ggzcore_conf_write_int(event_data,"LASTSEEN", time(NULL));
	log_write("<--", event_data);
	return GGZ_HOOK_OK;
}
