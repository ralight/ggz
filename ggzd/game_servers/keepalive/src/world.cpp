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
#include "world.h"

// Keepalive includes
#include "player.h"
#include "protocol.h"

// Easysock includes
#include <ggz.h>

// System includes
#include <iostream>

// Constructor
World::World()
{
	m_width = 500;
	m_height = 400;
}

// Destructor
World::~World()
{
	m_playerlist.clear();
}

// Add a new player to the world
void World::addPlayer(const char *name, int fd)
{
	Player *p = new Player(name, fd);

	std::cout << "Transmit data to player " << name << std::endl;

	// Transmit map data
	ggz_write_char(p->fd(), op_map);
	ggz_write_int(p->fd(), width());
	ggz_write_int(p->fd(), height());

	// Transmit initialization data
	ggz_write_char(p->fd(), op_init);
	ggz_write_int(p->fd(), p->x());
	ggz_write_int(p->fd(), p->y());

	// Transmit player list
	ggz_write_char(p->fd(), op_player);
	ggz_write_int(p->fd(), m_playerlist.size());
	for(std::list<Player>::iterator it = m_playerlist.begin(); it != m_playerlist.end(); it++)
	{
		ggz_write_string((*it).fd(), (*it).name());
		ggz_write_int((*it).fd(), (*it).x());
		ggz_write_int((*it).fd(), (*it).y());
	}

	m_playerlist.push_back(*p);

	std::cout << "Ready with transmitting" << std::endl;
}

// Suspend a player's operation until he joins again
void World::removePlayer(const char *name)
{
	std::cout << "Remove player " << name << std::endl;
	Player *p = getPlayer(name);
	if(p)
	{
		std::cout << "Got him" << std::endl;
		for(std::list<Player>::iterator it = m_playerlist.begin(); it != m_playerlist.end(); it++)
			if(&(*it) == p)
			{
				std::cout << "Erase him" << std::endl;
				m_playerlist.erase(it);
				delete p;
				return;
			}
	}
}

// Return a player from the list
Player *World::getPlayer(const char *name)
{
	if(!name) return NULL;
	for(std::list<Player>::iterator it = m_playerlist.begin(); it != m_playerlist.end(); it++)
		if(!strcmp((*it).name(), name)) return &(*it);
	return NULL;
}

// Return the width of this world
int World::width()
{
	return m_width;
}

// Return the height of this world
int World::height()
{
	return m_height;
}

// Receive data from a client
void World::receive(const char *name, void *data)
{
	char c;
	int x, y;
	char username[32], password[32];
	char *pname, *message;
	int seat;
	Player *p;

	p = getPlayer(name);
	if(!p) return;
	ggz_read_char(p->fd(), &c);

	std::cout << "Got data from client: " << (int)c << std::endl;

	switch(c)
	{
		case op_login:
			std::cout << "op_login" << std::endl;
			ggz_read_string(p->fd(), username, 32);
			ggz_read_string(p->fd(), password, 32);
			pname = p->morph(username, password);
			if(pname)
			{
				ggz_write_char(p->fd(), op_name);
				ggz_write_string(p->fd(), pname);
			}
			else
			{
				ggz_write_char(p->fd(), op_loginfailed);
			}
			break;
		case op_move:
			std::cout << "op_move" << std::endl;
			ggz_read_int(p->fd(), &x);
			ggz_read_int(p->fd(), &y);
			p->move(x, y);
			for(std::list<Player>::iterator it = m_playerlist.begin(); it != m_playerlist.end(); it++)
			{
				if(&(*it) == p) continue;
				ggz_write_char((*it).fd(), op_moved);
				ggz_write_string((*it).fd(), name);
				ggz_write_int((*it).fd(), x);
				ggz_write_int((*it).fd(), y);
			}
			break;
		case op_chat:
			std::cout << "op_chat" << std::endl;
			ggz_read_string_alloc(p->fd(), &message);
			for(std::list<Player>::iterator it = m_playerlist.begin(); it != m_playerlist.end(); it++)
			{
				if(&(*it) == p) continue;
				ggz_write_char((*it).fd(), op_chatted);
				ggz_write_string((*it).fd(), name);
				ggz_write_string((*it).fd(), message);
			}
			free(message);
			break;
		default:
			std::cerr << "Unknown opcode: " << (int)c << std::endl;
	}
}

