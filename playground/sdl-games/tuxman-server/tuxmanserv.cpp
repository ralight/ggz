// Tuxman Server
// Copyright (C) 2003 Josef Spillner <josef@ggzgamingzone.org>
// Tuxman Copyright (C) 2003 Robert Strobl <badwolf@acoderz.de>
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
#include "tuxmanserv.h"
#include "net.h"
#include "proto.h"
#include "pacman.h"
#include "map.h"

// GGZ includes
#include <ggz.h>

// System includes
#include <iostream>

// Constructor: inherit from ggzgameserver
TuxmanServer::TuxmanServer()
: GGZGameServer()
{
	join_lock = false;
	net = new Net();
	pac = new Pacman();
}

// Destructor
TuxmanServer::~TuxmanServer()
{
	delete pac;
	delete net;
}

// Player join hook
void TuxmanServer::joinEvent(int player)
{
	std::cout << "Tuxman: joinEvent" << std::endl;

	if(join_lock)
	{
		close(fd(player));
		return;
	}
	join_lock = true;
	*net << Net::channel << fd(player);
	*net << map_list;
	*net << "default";
}

// Player leave event
void TuxmanServer::leaveEvent(int player)
{
	std::cout << "Tuxman: leaveEvent" << std::endl;

	join_lock = false;

	// ouch :)
	delete this;
}

// Game data event
void TuxmanServer::dataEvent(int player)
{
	int opcode;
	char *map;
	int move;
	int oldx, oldy;

	std::cout << "Tuxman: dataEvent" << std::endl;

	*net << Net::channel << fd(player);
	*net >> &opcode;

	switch(opcode)
	{
		case map_selected:
			*net >> &map;
			startGame(TUXMANDATA "/level9.pac");
			//startGame(map); // discard map choice for now :)
			ggz_free(map);
			break;
		case map_move:
			*net >> &move;
			std::cout << "got move: " << move << std::endl;
			oldx = pac->x();
			oldy = pac->y();
			switch(move)
			{
				case move_up:
					pac->move(pac->x(), pac->y() - 1);
					*net << map_pacman << oldx << oldy << move_up;
					break;
				case move_down:
					pac->move(pac->x(), pac->y() + 1);
					*net << map_pacman << oldx << oldy << move_down;
					break;
				case move_left:
					pac->move(pac->x() - 1, pac->y());
					*net << map_pacman << oldx << oldy << move_left;
					break;
				case move_right:
					pac->move(pac->x() + 1, pac->y());
					*net << map_pacman << oldx << oldy << move_right;
					break;
				default:
					*net << map_event << event_error << error_net;
					close(fd(player));
			}
			break;
		default:
			*net << map_event << event_error << error_net;
			close(fd(player));
	}
}

void TuxmanServer::idleEvent()
{
	std::cout << "(idle)" << std::endl;
}

// Error handling event
void TuxmanServer::errorEvent()
{
	std::cout << "Tuxman: errorEvent" << std::endl;
}

void TuxmanServer::startGame(const char *mapfile)
{
	bool success;

	Map map;
	success = map.load(mapfile);
	if(success)
	{
		std::cout << "loading map " << mapfile << " ok" << std::endl;
		map.dump();
		*net << map_event << event_start;
	}
	else
	{
		std::cout << "loading map " << mapfile << " failed" << std::endl;
		*net << map_event << event_error << error_map;
		//close(fd(player));
	}
}

