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

#include <kggzcore/room.h>

#include <kggzcore/roombase.h>
#include <kggzcore/table.h>
#include <kggzcore/player.h>

using namespace KGGZCore;

Room::Room(QObject *parent)
: QObject(parent)
{
	m_base = NULL;
}

Room::~Room()
{
}

QString Room::name()
{
	QString name = ggzcore_room_get_name(m_base->room());
	return name;
}

QString Room::description()
{
	QString desc = ggzcore_room_get_desc(m_base->room());
	return desc;
}

bool Room::restricted()
{
	return ggzcore_room_get_closed(m_base->room());
}

QList<Table*> Room::tables()
{
	return m_tables;
}

QList<Player*> Room::players()
{
	return m_players;
}

void Room::launchtable()
{
	GGZGameType *gametype = ggzcore_room_get_gametype(m_base->room());
	qDebug(">>launchtable>> gametype=%s", ggzcore_gametype_get_name(gametype));

	GGZTable *table = ggzcore_table_new();
	ggzcore_table_init(table, gametype, "*embedded*", 2);
	ggzcore_table_set_seat(table, 0, GGZ_SEAT_OPEN, "player");
	ggzcore_table_set_seat(table, 1, GGZ_SEAT_BOT, "aiplayer");

	int ret = ggzcore_room_launch_table(m_base->room(), table);
	qDebug(">>launchtable>> ret=%i", ret);
}

void Room::launchmodule()
{
	qDebug(">>launchmodule>>");
	GGZGame *game = ggzcore_game_new();
	GGZServer *server = ggzcore_room_get_server(m_base->room());
	ggzcore_game_init(game, server, NULL);

	connect(m_base, SIGNAL(signalBaseGame(int)), SLOT(slotBaseGame(int)));
	m_base->setup(game);

	int ret = ggzcore_game_launch(game);
	qDebug(">>launchmodule>> ret=%i", ret);
}

void Room::slotBaseGame(int id)
{
	if(id == GGZ_GAME_LAUNCHED)
	{
		emit signalModuleReady();
	}
	else if(id == GGZ_GAME_PLAYING)
	{
		emit signalTableReady();
	}
}

void Room::init(RoomBase *base)
{
	m_base = base;

	connect(m_base, SIGNAL(signalBaseError()), SLOT(slotBaseError()));
	connect(m_base, SIGNAL(signalBaseRoom(int, int)), SLOT(slotBaseRoom(int, int)));
}

void Room::slotBaseError()
{
	qDebug("room base error!");

	m_base->deleteLater();
	m_base = NULL;

	emit signalEvent(libraryerror);
}

void Room::slotBaseRoom(int id, int code)
{
	qDebug("room base feedback!");

	Q_UNUSED(code);

	Error::ErrorCode errorcode = Error::no_status;
	GGZRoomEvent xid = (GGZRoomEvent)id;

	switch(xid)
	{
		case GGZ_PLAYER_LIST:
			qDeleteAll(m_players);
			m_players = m_base->buildplayers();
			emit signalAnswer(playerlist);
			break;
		case GGZ_TABLE_LIST:
			qDeleteAll(m_tables);
			m_tables = m_base->buildtables();
			emit signalAnswer(tablelist);
			break;
		case GGZ_CHAT_EVENT:
			emit signalEvent(chat);
			break;
		case GGZ_ROOM_ENTER:
			qDeleteAll(m_players);
			m_players = m_base->buildplayers();
			emit signalEvent(enter);
			break;
		case GGZ_ROOM_LEAVE:
			qDeleteAll(m_players);
			m_players = m_base->buildplayers();
			emit signalEvent(leave);
			break;
		case GGZ_TABLE_UPDATE:
			qDeleteAll(m_tables);
			m_tables = m_base->buildtables();
			emit signalEvent(tableupdate);
			break;
		case GGZ_TABLE_LAUNCHED:
			emit signalFeedback(tablelaunched, errorcode);
			break;
		case GGZ_TABLE_LAUNCH_FAIL:
			emit signalFeedback(tablelaunched, errorcode);
			break;
		case GGZ_TABLE_JOINED:
			emit signalFeedback(tablejoined, errorcode);
			break;
		case GGZ_TABLE_JOIN_FAIL:
			emit signalFeedback(tablejoined, errorcode);
			break;
		case GGZ_TABLE_LEFT:
			emit signalFeedback(tableleft, errorcode);
			break;
		case GGZ_TABLE_LEAVE_FAIL:
			emit signalFeedback(tableleft, errorcode);
			break;
		case GGZ_PLAYER_LAG:
			emit signalEvent(playerlag);
			break;
		case GGZ_PLAYER_STATS:
			emit signalEvent(stats);
			break;
		case GGZ_PLAYER_COUNT:
			emit signalEvent(count);
			break;
		case GGZ_PLAYER_PERMS:
			emit signalEvent(perms);
			break;
	}
}

