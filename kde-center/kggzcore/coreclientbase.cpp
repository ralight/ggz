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

#include <kggzcore/coreclientbase.h>

#include <kggzcore/misc.h>
#include <kggzcore/roombase.h>
#include <kggzcore/room.h>
#include <kggzcore/module.h>

#include <QSocketNotifier>
#include <QStringList>

#define RECV_DEBUG 1
#if RECV_DEBUG
#include <sys/socket.h>
#endif

using namespace KGGZCore;

CoreClientBase::CoreClientBase(QObject *parent, bool embedded)
: QObject(parent)
{
	m_server = NULL;
	m_sn = NULL;
	m_channelsn = NULL;
	m_roombase = NULL;

	init(embedded);
}

CoreClientBase::~CoreClientBase()
{
	if(m_server)
		ggzcore_server_free(m_server);

	ggzcore_destroy();
}

void CoreClientBase::init(bool embedded)
{
	GGZOptions options;

	options.flags = (GGZOptionFlags)(GGZ_OPT_PARSER | GGZ_OPT_RECONNECT);
	if(embedded)
		options.flags = (GGZOptionFlags)(options.flags | GGZ_OPT_EMBEDDED);
	else
		options.flags = (GGZOptionFlags)(options.flags | GGZ_OPT_MODULES);

	int ret = ggzcore_init(options);
	if(ret == -1)
	{
		qWarning("base: Couldn't initialize ggzcore.");
		emit signalBaseError();
		return;
	}

	m_server = ggzcore_server_new();

	ggzcore_server_add_event_hook_full(m_server, GGZ_CONNECTED, &CoreClientBase::cb_server, this);
	ggzcore_server_add_event_hook_full(m_server, GGZ_CONNECT_FAIL, &CoreClientBase::cb_server, this);
	ggzcore_server_add_event_hook_full(m_server, GGZ_NEGOTIATED, &CoreClientBase::cb_server, this);
	ggzcore_server_add_event_hook_full(m_server, GGZ_NEGOTIATE_FAIL, &CoreClientBase::cb_server, this);
	ggzcore_server_add_event_hook_full(m_server, GGZ_LOGGED_IN, &CoreClientBase::cb_server, this);
	ggzcore_server_add_event_hook_full(m_server, GGZ_LOGIN_FAIL, &CoreClientBase::cb_server, this);
	ggzcore_server_add_event_hook_full(m_server, GGZ_MOTD_LOADED, &CoreClientBase::cb_server, this);
	ggzcore_server_add_event_hook_full(m_server, GGZ_ROOM_LIST, &CoreClientBase::cb_server, this);
	ggzcore_server_add_event_hook_full(m_server, GGZ_TYPE_LIST, &CoreClientBase::cb_server, this);
	ggzcore_server_add_event_hook_full(m_server, GGZ_SERVER_PLAYERS_CHANGED, &CoreClientBase::cb_server, this);
	ggzcore_server_add_event_hook_full(m_server, GGZ_ENTERED, &CoreClientBase::cb_server, this);
	ggzcore_server_add_event_hook_full(m_server, GGZ_ENTER_FAIL, &CoreClientBase::cb_server, this);
	ggzcore_server_add_event_hook_full(m_server, GGZ_LOGOUT, &CoreClientBase::cb_server, this);
	ggzcore_server_add_event_hook_full(m_server, GGZ_NET_ERROR, &CoreClientBase::cb_server, this);
	ggzcore_server_add_event_hook_full(m_server, GGZ_PROTOCOL_ERROR, &CoreClientBase::cb_server, this);
	ggzcore_server_add_event_hook_full(m_server, GGZ_CHAT_FAIL, &CoreClientBase::cb_server, this);
	ggzcore_server_add_event_hook_full(m_server, GGZ_STATE_CHANGE, &CoreClientBase::cb_server, this);
	ggzcore_server_add_event_hook_full(m_server, GGZ_CHANNEL_CONNECTED, &CoreClientBase::cb_server, this);
	ggzcore_server_add_event_hook_full(m_server, GGZ_CHANNEL_READY, &CoreClientBase::cb_server, this);
	ggzcore_server_add_event_hook_full(m_server, GGZ_CHANNEL_FAIL, &CoreClientBase::cb_server, this);
	ggzcore_server_add_event_hook_full(m_server, GGZ_SERVER_ROOMS_CHANGED, &CoreClientBase::cb_server, this);
}

void CoreClientBase::setConnectionInfo(const char *host, int port, const char *username, const char *password, const char *email, GGZLoginType mode, GGZConnectionPolicy policy)
{
	ggzcore_server_set_hostinfo(m_server, host, port, policy);
	ggzcore_server_set_logininfo(m_server, mode, username, password, email);
}

void CoreClientBase::startConnection()
{
	int ret = ggzcore_server_connect(m_server);
	if(ret == -1)
	{
		qWarning("base: Couldn't start connection to server.");
		emit signalBaseError();
		return;
	}
}

void CoreClientBase::switchRoom(const char *name)
{
	for(int i = 0; i < ggzcore_server_get_num_rooms(m_server); i++)
	{
		GGZRoom *room = ggzcore_server_get_nth_room(m_server, i);
		if(QString(name) == QString(ggzcore_room_get_name(room)))
		{
			ggzcore_server_join_room(m_server, room);
			break;
		}
	}
}

QStringList CoreClientBase::roomnames()
{
	QStringList names;
	for(int i = 0; i < ggzcore_server_get_num_rooms(m_server); i++)
	{
		GGZRoom *room = ggzcore_server_get_nth_room(m_server, i);
		names << ggzcore_room_get_name(room);
	}


	return names;
}

QList<Room*> CoreClientBase::rooms()
{
	QList<Room*> rooms;

	for(int i = 0; i < ggzcore_server_get_num_rooms(m_server); i++)
	{
		GGZRoom *ggzroom = ggzcore_server_get_nth_room(m_server, i);
		RoomBase *roombase = new RoomBase(this);
		roombase->setRoom(ggzroom, false);
		Room *room = new Room();
		room->init(roombase);
		rooms << room;
	}

	return rooms;
}

QList<Module> CoreClientBase::modules()
{
	QList<Module> modules;

	for(unsigned int i = 0; i < ggzcore_module_get_num(); i++)
	{
		GGZModule *ggzmodule = ggzcore_module_get_nth(i);
		Module module;
		module.setName(ggzcore_module_get_name(ggzmodule));
		modules << module;
	}

	return modules;
}

int CoreClientBase::state()
{
	return (int)(ggzcore_server_get_state(m_server));
}

QString CoreClientBase::textmotd()
{
	return m_motd;
}

QString CoreClientBase::webmotd()
{
	return m_webmotd;
}

RoomBase *CoreClientBase::roombase()
{
	return m_roombase;
}

void CoreClientBase::callback_server(unsigned int id, const void *event_data)
{
	qDebug("base: cb_server! id=%i event=%s", id, Misc::messagename(id).toUtf8().data());

	int errorcode = E_OK;

	if((id == GGZ_LOGIN_FAIL)
	|| (id == GGZ_ENTER_FAIL)
	|| (id == GGZ_PROTOCOL_ERROR)
	|| (id == GGZ_CHAT_FAIL))
	{
		errorcode = ((const GGZErrorEventData*)event_data)->status;
	}

	if(id == GGZ_MOTD_LOADED)
	{
		GGZMotdEventData *motddata = (GGZMotdEventData*)event_data;
		m_motd = motddata->motd;
		m_webmotd = motddata->url;
	}

	handle_server_pre(id);

	emit signalBaseServer(id, errorcode);

	handle_server_post(id);
}

void CoreClientBase::handle_server_pre(unsigned int id)
{
	if(id == GGZ_ENTERED)
	{
		delete m_roombase;
		m_roombase = new RoomBase(this);
		m_roombase->setRoom(ggzcore_server_get_cur_room(m_server), true);
	}
	else if((id == GGZ_NET_ERROR)
	     || (id == GGZ_PROTOCOL_ERROR))
	{
		delete m_roombase;
		m_roombase = NULL;

		delete m_sn;
		m_sn = NULL;
	}
}

void CoreClientBase::handle_server_post(unsigned int id)
{
	if(id == GGZ_CONNECTED)
	{
		m_sn = new QSocketNotifier(ggzcore_server_get_fd(m_server), QSocketNotifier::Read, this);
		connect(m_sn, SIGNAL(activated(int)), SLOT(slotSocket(int)));
	}

	if(id == GGZ_CHANNEL_CONNECTED)
	{
		m_channelsn = new QSocketNotifier(ggzcore_server_get_channel(m_server), QSocketNotifier::Read, this);
		connect(m_channelsn, SIGNAL(activated(int)), SLOT(slotChannel(int)));
	}

	if(id == GGZ_CHANNEL_READY)
	{
		disconnect(m_channelsn);
		m_channelsn->deleteLater();
		m_channelsn = NULL;

		GGZGame *game = ggzcore_server_get_cur_game(m_server);
		ggzcore_game_set_server_fd(game, ggzcore_server_get_channel(m_server));
	}

	if(id == GGZ_NEGOTIATED)
	{
		ggzcore_server_login(m_server);
	}

	if(id == GGZ_LOGGED_IN)
	{
		ggzcore_server_list_rooms(m_server, 1);
		ggzcore_server_list_gametypes(m_server, 1);
	}

	if(id == GGZ_LOGOUT)
	{
		disconnect(m_sn);
		m_sn->deleteLater();
		m_sn = NULL;
	}
}

GGZHookReturn CoreClientBase::cb_server(unsigned int id, const void *event_data, const void *user_data)
{
	qDebug("base: cb_server-static!");

	const CoreClientBase *base = static_cast<const CoreClientBase*>(user_data);
	CoreClientBase *ncbase = (CoreClientBase*)base;
	ncbase->callback_server(id, event_data);

	return GGZ_HOOK_OK;
}

void CoreClientBase::slotSocket(int socket)
{
	qDebug("--------------------------------");
	qDebug("----- socket activity...");

#if RECV_DEBUG
	char buf[4096];
	int n = recv(socket, buf, sizeof(buf) - 1, MSG_PEEK);
	qDebug("PEEKED: %i", n);
	if(n > 0)
	{
		buf[n] = '\0';
		qDebug("PEEKBUFFER: '%s'", buf);
	}
#endif

	if(ggzcore_server_data_is_pending(m_server))
		ggzcore_server_read_data(m_server, socket);

	qDebug("----- socket reading done.");
	qDebug("--------------------------------");
}

void CoreClientBase::slotChannel(int socket)
{
	qDebug("--------------------------------");
	qDebug("----- channel activity...");

	ggzcore_server_read_data(m_server, socket);

	qDebug("----- channel reading done.");
	qDebug("--------------------------------");
}

