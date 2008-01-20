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

#ifndef KGGZCORE_ROOM_BASE_H
#define KGGZCORE_ROOM_BASE_H

#include <ggzcore.h>

#include <QObject>

namespace KGGZCore
{

class RoomBase : public QObject
{
	Q_OBJECT
	public:
		RoomBase(QObject *parent = NULL);
		~RoomBase();

		void setRoom(GGZRoom *room);
		GGZRoom *room() const;

	signals:
		void signalBaseError();
		void signalBaseRoom(int id, int code) const;

	private:
		void callback_room(unsigned int id, const void *event_data) const;
		void handle_room_pre(unsigned int id);
		void handle_room_post(unsigned int id);

		static GGZHookReturn cb_room(unsigned int id, const void *event_data, const void *user_data);

		GGZRoom *m_room;
};

}

#endif
