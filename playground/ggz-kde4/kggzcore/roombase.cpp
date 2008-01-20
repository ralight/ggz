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

#include <kggzcore/roombase.h>

#include <kggzcore/misc.h>

using namespace KGGZCore;

RoomBase::RoomBase(QObject *parent)
: QObject(parent)
{
	m_room = NULL;
}

RoomBase::~RoomBase()
{
}

void RoomBase::setRoom(GGZRoom *room)
{
	m_room = room;

	ggzcore_room_add_event_hook_full(m_room, GGZ_PLAYER_LIST, &RoomBase::cb_room, this);
	ggzcore_room_add_event_hook_full(m_room, GGZ_TABLE_LIST, &RoomBase::cb_room, this);
	ggzcore_room_add_event_hook_full(m_room, GGZ_CHAT_EVENT, &RoomBase::cb_room, this);
	ggzcore_room_add_event_hook_full(m_room, GGZ_ROOM_ENTER, &RoomBase::cb_room, this);
	ggzcore_room_add_event_hook_full(m_room, GGZ_ROOM_LEAVE, &RoomBase::cb_room, this);
	ggzcore_room_add_event_hook_full(m_room, GGZ_TABLE_UPDATE, &RoomBase::cb_room, this);
	ggzcore_room_add_event_hook_full(m_room, GGZ_TABLE_LAUNCHED, &RoomBase::cb_room, this);
	ggzcore_room_add_event_hook_full(m_room, GGZ_TABLE_LAUNCH_FAIL, &RoomBase::cb_room, this);
	ggzcore_room_add_event_hook_full(m_room, GGZ_TABLE_JOINED, &RoomBase::cb_room, this);
	ggzcore_room_add_event_hook_full(m_room, GGZ_TABLE_JOIN_FAIL, &RoomBase::cb_room, this);
	ggzcore_room_add_event_hook_full(m_room, GGZ_TABLE_LEFT, &RoomBase::cb_room, this);
	ggzcore_room_add_event_hook_full(m_room, GGZ_TABLE_LEAVE_FAIL, &RoomBase::cb_room, this);
	ggzcore_room_add_event_hook_full(m_room, GGZ_PLAYER_LAG, &RoomBase::cb_room, this);
	ggzcore_room_add_event_hook_full(m_room, GGZ_PLAYER_STATS, &RoomBase::cb_room, this);
	ggzcore_room_add_event_hook_full(m_room, GGZ_PLAYER_COUNT, &RoomBase::cb_room, this);
	ggzcore_room_add_event_hook_full(m_room, GGZ_PLAYER_PERMS, &RoomBase::cb_room, this);

	ggzcore_room_list_players(m_room);
	ggzcore_room_list_tables(m_room, 0, 0);
}

GGZRoom *RoomBase::room() const
{
	return m_room;
}

void RoomBase::callback_room(unsigned int id, const void *event_data) const
{
	qDebug("cb_room! id=%i event=%s", id, Misc::roommessagename(id).toUtf8().data());

	int errorcode = E_OK;

	if(id == GGZ_CHAT_EVENT)
	{
		// FIXME: read GGZChatEventData
	}

	if((id == GGZ_ROOM_ENTER)
	|| (id == GGZ_ROOM_LEAVE))
	{
		// FIXME: read GGZRoomChangeEventData
	}

	if(id == GGZ_TABLE_LEFT)
	{
		// FIXME: read GGZTableLeaveEventData(?)
	}

	if((id == GGZ_PLAYER_LAG)
	|| (id == GGZ_PLAYER_STATS)
	|| (id == GGZ_PLAYER_PERMS))
	{
		// FIXME: read player name as string
	}

	if(id == GGZ_PLAYER_COUNT)
	{
		// FIXME: read GGZRoom*
	}

	if(id == GGZ_TABLE_LAUNCH_FAIL)
	{
		errorcode = ((const GGZErrorEventData*)event_data)->status;
	}

	if((id == GGZ_TABLE_JOIN_FAIL)
	|| (id == GGZ_TABLE_LEAVE_FAIL))
	{
		// FIXME: error data is a string
	}

	handle_room_pre(id);

	emit signalBaseRoom(id, errorcode);

	handle_room_post(id);
}

void RoomBase::handle_room_pre(unsigned int id)
{
	Q_UNUSED(id);
}

void RoomBase::handle_room_post(unsigned int id)
{
	Q_UNUSED(id);
}

GGZHookReturn RoomBase::cb_room(unsigned int id, const void *event_data, const void *user_data)
{
	qDebug("cb_room-static!");

	static_cast<const RoomBase*>(user_data)->callback_room(id, event_data);

	return GGZ_HOOK_OK;
}

