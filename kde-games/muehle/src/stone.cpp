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

// Header file
#include "stone.h"

// Constructor
Stone::Stone()
{
	m_x = -1;
	m_y = -1;
	m_owner = -1;
}

// Destructor
Stone::~Stone()
{
}

// Move the stone to the coordinates x/y
void Stone::move(int x, int y)
{
	m_x = x;
	m_y = y;
}

// Assign an owner to the stone
void Stone::assign(int owner)
{
	m_owner = owner;
}

// Return the x coordinate
int Stone::x()
{
	return m_x;
}

// Return the y coordinate
int Stone::y()
{
	return m_y;
}

// Return the current owner
int Stone::owner()
{
	return m_owner;
}

