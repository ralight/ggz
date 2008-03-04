/*
    This file is part of the kggzdcore library.
    Copyright (c) 2008 Josef Spillner <josef@ggzgamingzone.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include <kggzcore/misc.h>

#include <ggzcore.h>

using namespace KGGZCore;

// FIXME: replace all switches with hash lookups

static QString Misc::messagename(int id)
{
	QString s;

	switch(id)
	{
		case GGZ_CONNECTED:
			s = "connection succeeded";
			break;
		case GGZ_CONNECT_FAIL:
			s = "connection failed";
			break;
		case GGZ_NEGOTIATED:
			s = "negotiation succeeded";
			break;
		case GGZ_NEGOTIATE_FAIL:
			s = "negotiation failed";
			break;
		case GGZ_LOGGED_IN:
			s = "login succeeded";
			break;
		case GGZ_LOGIN_FAIL:
			s = "login failed";
			break;
		case GGZ_ENTERED:
			s = "entering room succeeded";
			break;
		case GGZ_ENTER_FAIL:
			s = "entering room failed";
			break;
		case GGZ_LOGOUT:
			s = "logout succeeded";
			break;
		case GGZ_CHAT_FAIL:
			s = "chat failed";
			break;
		case GGZ_CHANNEL_CONNECTED:
			s = "game channel connected (ignored)";
			break;
		case GGZ_CHANNEL_READY:
			s = "game channel connection succeeded";
			break;
		case GGZ_CHANNEL_FAIL:
			s = "game channel connection failed";
			break;
		case GGZ_MOTD_LOADED:
			s = "loaded the motd";
			break;
		case GGZ_ROOM_LIST:
			s = "loaded the room list";
			break;
		case GGZ_TYPE_LIST:
			s = "loaded the type list";
			break;
		case GGZ_NET_ERROR:
			s = "network error event";
			break;
		case GGZ_PROTOCOL_ERROR:
			s = "protocol error event";
			break;
		case GGZ_SERVER_ROOMS_CHANGED:
			s = "server rooms changed event";
			break;
		case GGZ_SERVER_PLAYERS_CHANGED:
			s = "server players changed event";
			break;
		case GGZ_STATE_CHANGE:
			s = "internal state changed event";
			break;
		case GGZ_NUM_SERVER_EVENTS:
			s = "*unused*";
			break;
	}

	return s;
}

static QString Misc::roommessagename(int id)
{
	QString s;

	switch(id)
	{
		case GGZ_PLAYER_LIST:
			s = "player list arrived";
			break;
		case GGZ_TABLE_LIST:
			s = "table list arrived";
			break;
		case GGZ_CHAT_EVENT:
			s = "chat event";
			break;
		case GGZ_ROOM_ENTER:
			s = "someone entered the room";
			break;
		case GGZ_ROOM_LEAVE:
			s = "someone left the room";
			break;
		case GGZ_TABLE_UPDATE:
			s = "table update";
			break;
		case GGZ_TABLE_LAUNCHED:
			s = "table launch succeeded";
			break;
		case GGZ_TABLE_LAUNCH_FAIL:
			s = "table launch failed";
			break;
		case GGZ_TABLE_JOINED:
			s = "table join succeeded";
			break;
		case GGZ_TABLE_JOIN_FAIL:
			s = "table join failed";
			break;
		case GGZ_TABLE_LEFT:
			s = "table leave succeeded";
			break;
		case GGZ_TABLE_LEAVE_FAIL:
			s = "table leave failed";
			break;
		case GGZ_PLAYER_LAG:
			s = "player lag";
			break;
		case GGZ_PLAYER_STATS:
			s = "player statistics";
			break;
		case GGZ_PLAYER_COUNT:
			s = "player count";
			break;
		case GGZ_PLAYER_PERMS:
			s = "player permissions";
			break;
	}

	return s;
}

static QString Misc::statename(int id)
{
	QString s;

	switch(id)
	{
		case GGZ_STATE_OFFLINE:
			s = "offline";
			break;
		case GGZ_STATE_CONNECTING:
			s = "connecting";
			break;
		case GGZ_STATE_RECONNECTING:
			s = "reconnecting";
			break;
		case GGZ_STATE_ONLINE:
			s = "online";
			break;
		case GGZ_STATE_LOGGING_IN:
			s = "logging in";
			break;
		case GGZ_STATE_LOGGED_IN:
			s = "logged in";
			break;
		case GGZ_STATE_ENTERING_ROOM:
			s = "entering room";
			break;
		case GGZ_STATE_IN_ROOM:
			s = "in room";
			break;
		case GGZ_STATE_BETWEEN_ROOMS:
			s = "between rows";
			break;
		case GGZ_STATE_LAUNCHING_TABLE:
			s = "launching table";
			break;
		case GGZ_STATE_JOINING_TABLE:
			s = "joining table";
			break;
		case GGZ_STATE_AT_TABLE:
			s = "at table";
			break;
		case GGZ_STATE_LEAVING_TABLE:
			s = "leaving table";
			break;
		case GGZ_STATE_LOGGING_OUT:
			s = "logging out";
			break;
	}

	return s;
}
