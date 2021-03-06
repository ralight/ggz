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

#include "table.h"

#include "player.h"

using namespace KGGZCore;

Table::Table(const QString& description)
{
	m_description = description;
}

Table::Table()
{
}

Table::Table(const Table& table)
: QObject()
{
	init(table);
}

Table& Table::operator=(const Table& table)
{
	init(table);
	return *this;
}

void Table::init(const Table& table)
{
	m_description = table.description();
	m_players = table.players();
}

Table::~Table()
{
}

QString Table::description() const
{
	return m_description;
}

void Table::setSeats(QList<Player> seats)
{
	m_players = seats;
}

void Table::addPlayer(const Player& p)
{
	m_players << p;
}

QList<Player> Table::players() const
{
	return m_players;
}

//void Table::launch(CoreClient *client)
//{
// FIXME: ???
//}

