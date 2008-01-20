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

#include <kggzcore/coreclient.h>

#include <kggzcore/coreclientbase.h>
#include <kggzcore/room.h>
//#include <kggzcore/roombase.h>

#include <QUrl>

using namespace KGGZCore;

CoreClient::CoreClient(QObject *parent)
: QObject(parent)
{
	m_room = NULL;

	m_base = new CoreClientBase(this);

	m_port = 5688;
	m_mode = guest;

	connect(m_base, SIGNAL(signalBaseError()), SLOT(slotBaseError()));
	connect(m_base, SIGNAL(signalBaseServer(int, int)), SLOT(slotBaseServer(int, int)));
}

CoreClient::~CoreClient()
{
}

void CoreClient::setUrl(QString url)
{
	QUrl qurl(url);
	m_host = qurl.host();
	m_port = qurl.port();
	m_username = qurl.userName();
	m_password = qurl.password();
}

void CoreClient::setHost(QString host)
{
	m_host = host;
}

void CoreClient::setPort(int port)
{
	m_port = port;
}

void CoreClient::setUsername(QString username)
{
	m_username = username;
}

void CoreClient::setPassword(QString password)
{
	m_password = password;
}

void CoreClient::setEmail(QString email)
{
	m_email = email;
}

void CoreClient::setTLS(bool tls)
{
	m_tls = tls;
}

void CoreClient::setMode(LoginType mode)
{
	m_mode = mode;
}

void CoreClient::initiateLogin()
{
	int tls;
	GGZLoginType mode;

	if(m_tls) tls = 1;
	else tls = 0;

	if(m_mode == guest) mode = GGZ_LOGIN_GUEST;
	else if(m_mode == normal) mode = GGZ_LOGIN;
	else if(m_mode == firsttime) mode = GGZ_LOGIN_NEW;

	m_base->setConnectionInfo(m_host.toUtf8().data(), m_port, m_username.toUtf8().data(),
		m_password.toUtf8().data(), m_email.toUtf8().data(), mode, tls);
	m_base->startConnection();
}

void CoreClient::initiateRoomChange(QString roomname)
{
	m_base->switchRoom(roomname.toUtf8().data());
}

void CoreClient::initiateLogout()
{
}

QStringList CoreClient::roomnames()
{
	return m_roomnames;
}

Room *CoreClient::room()
{
	return m_room;
}

void CoreClient::slotBaseError()
{
	qDebug("base error!");

	m_base->deleteLater();
	m_base = NULL;

	emit signalEvent(libraryerror);
}

void CoreClient::slotBaseServer(int id, int code)
{
	qDebug("base feedback!");

	Q_UNUSED(code);

	Error::ErrorCode errorcode = Error::no_status;
	GGZServerEvent xid = (GGZServerEvent)id;

	switch(xid)
	{
		case GGZ_CONNECTED:
			emit signalFeedback(connection, errorcode);
			break;
		case GGZ_CONNECT_FAIL:
			emit signalFeedback(connection, errorcode);
			break;
		case GGZ_NEGOTIATED:
			emit signalFeedback(negotiation, errorcode);
			break;
		case GGZ_NEGOTIATE_FAIL:
			emit signalFeedback(negotiation, errorcode);
			break;
		case GGZ_LOGGED_IN:
			emit signalFeedback(login, errorcode);
			break;
		case GGZ_LOGIN_FAIL:
			emit signalFeedback(login, errorcode);
			break;
		case GGZ_ENTERED:
			delete m_room;
			m_room = new Room(this);
			m_room->init(m_base->roombase());
			emit signalFeedback(roomenter, errorcode);
			break;
		case GGZ_ENTER_FAIL:
			emit signalFeedback(roomenter, errorcode);
			break;
		case GGZ_LOGOUT:
			delete m_room;
			m_room = NULL;
			emit signalFeedback(logout, errorcode);
			break;
		case GGZ_CHAT_FAIL:
			emit signalFeedback(chat, errorcode);
			break;
		case GGZ_CHANNEL_CONNECTED:
			// skip
			break;
		case GGZ_CHANNEL_READY:
			emit signalFeedback(channel, errorcode);
			break;
		case GGZ_CHANNEL_FAIL:
			emit signalFeedback(channel, errorcode);
			break;
		case GGZ_MOTD_LOADED:
			emit signalAnswer(motd);
			break;
		case GGZ_ROOM_LIST:
			m_roomnames = m_base->roomnames();
			emit signalAnswer(roomlist);
			break;
		case GGZ_TYPE_LIST:
			emit signalAnswer(typelist);
			break;
		case GGZ_NET_ERROR:
			delete m_room;
			m_room = NULL;
			emit signalEvent(neterror);
			break;
		case GGZ_PROTOCOL_ERROR:
			delete m_room;
			m_room = NULL;
			emit signalEvent(protoerror);
			break;
		case GGZ_SERVER_ROOMS_CHANGED:
			emit signalEvent(rooms_changed);
			break;
		case GGZ_SERVER_PLAYERS_CHANGED:
			emit signalEvent(players_changed);
			break;
		case GGZ_STATE_CHANGE:
			emit signalEvent(state_changed);
			break;
		case GGZ_NUM_SERVER_EVENTS:
			// skip
			break;
	}
}

