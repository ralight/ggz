// Muehle - KDE Muehle (Nine Men's Morris) game for GGZ
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

#ifndef MUEHLE_TOPLEVEL_H
#define MUEHLE_TOPLEVEL_H

// KDE includes
#include <kmainwindow.h>

// Qt includes
#include <qstringlist.h>

// Class declarations
class Board;

// Main window which controls the game
class Toplevel : public KMainWindow
{
	Q_OBJECT
	public:
		Toplevel();
		~Toplevel();
		enum MenuItems
		{
			menugamenew = 1,
			menugamequit = 2,
			menuplayerremis = 3,
			menuplayerloose = 4,
			menuthemes = 100,
			menuvariants = 200
		};

	public slots:
		void slotMenu(int id);

	private:
		Board *board;
		QStringList themelist, variantslist;
};

#endif

