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

#ifndef KGGZCORE_GAMETYPE_H
#define KGGZCORE_GAMETYPE_H

#include <QString>
#include <QUrl>

#include "kggzcore_export.h"

namespace KGGZCore
{

// FIXME: valid* methods and named bots stuff

class KGGZCORE_EXPORT GameType
{
	public:
		GameType();
		~GameType();

		void setName(const QString& name);
		void setVersion(const QString& version);
		void setAuthor(const QString& author);
		void setDescription(const QString& description);
		void setUrl(const QUrl& url);

		void setProtocolEngine(const QString& protocolengine);
		void setProtocolVersion(const QString& protocolversion);

		void setMaxPlayers(int maxplayers);
		void setMaxBots(int maxbots);

		void setAllowSpectators(bool allowspectators);
		void setAllowPeers(bool allowpeers);

		QString name() const;
		QString version() const;
		QString author() const;
		QString description() const;
		QUrl url() const;

		QString protocolEngine() const;
		QString protocolVersion() const;

		int maxPlayers() const;
		int maxBots() const;

		bool allowSpectators() const;
		bool allowPeers() const;

	private:
		QString m_name;
		QString m_version;
		QString m_author;
		QString m_description;
		QUrl m_url;

		QString m_protocolengine;
		QString m_protocolversion;

		int m_maxplayers;
		int m_maxbots;

		bool m_allowspectators;
		bool m_allowpeers;
};

}

#endif
