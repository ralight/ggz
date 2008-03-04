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

#ifndef KGGZCORE_CORECLIENT_H
#define KGGZCORE_CORECLIENT_H

#include <QObject>
#include <QStringList>
#include <QString>

#include <kggzcore/error.h>

namespace KGGZCore
{

class CoreClientBase;
class Room;

class CoreClient : public QObject
{
	Q_OBJECT
	public:
		CoreClient(QObject *parent = NULL);
		~CoreClient();

		enum FeedbackMessage
		{
			connection,
			negotiation,
			login,
			roomenter,
			chat,
			channel,
			logout,
		};

		enum AnswerMessage
		{
			motd,
			roomlist,
			typelist
		};

		enum EventMessage
		{
			state_changed,
			players_changed,
			rooms_changed,
			neterror,
			protoerror,
			libraryerror
		};

		enum LoginType
		{
			normal,
			guest,
			firsttime
		};

	signals:
		void signalFeedback(KGGZCore::CoreClient::FeedbackMessage message, KGGZCore::Error::ErrorCode error);
		void signalAnswer(KGGZCore::CoreClient::AnswerMessage message);
		void signalEvent(KGGZCore::CoreClient::EventMessage message);

	public:
		void setUrl(QString url);
		void setHost(QString host);
		void setPort(int port);
		void setUsername(QString username);
		void setPassword(QString password);
		void setEmail(QString email);
		void setTLS(bool tls);
		void setMode(LoginType mode);

		void initiateLogin();
		void initiateRoomChange(QString roomname);
		void initiateLogout();

		QStringList roomnames();
		Room *room();

		QString textmotd();
		QString webmotd();

		// FIXME: we don't have an enum yet
		int state();

	private slots:
		void slotBaseError();
		void slotBaseServer(int id, int code);

	private:
		QString m_host, m_username, m_password, m_email;
		int m_port;
		bool m_tls;
		LoginType m_mode;
		CoreClientBase *m_base;
		QStringList m_roomnames;
		Room *m_room;
};

}

#endif
