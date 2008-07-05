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

#ifndef KGGZCORE_ROOM_H
#define KGGZCORE_ROOM_H

#include <QString>
#include <QObject>

#include <kggzcore/error.h>

namespace KGGZCore
{

class RoomBase;
class Table;

class Room : public QObject
{
	Q_OBJECT
	public:
		Room(QObject *parent = NULL);
		~Room();

		void init(RoomBase *base);

		enum AnswerMessage
		{
			playerlist,
			tablelist
		};

		enum FeedbackMessage
		{
			tablelaunched,
			tablejoined,
			tableleft
		};

		enum EventMessage
		{
			chat,
			enter,
			leave,
			tableupdate,
			playerlag,
			stats,
			count,
			perms,
			libraryerror
		};

		enum ChatType
		{
			chatnormal,
			chatannounce,
			chatprivate,
			chatbeep,
			chattable,
			chatunknown
		};

		enum AdminType
		{
			admingag,
			adminungag,
			adminkick
		};

		QString name();
		QString description();
		int numplayers();
		bool restricted();

		QList<Table*> tables();

	signals:
		void signalFeedback(KGGZCore::Room::FeedbackMessage message, KGGZCore::Error::ErrorCode error);
		void signalAnswer(KGGZCore::Room::AnswerMessage message);
		void signalEvent(KGGZCore::Room::EventMessage message);

	private slots:
		void slotBaseError();
		void slotBaseRoom(int id, int code);

	private:
		RoomBase *m_base;
		QList<Table*> m_tables;
};

}

#endif
