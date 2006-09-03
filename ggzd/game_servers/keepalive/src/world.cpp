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
#include "spectator.h"
#include "protocol.h"

// Easysock includes
#include <ggz.h>

// System includes
#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <string>

// Configuration
#include "config.h"

#define KEEPALIVE_CHAT_LENGTH 1024

// Constructor
World::World()
{
	m_width = 500;
	m_height = 400;

	loadPlayers();
}

// Destructor
World::~World()
{
	m_playerlist.clear();
}

// Add a spectator
void World::addSpectator(const char *name, int fd)
{
	Spectator *s = new Spectator(name, fd);
	m_spectatorlist.push_back(*s);

	std::cout << "Transmit data to spectator " << name << std::endl;

	// Invite player into the game world
	ggz_write_char(s->fd(), op_spectator);

	// Transmit map data to the spectator
	ggz_write_char(s->fd(), op_map);
	ggz_write_int(s->fd(), width());
	ggz_write_int(s->fd(), height());

	// Transmit player list to the spectator
	ggz_write_char(s->fd(), op_player);
	ggz_write_int(s->fd(), m_playerlist.size());
	for(std::list<Player>::iterator it = m_playerlist.begin(); it != m_playerlist.end(); it++)
	{
		ggz_write_string(s->fd(), (*it).name());
		ggz_write_int(s->fd(), (*it).type());
		ggz_write_int(s->fd(), (*it).x());
		ggz_write_int(s->fd(), (*it).y());
	}
}

// Remove a spectator again
void World::removeSpectator(const char *name)
{
	for(std::list<Spectator>::iterator it = m_spectatorlist.begin(); it != m_spectatorlist.end(); it++)
	{
		if(!strcmp((*it).name(), name))
		{
			m_spectatorlist.erase(it);
			return;
		}
	}
}

// Add a new player to the world
void World::addPlayer(const char *name, int fd)
{
	// Either get player's grave or create new one
	Player *p = getPlayer(name);
	if(!p)
	{
		p = new Player(name, fd);
		m_playerlist.push_back(*p);
	}
	else
	{
		p->revive(fd);
	}
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
				std::cout << "Erase him (" << p->name() << ")" << std::endl;
				//m_playerlist.erase(it);
				p->die();

				std::cout << "Erased - broadcast death. " << std::endl;
				for(std::list<Player>::iterator it2 = m_playerlist.begin(); it2 != m_playerlist.end(); it2++)
				{
					if(&(*it2) == p) continue;
					if((*it2).type() == type_grave) continue;
					ggz_write_char((*it2).fd(), op_quit);
					ggz_write_string((*it2).fd(), name);
				}

				return;
			}
	}
	std::cout << "Player not found, not removed." << std::endl;
}

// Return a player from the list
Player *World::getPlayer(const char *name)
{
	if(!name) return NULL;

	for(std::list<Player>::iterator it = m_playerlist.begin(); it != m_playerlist.end(); it++)
		if(!strcmp((*it).name(), name)) return &(*it);

	return NULL;
}

// Return a spectator
Spectator *World::getSpectator(const char *name)
{
	if(!name) return NULL;

	for(std::list<Spectator>::iterator it = m_spectatorlist.begin(); it != m_spectatorlist.end(); it++)
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

// Load all initial graves
void World::loadPlayers()
{
	DIR *dp;
	struct dirent *ep;

	std::string graveyard = std::string(GGZDDATADIR) + "/keepalive/";

	dp = opendir(graveyard.c_str());
	if(!dp) return;
	while((ep = readdir(dp)) != NULL)
	{
		if(!strcmp(ep->d_name, ".")) continue;
		if(!strcmp(ep->d_name, "..")) continue;
		Player *p = new Player(ep->d_name, -1);
		p->automorph();
		m_playerlist.push_back(*p);
	}
	closedir(dp);
}

// Receive data from a client
void World::receive(const char *name, void *data)
{
	char c;
	int x, y;
	char username[32], password[32];
	char *pname, *message;
	Player *p;
	Spectator *s;
	int fd;

	p = getPlayer(name);
	if(!p)
	{
		s = getSpectator(name);
		if(!s) return;
		fd = s->fd();
	}
	else
	{
		s = NULL;
		fd = p->fd();
	}

	ggz_read_char(fd, &c);

	std::cout << "Got data from client: " << (int)c << std::endl;

	switch(c)
	{
		case op_login:
			if(s) return;
			std::cout << "op_login" << std::endl;
			ggz_read_string(fd, username, 32);
			ggz_read_string(fd, password, 32);
			pname = p->morph(username, password);
			if(pname)
			{
				// Send login confirmation
				ggz_write_char(fd, op_name);
				ggz_write_string(fd, pname);

				// Broadcast avatar name and position
				for(std::list<Player>::iterator it = m_playerlist.begin(); it != m_playerlist.end(); it++)
				{
					if((*it).type() == type_grave) continue;
					ggz_write_char((*it).fd(), op_moved);
					ggz_write_string((*it).fd(), name);
					ggz_write_int((*it).fd(), p->x());
					ggz_write_int((*it).fd(), p->y());
				}

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
				ggz_write_int(p->fd(), m_playerlist.size() - 1);
				for(std::list<Player>::iterator it = m_playerlist.begin(); it != m_playerlist.end(); it++)
				{
					if(&(*it) == p) continue;
std::cout << "Transmit: " << (*it).name() << " - " << (*it).type() << " - " << (*it).x() << ", " << (*it).y() << std::endl;
					ggz_write_string(p->fd(), (*it).name());
					ggz_write_int(p->fd(), (*it).type());
					ggz_write_int(p->fd(), (*it).x());
					ggz_write_int(p->fd(), (*it).y());
				}

				std::cout << "Ready with transmitting" << std::endl;
			}
			else
			{
				// Send login failure
				ggz_write_char(fd, op_loginfailed);
				std::cout << "sent login failed" << std::endl;
			}
			break;
		case op_move:
			if(s) return;
			std::cout << "op_move" << std::endl;
			ggz_read_int(fd, &x);
			ggz_read_int(fd, &y);
			p->move(x, y);

			// Broadcast again
			for(std::list<Player>::iterator it = m_playerlist.begin(); it != m_playerlist.end(); it++)
			{
				if(&(*it) == p) continue;
				if((*it).type() == type_grave) continue;
				ggz_write_char((*it).fd(), op_moved);
				ggz_write_string((*it).fd(), name);
				ggz_write_int((*it).fd(), x);
				ggz_write_int((*it).fd(), y);
			}
			break;
		case op_chat:
			if(s) return;
			std::cout << "op_chat" << std::endl;
			message = (char*)ggz_malloc(KEEPALIVE_CHAT_LENGTH + 5);
			ggz_read_string(fd, message, KEEPALIVE_CHAT_LENGTH);

			// Chat with all players alive
			for(std::list<Player>::iterator it = m_playerlist.begin(); it != m_playerlist.end(); it++)
			{
				if(&(*it) == p) continue;
				if((*it).type() == type_grave) continue;
				ggz_write_char((*it).fd(), op_chatted);
				ggz_write_string((*it).fd(), name);
				ggz_write_string((*it).fd(), message);
			}
			ggz_free(message);
			break;
		default:
			std::cerr << "Unknown opcode: " << (int)c << std::endl;
	}
}

