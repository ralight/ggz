// Muehle - KDE Muehle (Nine Men's Morris) game for GGZ
// Copyright (C) 2001 - 2004 Josef Spillner <josef@ggzgamingzone.org>
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

#ifndef MUEHLE_TOPLEVEL_H
#define MUEHLE_TOPLEVEL_H

// KDE includes
#include <kmainwindow.h>

// Qt includes
#include <qstringlist.h>

// Class declarations
class Board;
class KPopupMenu;

// Main window which controls the game
class Toplevel : public KMainWindow
{
	Q_OBJECT
	public:
		Toplevel();
		~Toplevel();
		void enableNetwork(bool enabled);
		enum MenuItems
		{
			menugamenew = 1,
			menugamelevels = 2,
			menugamequit = 3,
			menuplayerremis = 4,
			menuplayerloose = 5,
			menuthemes = 100,
			menuvariants = 200
		};
		enum StatusItems
		{
			statusplayer = 1,
			statusopponent = 2,
			statushint = 3
		};

	public slots:
		void slotMenu(int id);
		void slotStatus(const QString &message);
		void slotScore(const QString &player, int num, int score, int stones);
		void slotEnd();

	private:
		Board *board;
		QStringList themelist, variantslist;
		KPopupMenu *menu_variants, *menu_game;
};

#endif

