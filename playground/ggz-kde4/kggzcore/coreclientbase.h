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

#ifndef KGGZCORE_CORECLIENT_BASE_H
#define KGGZCORE_CORECLIENT_BASE_H

#include <QObject>

#include <ggzcore.h>

class QSocketNotifier;

namespace KGGZCore
{

class RoomBase;

class CoreClientBase : public QObject
{
	Q_OBJECT
	public:
		CoreClientBase(QObject *parent = NULL);
		~CoreClientBase();

		void setConnectionInfo(const char *host, int port, const char *username, const char *password, const char *email, GGZLoginType mode, int tls);
		void startConnection();
		void switchRoom(const char *name);

		QStringList roomnames();
		int state();

		RoomBase *roombase();

		QString textmotd();
		QString webmotd();

	signals:
		void signalBaseError();
		void signalBaseServer(int id, int code) const;

	private slots:
		void slotSocket(int socket);

	private:
		void init();

		void callback_server(unsigned int id, const void *event_data) const;
		void handle_server_pre(unsigned int id);
		void handle_server_post(unsigned int id);

		static GGZHookReturn cb_server(unsigned int id, const void *event_data, const void *user_data);

		GGZServer *m_server;
		QSocketNotifier *m_sn;
		RoomBase *m_roombase;
		QString m_motd, m_webmotd;
};

}

#endif
