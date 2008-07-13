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

#ifndef MUEHLE_STONE_H
#define MUEHLE_STONE_H

// A stone on a Muehle board
class Stone
{
	public:
		Stone();
		~Stone();
		enum Owners
		{
			black,
			white,
			blackactive,
			whiteactive,
			blackidle,
			whiteidle,
			blackmuehle,
			whitemuehle
		};
		void move(int x, int y);
		void assign(int owner);
		int x();
		int y();
		int owner();

	private:
		int m_x, m_y;
		int m_owner;
};

#endif

