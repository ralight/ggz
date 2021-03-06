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

#ifndef KGGZCORE_TABLE_H
#define KGGZCORE_TABLE_H

#include <QString>
#include <QObject>

#include <kggzcore/player.h>
#include "kggzcore_export.h"

namespace KGGZCore
{

//class CoreClient;

class KGGZCORE_EXPORT Table : public QObject
{
	Q_OBJECT
	public:
		Table();
		Table(const Table& table);
		Table& operator=(const Table& table);

		Table(const QString& description);
		~Table();

		QString description() const;

		void setSeats(QList<Player> seats);
		void addPlayer(const Player& player);
		QList<Player> players() const;

		//void launch(CoreClient *client);

	private:
		void init(const Table& table);

		QString m_description;
		QList<Player> m_players;
};

}

#endif
