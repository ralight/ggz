/*
    This file is part of the kggzcore library.
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
#include <kggzcore/table.h>
#include <kggzcore/player.h>

#include <QSocketNotifier>

using namespace KGGZCore;

RoomBase::RoomBase(QObject *parent)
: QObject(parent)
{
	m_room = NULL;
	m_game = NULL;
	m_sn = NULL;
}

RoomBase::~RoomBase()
{
	delete m_sn;
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
	ggzcore_room_list_tables(m_room);
}

void RoomBase::setup(GGZGame *game)
{
	ggzcore_game_add_event_hook_full(game, GGZ_GAME_LAUNCHED, &RoomBase::cb_game, this);
	ggzcore_game_add_event_hook_full(game, GGZ_GAME_LAUNCH_FAIL, &RoomBase::cb_game, this);
	ggzcore_game_add_event_hook_full(game, GGZ_GAME_NEGOTIATED, &RoomBase::cb_game, this);
	ggzcore_game_add_event_hook_full(game, GGZ_GAME_NEGOTIATE_FAIL, &RoomBase::cb_game, this);
	ggzcore_game_add_event_hook_full(game, GGZ_GAME_PLAYING, &RoomBase::cb_game, this);

	m_game = game;
}

void RoomBase::slotGameSocket(int socket)
{
	Q_UNUSED(socket);

	qDebug("--------------------------------");
	qDebug("----- game socket activity...");

	ggzcore_game_read_data(m_game);

	qDebug("----- game socket reading done.");
	qDebug("--------------------------------");
}

GGZRoom *RoomBase::room() const
{
	return m_room;
}

QList<Player> RoomBase::buildplayers()
{
	QList<Player> players;

	for(int i = 0; i < ggzcore_room_get_num_players(m_room); i++)
	{
		GGZPlayer *player = ggzcore_room_get_nth_player(m_room, i);
		QString name = ggzcore_player_get_name(player);
		//GGZSeatType type = ggzcore_player_get_type(player);
		//KGGZCore::Player::PlayerType ktype = (KGGZCore::Player::PlayerType)type;
		KGGZCore::Player::PlayerType ktype = KGGZCore::Player::none;
		//Player *p = new Player(name, ktype);
		Player p(name, ktype);
		players << p;
	}

	return players;
}

QList<Table> RoomBase::buildtables()
{
	QList<Table> tables;

	for(int i = 0; i < ggzcore_room_get_num_tables(m_room); i++)
	{
		GGZTable *t = ggzcore_room_get_nth_table(m_room, i);
		Table table(ggzcore_table_get_desc(t));
		for(int j = 0; j < ggzcore_table_get_num_seats(t); j++)
		{
			QString name = ggzcore_table_get_nth_player_name(t, j);
			GGZSeatType type = ggzcore_table_get_nth_player_type(t, j);
			KGGZCore::Player::PlayerType ktype = (KGGZCore::Player::PlayerType)type;
			//Player *p = new Player(name, ktype);
			Player p(name, ktype);
			table.addPlayer(p);
		}
		tables << table;
	}

	return tables;
}

void RoomBase::callback_game(unsigned int id, const void *event_data)
{
	qDebug("cb_game! id=%i event=%s", id, Misc::gamemessagename(id).toUtf8().data());

	Q_UNUSED(event_data);

	if(id == GGZ_GAME_LAUNCHED)
	{
		m_sn = new QSocketNotifier(ggzcore_game_get_control_fd(m_game), QSocketNotifier::Read, this);
		connect(m_sn, SIGNAL(activated(int)), SLOT(slotGameSocket(int)));

		//qDebug("##hack## read from fd %i", ggzcore_game_get_control_fd(m_game));
		// FIXME: hack
		//ggzcore_game_read_data(m_game);
	}
	else if(id == GGZ_GAME_NEGOTIATED)
	{
		GGZServer *server = ggzcore_room_get_server(room());
		ggzcore_server_create_channel(server);
	}

	emit signalBaseGame(id);
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

	if(id == GGZ_TABLE_LAUNCHED)
	{
		//GGZServer *server = ggzcore_room_get_server(room());
		//ggzcore_room_join_table(room, id, 0);
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

void RoomBase::handle_room_pre(unsigned int id) const
{
	Q_UNUSED(id);
}

void RoomBase::handle_room_post(unsigned int id) const
{
	Q_UNUSED(id);
}

GGZHookReturn RoomBase::cb_room(unsigned int id, const void *event_data, const void *user_data)
{
	qDebug("cb_room-static!");

	static_cast<const RoomBase*>(user_data)->callback_room(id, event_data);

	return GGZ_HOOK_OK;
}

GGZHookReturn RoomBase::cb_game(unsigned int id, const void *event_data, const void *user_data)
{
	qDebug("cb_game-static!");

	const RoomBase *base = static_cast<const RoomBase*>(user_data);
	RoomBase *ncbase = (RoomBase*)base;
	ncbase->callback_game(id, event_data);

	return GGZ_HOOK_OK;
}

