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

#include "gametype.h"

using namespace KGGZCore;

GameType::GameType()
{
	m_maxplayers = 0;
	m_maxbots = 0;

	m_allowspectators = false;
	m_allowpeers = false;
}

GameType::~GameType()
{
}

void GameType::setName(const QString& name)
{
	m_name = name;
}

void GameType::setVersion(const QString& version)
{
	m_version = version;
}

void GameType::setAuthor(const QString& author)
{
	m_author = author;
}

void GameType::setDescription(const QString& description)
{
	m_description = description;
}

void GameType::setUrl(const QUrl& url)
{
	m_url = url;
}

void GameType::setProtocolEngine(const QString& protocolengine)
{
	m_protocolengine = protocolengine;
}

void GameType::setProtocolVersion(const QString& protocolversion)
{
	m_protocolversion = protocolversion;
}

void GameType::setMaxPlayers(int maxplayers)
{
	m_maxplayers = maxplayers;
}

void GameType::setMaxBots(int maxbots)
{
	m_maxbots = maxbots;
}

void GameType::setAllowSpectators(bool allowspectators)
{
	m_allowspectators = allowspectators;
}

void GameType::setAllowPeers(bool allowpeers)
{
	m_allowpeers = allowpeers;
}

QString GameType::name() const
{
	return m_name;
}

QString GameType::version() const
{
	return m_version;
}

QString GameType::author() const
{
	return m_author;
}

QString GameType::description() const
{
	return m_description;
}

QUrl GameType::url() const
{
	return m_url;
}

QString GameType::protocolEngine() const
{
	return m_protocolengine;
}

QString GameType::protocolVersion() const
{
	return m_protocolversion;
}

int GameType::maxPlayers() const
{
	return m_maxplayers;
}

int GameType::maxBots() const
{
	return m_maxbots;
}

bool GameType::allowSpectators() const
{
	return m_allowspectators;
}

bool GameType::allowPeers() const
{
	return m_allowpeers;
}

void GameType::addNamedBot(QString name, QString botclass)
{
	m_namedbots[name] = botclass;
}

QMap<QString, QString> GameType::namedBots() const
{
	return m_namedbots;
}
