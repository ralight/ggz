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

// Keepalive includes
#include "protocol.h"

// System includes
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <errno.h>
#include <fstream>
#include <string>
#include "config.h"
#include <sys/stat.h>
#include <iostream>

using namespace std;

// Constructor
Player::Player(const char *name, int fd)
{
	m_name = strdup(name);
	m_fd = fd;
	m_x = 0;
	m_y = 0;
	m_username = NULL;
	m_password = NULL;
	m_type = type_born;
}

// Destructor
Player::~Player()
{
	die();

	if(m_username) free(m_username);
	if(m_password) free(m_password);
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
	fstream f;
	string user, pass;

	std::cout << "Morph: " << username << ", " << password << std::endl;

	string graveyard = string(GGZDDATADIR) + "/keepalive/";
	string grave = graveyard + username;
	f.open(grave.c_str(), /*_IO_INPUT*/ios::in);
	if(f.is_open())
	{
std::cout << "grave is open... let's see who's inside" << std::endl;
		f >> user;
		f >> pass;
		f >> m_x;
		f >> m_y;
		f.close();
std::cout << "Does " << pass.c_str() << " match " << password << "?" << std::endl;
		if((password) && (pass != password))
		{
			return NULL;
		}
		else
		{
			m_username = strdup(user.c_str());
			m_password = strdup(pass.c_str());
			//m_x = 0;
			//m_y = 0;
		}
	}
	else
	{
		m_username = strdup(username);
		m_password = strdup(password);
		m_x = 0;
		m_y = 0;
	}

	if(password) m_type = type_avatar;
	else m_type = type_grave;

	return m_username;
}

void Player::automorph()
{
	morph(m_name, NULL);
}

// Put the player into the graveyard
void Player::die()
{
	fstream f;

	std::cout << "Die: " << m_username << ", " << m_password << std::endl;

	string graveyard = string(GGZDDATADIR) + "/keepalive/";
	std::cout << "mkdir " << graveyard << std::endl;
	if(mkdir(graveyard.c_str(), S_IRWXU))
	{
		std::cout << "mkdir failed" << std::endl;
		if(errno != EEXIST) return;
	}
	string grave = graveyard + m_username;
	f.open(grave.c_str(), ios::out);
	if(f.is_open())
	{
		//f << "username: " << m_username << endl;
		//f << "password: " << m_password << endl;
		//f << "x: " << m_x << endl;
		//f << "y: " << m_y << endl;
		f << m_username << endl;
		f << m_password << endl;
		f << m_x << endl;
		f << m_y << endl;
		f.close();
	}

	m_type = type_grave;
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

// Return player's username
char *Player::username()
{
	return m_username;
}

// Return type: born, avatar, grave
int Player::type()
{
	return m_type;
}

// Assign a new valid fd to a player
void Player::revive(int fd)
{
	m_fd = fd;
}

