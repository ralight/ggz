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

	if(!join_lock)
	{
		close(fd(player));
		return;
	}
	join_lock = false;
	*net << Net::channel << fd(player);
	*net << map_list;
	*net << "default";
}

// Player leave event
void TuxmanServer::leaveEvent(int player)
{
	std::cout << "Tuxman: leaveEvent" << std::endl;

	// ouch :)
	delete this;
}

// Game data event
void TuxmanServer::dataEvent(int player)
{
	int opcode;
	char *map;
	int move;

	std::cout << "Tuxman: dataEvent" << std::endl;

	*net << Net::channel << fd(player);
	*net >> &opcode;

	switch(opcode)
	{
		case map_selected:
			*net >> &map;
			ggz_free(map);
			*net << event_start;
			break;
		case map_move:
			*net >> &move;
			switch(move)
			{
				case move_up:
					pac->move(pac->x(), pac->y() - 1);
					break;
				case move_down:
					pac->move(pac->x(), pac->y() + 1);
					break;
				case move_left:
					pac->move(pac->x() - 1, pac->y());
					break;
				case move_right:
					pac->move(pac->x() + 1, pac->y());
					break;
				default:
					*net << map_event << event_error;
					close(fd(player));
			}
			break;
		default:
			*net << map_event << event_error;
			close(fd(player));
	}
}

// Error handling event
void TuxmanServer::errorEvent()
{
	std::cout << "Tuxman: errorEvent" << std::endl;
}

