// GGZap - GGZ quick start application for the KDE panel
// Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


#ifndef GGZAP_GAME_H
#define GGZAP_GAME_H

#include <qstringlist.h>

// Abstract game class
class GGZapGame
{
	public:
		GGZapGame();
		~GGZapGame();

		void addGame(QString name, QString frontend);

		int count();
		QString name(int id);
		QString frontend(int id);

		void clear();

		void autoscan();

	private:
		QStringList m_gamelist;
		QStringList m_frontendlist;
};

#endif

