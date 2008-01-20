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

#include <kggzcore/coreclientbase.h>

#include <kggzcore/misc.h>
#include <kggzcore/roombase.h>

#include <QSocketNotifier>
#include <QStringList>

using namespace KGGZCore;

CoreClientBase::CoreClientBase(QObject *parent)
: QObject(parent)
{
	m_server = NULL;
	m_sn = NULL;
	m_roombase = NULL;

	init();
}

CoreClientBase::~CoreClientBase()
{
	if(m_server)
		ggzcore_server_free(m_server);

	ggzcore_destroy();
}

void CoreClientBase::init()
{
	GGZOptions options;

	options.flags = (GGZOptionFlags)(GGZ_OPT_PARSER | GGZ_OPT_MODULES);

	int ret = ggzcore_init(options);
	if(ret == -1)
	{
		qWarning("Couldn't initialize ggzcore.");
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

void CoreClientBase::setConnectionInfo(const char *host, int port, const char *username, const char *password, const char *email, GGZLoginType mode, int tls)
{
	ggzcore_server_set_hostinfo(m_server, host, port, tls);
	ggzcore_server_set_logininfo(m_server, mode, username, password, email);
}

void CoreClientBase::startConnection()
{
	int ret = ggzcore_server_connect(m_server);
	if(ret == -1)
	{
		qWarning("Couldn't start connection to server.");
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

RoomBase *CoreClientBase::roombase()
{
	return m_roombase;
}

void CoreClientBase::callback_server(unsigned int id, const void *event_data) const
{
	qDebug("cb_server! id=%i event=%s", id, Misc::messagename(id).toUtf8().data());

	int errorcode = E_OK;

	if((id == GGZ_LOGIN_FAIL)
	|| (id == GGZ_ENTER_FAIL)
	|| (id == GGZ_PROTOCOL_ERROR)
	|| (id == GGZ_CHAT_FAIL))
	{
		errorcode = ((const GGZErrorEventData*)event_data)->status;
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
		m_roombase->setRoom(ggzcore_server_get_cur_room(m_server));
	}
	else if((id == GGZ_NET_ERROR)
	     || (id == GGZ_PROTOCOL_ERROR))
	{
		delete m_roombase;
		m_roombase = NULL;
	}
}

void CoreClientBase::handle_server_post(unsigned int id)
{
	if(id == GGZ_CONNECTED)
	{
		m_sn = new QSocketNotifier(ggzcore_server_get_fd(m_server), QSocketNotifier::Read, this);
		connect(m_sn, SIGNAL(activated(int)), SLOT(slotSocket(int)));
	}

	if(id == GGZ_NEGOTIATED)
	{
		ggzcore_server_login(m_server);
	}

	if(id == GGZ_LOGGED_IN)
	{
		ggzcore_server_list_rooms(m_server, 0, 1);
		ggzcore_server_list_gametypes(m_server, 1);
	}

	if(id == GGZ_LOGOUT)
	{
		delete m_sn;
	}
}

GGZHookReturn CoreClientBase::cb_server(unsigned int id, const void *event_data, const void *user_data)
{
	qDebug("cb_server-static!");

	static_cast<const CoreClientBase*>(user_data)->callback_server(id, event_data);

	return GGZ_HOOK_OK;
}

void CoreClientBase::slotSocket(int socket)
{
	qDebug("socket activity...");

	if(ggzcore_server_data_is_pending(m_server))
		ggzcore_server_read_data(m_server, socket);

	qDebug("socket reading done.");
}

