// Keepalive - Experimental GGZ game
// Copyright (C) 2002 Josef Spillner, dr_maux@users.sourceforge.net
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
#include "player.h"

// System includes
#include <cstdio>
#include <cstring>
#include <cstdlib>

// Constructor
Player::Player(const char *name, int fd)
{
	m_name = strdup(name);
	m_fd = fd;
	m_x = 0;
	m_y = 0;
	m_username = NULL;
}

// Destructor
Player::~Player()
{
	if(m_username) free(m_username);
	free(m_name);
}

// Move to the given position
void Player::move(int x, int y)
{
	m_x = x;
	m_y = y;
}

// Load an identity or create a new one
char *Player::morph(const char *username, const char *password)
{
	m_username = strdup(username);
	return m_username;
}

// Return x coordinate
int Player::x()
{
	return m_x;
}

// Return y coordinate
int Player::y()
{
	return m_y;
}

// Return player's fd
int Player::fd()
{
	return m_fd;
}

// Return player's name
char *Player::name()
{
	return m_name;
}


