/*
 * File: user.c
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 9/16/00
 *
 * Code for responding to user events
 *
 * Copyright (C) 2000 Brent Hendricks.
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


#include <config.h>
#include <ggzcore.h>
#include <user.h>
#include <msg.h>
#include <net.h>
#include <state.h>
#include <protocol.h>

#include <stdlib.h>
#include <string.h>

/* Hooks */
static GGZHookReturn _ggzcore_user_list_players(GGZEventID, void*, void*);
static GGZHookReturn _ggzcore_user_motd(GGZEventID, void*, void*);


/* ggzcore_user_register() - Register hooks for UI events
 *
 * Receives:
 *
 * Returns:
 */
void _ggzcore_user_register(void)
{
	ggzcore_event_add_hook(GGZ_USER_LIST_PLAYERS, 
				   _ggzcore_user_list_players);
	ggzcore_event_add_hook(GGZ_USER_MOTD, _ggzcore_user_motd);
}


/* _ggzcore_user_list_players() - Hook for user player-list request
 *
 * Receives:
 * GGZEventID id    : ID code of triggered event
 * void* event_data : Event-specific data
 * void* user_data  : "User" data
 *
 * Returns:
 */
static GGZHookReturn _ggzcore_user_list_players(GGZEventID id, void* event_data, void* user_data)
{
	ggzcore_debug(GGZ_DBG_USER, "Executing user_list_players");
	_ggzcore_net_send_list_players(-1);

	return GGZ_HOOK_OK;
}


/* _ggzcore_user_motd() - Hook for user motd request
 *
 * Receives:
 * GGZEventID id    : ID code of triggered event
 * void* event_data : Event-specific data
 * void* user_data  : "User" data
 *
 * Returns:
 */
static GGZHookReturn _ggzcore_user_motd(GGZEventID id, void* event_data, void* user_data)
{
	ggzcore_debug(GGZ_DBG_USER, "Executing user_motd");	
	_ggzcore_net_send_motd(-1);

	return GGZ_HOOK_OK;
}
