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

QList<Table> Room::tables() const
{
	return m_tables;
}

QList<Player> Room::players() const
{
	return m_players;
}

GameType Room::gametype() const
{
	GameType gametype;
	GGZGameType *ggzgametype = ggzcore_room_get_gametype(m_base->room());

	gametype.setName(ggzcore_gametype_get_name(ggzgametype));
	gametype.setVersion(ggzcore_gametype_get_version(ggzgametype));
	gametype.setAuthor(ggzcore_gametype_get_author(ggzgametype));
	gametype.setDescription(ggzcore_gametype_get_desc(ggzgametype));
	gametype.setUrl(QUrl(ggzcore_gametype_get_url(ggzgametype)));

	gametype.setProtocolEngine(ggzcore_gametype_get_prot_engine(ggzgametype));
	gametype.setProtocolVersion(ggzcore_gametype_get_prot_version(ggzgametype));

	gametype.setMaxPlayers(ggzcore_gametype_get_max_players(ggzgametype));
	gametype.setMaxBots(ggzcore_gametype_get_max_bots(ggzgametype));

	gametype.setAllowSpectators(ggzcore_gametype_get_spectators_allowed(ggzgametype));
	gametype.setAllowPeers(ggzcore_gametype_get_peers_allowed(ggzgametype));

	return gametype;
}

void Room::sendchat(QString message, QString receiver, ChatType type)
{
	GGZChatType chattype = GGZ_CHAT_UNKNOWN;

	switch(type)
	{
		case chatnormal:
			chattype = GGZ_CHAT_NORMAL;
			break;
		case chatannounce:
			chattype = GGZ_CHAT_ANNOUNCE;
			break;
		case chatprivate:
			chattype = GGZ_CHAT_PERSONAL;
			break;
		case chatbeep:
			chattype = GGZ_CHAT_BEEP;
			break;
		case chattable:
			chattype = GGZ_CHAT_TABLE;
			break;
		default:
			return;
	}

	ggzcore_room_chat(m_base->room(), chattype, receiver.toUtf8(), message.toUtf8());
}

void Room::launchtable(QList<Player> seats)
{
	GGZGameType *gametype = ggzcore_room_get_gametype(m_base->room());
	qDebug(">>launchtable>> gametype=%s", ggzcore_gametype_get_name(gametype));

	GGZTable *table = ggzcore_table_new();
	ggzcore_table_init(table, gametype, "*embedded*", seats.size());
	for(int i = 0; i < seats.size(); i++)
	{
		ggzcore_table_set_seat(table, i, (GGZSeatType)seats.at(i).type(), seats.at(i).name().toUtf8());
	}

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

	connect(m_base, SIGNAL(signalBaseRoomChat(QString, QString, KGGZCore::Room::ChatType)),
		SIGNAL(signalChat(QString, QString, KGGZCore::Room::ChatType)));
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
			//qDeleteAll(m_players);
			m_players = m_base->buildplayers();
			emit signalAnswer(playerlist);
			break;
		case GGZ_TABLE_LIST:
			//qDeleteAll(m_tables);
			m_tables = m_base->buildtables();
			emit signalAnswer(tablelist);
			break;
		case GGZ_CHAT_EVENT:
			emit signalEvent(chat);
			break;
		case GGZ_ROOM_ENTER:
			//qDeleteAll(m_players);
			m_players = m_base->buildplayers();
			emit signalEvent(enter);
			break;
		case GGZ_ROOM_LEAVE:
			//qDeleteAll(m_players);
			m_players = m_base->buildplayers();
			emit signalEvent(leave);
			break;
		case GGZ_TABLE_UPDATE:
			//qDeleteAll(m_tables);
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

