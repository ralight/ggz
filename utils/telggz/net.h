/*
 * TelGGZ - The GGZ Gaming Zone Telnet Wrapper
 * Copyright (C) 2001 Josef Spillner, dr_maux@users.sourceforge.net

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef NET_H
#define NET_H

/* GGZ includes */
#include <ggzcore.h>

/* Initialize the network structures */
void net_init();

/* Login with the specified parameters */
void net_login(const char *username, const char *password);

/* Main server loop */
void net_process();

/* Send a string to the GGZ server */
void net_send(char *buffer);

/* Toggle between chat input and read mode */
void net_allow(int allow);

/* Join the given room */
void net_join(int roomnum);

/* Set the host name */
void net_host(const char *hostname);

/* List all available rooms */
void net_list();

/* List all players in a room */
void net_who();

/* Hooks for ggzcore */
GGZHookReturn net_hook_connect(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_failure(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_login(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_roomlist(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_enter(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_roomenter(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_roomleave(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_chat(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_prvmsg(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_players(unsigned int id, void *event_data, void *user_data);

#endif

