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
static GGZHookReturn _ggzcore_user_chat(GGZEventID, void*, void*);


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

	ggzcore_event_add_hook_full(GGZ_USER_CHAT, _ggzcore_user_chat, 
					(void*)GGZ_CHAT_NORMAL);
	ggzcore_event_add_hook_full(GGZ_USER_CHAT_PRVMSG, 
					_ggzcore_user_chat,
					(void*)GGZ_CHAT_PERSONAL);
	ggzcore_event_add_hook_full(GGZ_USER_CHAT_BEEP, _ggzcore_user_chat,
					(void*)GGZ_CHAT_BEEP);
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


/* _ggzcore_user_chat() - Hook for user chat events
 *
 * Receives:
 * GGZEventID id    : ID code of triggered event
 * void* event_data : Event-specific data
 * void* user_data  : "User" data
 *
 * Returns:
 *
 * Note: The chat type gets passed in the user data as an integer
 */
static GGZHookReturn _ggzcore_user_chat(GGZEventID id, void* event_data, void* user_data)
{
	GGZUpdateOp opcode = (int)user_data;
	char *player = NULL, *msg = NULL;
	
	
	ggzcore_debug(GGZ_DBG_USER, "Executing user_chat");

	switch (opcode) {
	case GGZ_CHAT_NORMAL:
		msg = (char*)event_data;
		ggzcore_debug(GGZ_DBG_USER, " --chat is %s", msg);
		break;

	case GGZ_CHAT_BEEP:
		player = (char*)event_data;
		ggzcore_debug(GGZ_DBG_USER, " --player is %s", player);
		break;
		
	case GGZ_CHAT_PERSONAL:
		player = ((char**)(event_data))[0];
		msg    = ((char**)(event_data))[1];
		ggzcore_debug(GGZ_DBG_USER, " --msg is %s to %s", msg, player);
		
	default:
		break;
	}

	_ggzcore_net_send_chat(-1, opcode, player, msg);

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
