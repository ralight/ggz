/*
 * File: server.h
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 1/19/01
 *
 * Code for handling server connection state and properties
 *
 * Copyright (C) 2001 Brent Hendricks.
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

#ifndef __SERVER_H__
#define __SERVER_H__

#include "ggzcore.h"
#include "gametype.h"
#include "room.h"
#include "state.h"

struct _GGZServer* _ggzcore_server_new(void);

/* functions to get a GGZServer attribute */
struct _GGZNet* _ggzcore_server_get_net(struct _GGZServer *server);
GGZLoginType    _ggzcore_server_get_type(struct _GGZServer *server);
char*           _ggzcore_server_get_handle(struct _GGZServer *server);
char*           _ggzcore_server_get_password(struct _GGZServer *server);
GGZStateID      _ggzcore_server_get_state(struct _GGZServer *server);

int              _ggzcore_server_get_num_rooms(struct _GGZServer *server);
struct _GGZRoom* _ggzcore_server_get_cur_room(struct _GGZServer *server);
struct _GGZRoom* _ggzcore_server_get_nth_room(struct _GGZServer *server, 
					      const unsigned int num);
struct _GGZRoom* _ggzcore_server_get_room_by_id(struct _GGZServer *server,
						const unsigned int id);

int          _ggzcore_server_get_num_gametypes(struct _GGZServer *server);
struct _GGZGameType* _ggzcore_server_get_nth_gametype(struct _GGZServer *server, const unsigned int num);
struct _GGZGameType* _ggzcore_server_get_type_by_id(struct _GGZServer *server,
						    const unsigned int id);
						      
/* functions to set a GGZServer attribute */
void _ggzcore_server_set_logintype(struct _GGZServer *server, 
				   const GGZLoginType type);
void _ggzcore_server_set_handle(struct _GGZServer *server, 
				const char *handle);
void _ggzcore_server_set_password(struct _GGZServer *server, 
				  const char *password);

void _ggzcore_server_set_room(struct _GGZServer *server,
			      struct _GGZRoom *room);

/* functions to pass status of other information to server object */
void _ggzcore_server_set_negotiate_status(struct _GGZServer *server, struct _GGZNet *net, int status);
void _ggzcore_server_set_login_status(struct _GGZServer *server, 
				      int status);
void _ggzcore_server_set_room_join_status(struct _GGZServer *server, 
					  int status);
void _ggzcore_server_set_table_launching(struct _GGZServer *server);
void _ggzcore_server_set_table_joining(struct _GGZServer *server);
void _ggzcore_server_set_table_leaving(struct _GGZServer *server);
void _ggzcore_server_set_table_launch_status(struct _GGZServer *server, 
					     int status);
void _ggzcore_server_set_table_join_status(struct _GGZServer *server, 
					   int status);
void _ggzcore_server_set_table_leave_status(struct _GGZServer *server, 
					    int status);
void _ggzcore_server_session_over(struct _GGZServer *server, struct _GGZNet *net);

/* functions to perform an action */
int _ggzcore_server_log_session(struct _GGZServer *server, 
				const char *filename);
void _ggzcore_server_reset(struct _GGZServer *server);
int _ggzcore_server_connect(struct _GGZServer *server);
int _ggzcore_server_create_channel(struct _GGZServer *server);
int _ggzcore_server_login(struct _GGZServer *server);
int _ggzcore_server_load_motd(struct _GGZServer *server);
int _ggzcore_server_load_typelist(struct _GGZServer *server, 
				  const char verbose);
int _ggzcore_server_load_roomlist(struct _GGZServer *server,
				  const int type,
				  const char verbose);
int _ggzcore_server_join_room(struct _GGZServer *server, 
			      const unsigned int room);

int _ggzcore_server_logout(struct _GGZServer *server);
int _ggzcore_server_disconnect(struct _GGZServer *server);
void _ggzcore_server_net_error(struct _GGZServer *server, char *message);
void _ggzcore_server_protocol_error(struct _GGZServer *server, char *message);

void _ggzcore_server_clear(struct _GGZServer *server);

void _ggzcore_server_free(struct _GGZServer *server);




/* Functions for manipulating list of rooms */
 void _ggzcore_server_init_roomlist(struct _GGZServer *server,
					  const int num);
 void _ggzcore_server_free_roomlist(struct _GGZServer *server);
 void _ggzcore_server_add_room(struct _GGZServer *server, 
				     struct _GGZRoom *room);

/* Functions for manipulating list of gametypes */
 void _ggzcore_server_init_typelist(struct _GGZServer *server, 
					  const int num);
 void _ggzcore_server_free_typelist(struct _GGZServer *server);
 void _ggzcore_server_add_type(struct _GGZServer *server, 
				     struct _GGZGameType *type);


 int _ggzcore_server_event_is_valid(GGZServerEvent event);
 void _ggzcore_server_change_state(GGZServer *server, GGZTransID trans);
 GGZHookReturn _ggzcore_server_event(GGZServer*, GGZServerEvent, void*);

#endif /* __SERVER_H__ */
