// Widelands server for GGZ
// Copyright (C) 2004 Josef Spillner <josef@ggzgamingzone.org>
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
#include "widelands_server.h"

// Widelands includes
#include "protocol.h"

// GGZ includes
#include <ggz.h>

// System includes
#include <iostream>
#include <cstdlib>

// Constructor: inherit from ggzgameserver
WidelandsServer::WidelandsServer()
: GGZGameServer()
{
	m_ip = NULL;
}

// Destructor
WidelandsServer::~WidelandsServer()
{
	if(m_ip) ggz_free(m_ip);
}

// State change hook
void WidelandsServer::stateEvent()
{
	std::cout << "WidelandsServer: stateEvent" << std::endl;
}

// Player join hook
void WidelandsServer::joinEvent(int player)
{
	std::cout << "WidelandsServer: joinEvent" << std::endl;

	// Send greeter
	int channel = fd(player);
	ggz_write_int(channel, op_greeting);
	ggz_write_string(channel, "widelands server");
	ggz_write_int(channel, WIDELANDS_PROTOCOL);

	if(!player) ggz_write_int(channel, op_request_ip);
	else
	{
		ggz_write_int(channel, op_broadcast_ip);
		ggz_write_string(channel, m_ip);
	}
}

// Player leave event
void WidelandsServer::leaveEvent(int player)
{
	std::cout << "WidelandsServer: leaveEvent" << std::endl;
}

// Spectator join event (ignored)
void WidelandsServer::spectatorJoinEvent(int spectator)
{
}

// Spectator leave event (ignored)
void WidelandsServer::spectatorLeaveEvent(int spectator)
{
}

// Spectator data event (ignored)
void WidelandsServer::spectatorDataEvent(int spectator)
{
}

// Game data event
void WidelandsServer::dataEvent(int player)
{
	int opcode;
	char *ip;

	std::cout << "WidelandsServer: dataEvent" << std::endl;

	// Read data
	int channel = fd(player);

	ggz_read_int(channel, &opcode);
	switch(opcode)
	{
		case op_reply_ip:
			ggz_read_string_alloc(channel, &ip);
			std::cout << "IP: " << ip << std::endl;
			m_ip = ggz_strdup(ip);
			ggz_free(ip);
			break;
		default:
			// Discard
			std::cerr << "ERROR!" << std::endl;
			break;
	}
}

// Error handling event
void WidelandsServer::errorEvent()
{
	std::cout << "WidelandsServer: errorEvent" << std::endl;
}

void WidelandsServer::game_start()
{
}

void WidelandsServer::game_stop()
{
}

void WidelandsServer::game_end()
{
}

