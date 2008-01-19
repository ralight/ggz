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
