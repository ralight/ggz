// Geekgame - a game which only real geeks understand
// Copyright (C) 2002 - 2004 Josef Spillner, josef@ggzgamingzone.org
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
#include "geekgame.h"

// Geekgame includes
#include "protocol.h"

// GGZ includes
#include <ggz.h>

// System includes
#include <iostream>
#include <cstdlib>

// Constructor: inherit from ggzgameserver
Geekgame::Geekgame()
: GGZGameServer()
{
}

// Destructor
Geekgame::~Geekgame()
{
}

// State change hook
void Geekgame::stateEvent()
{
	std::cout << "Geekgame: stateEvent" << std::endl;
}

// Player join hook
void Geekgame::joinEvent(int player)
{
	std::cout << "Geekgame: joinEvent" << std::endl;

	// Send greeter
	int channel = fd(player);
	ggz_write_char(channel, op_server_greeting);
	ggz_write_string(channel, "geekgame");
	ggz_write_int(channel, GEEKGAME_PROTOCOL);
}

// Player leave event
void Geekgame::leaveEvent(int player)
{
	std::cout << "Geekgame: leaveEvent" << std::endl;

	// Stop the game for the time being
	game_stop();
}

// Spectator join event
void Geekgame::spectatorJoinEvent(int spectator)
{
	std::cout << "Geekgame: spectatorJoinEvent" << std::endl;
}

// Spectator leave event
void Geekgame::spectatorLeaveEvent(int spectator)
{
	std::cout << "Geekgame: spectatorLeaveEvent" << std::endl;
}

// Spectator data event (ignored)
void Geekgame::spectatorDataEvent(int spectator)
{
}

// Game data event
void Geekgame::dataEvent(int player)
{
	char opcode;
	char playername[256], playerpic[256];
	int x, y, result;

	std::cout << "Geekgame: dataEvent" << std::endl;

	// Read data
	int channel = fd(player);

	ggz_read_char(channel, &opcode);
	switch(opcode)
	{
		case op_client_presentation:
			ggz_read_string(channel, playername, 256);
			ggz_read_string(channel, playerpic, 256);

			// Broadcast number of total seats first
			for(int i = 0; i < players(); i++)
			{
				int bfd = fd(i);
				if(bfd == -1) continue;
				ggz_write_char(bfd, op_server_numplayers);
				ggz_write_int(bfd, players());
			}

			// Broadcast
			for(int i = 0; i < players(); i++)
			{
				int bfd = fd(i);
				if(bfd == -1) continue;
				ggz_write_char(bfd, op_server_newplayer);
				ggz_write_string(bfd, playername);
				ggz_write_string(bfd, playerpic);
			}

			// If ready, announce game start
			if(playercount(Seat::open) == 0) game_start();
			break;
		case op_client_ruleset:
			ggz_read_string_alloc(channel, &ruleset);
			// check_mode(ruleset)
			ggz_write_char(channel, op_server_moderesult);
			ggz_write_int(channel, 1);
			break;
		case op_client_move:
			ggz_read_int(channel, &x);
			ggz_read_int(channel, &y);
			result = map_check(x, y);
			ggz_write_char(channel, op_server_moveresult);
			ggz_write_int(channel, result);
			break;
		default:
			// Discard
			break;
	}
}

// Error handling event
void Geekgame::errorEvent()
{
	std::cout << "Geekgame: errorEvent" << std::endl;
}

void Geekgame::game_start()
{
	for(int j = 0; j < ARRAY_HEIGHT; j++)
		for(int k = 0; k < ARRAY_WIDTH; k++)
			m_array[k][j] = rand() % 2;

	for(int i = 0; i < players(); i++)
	{
		int bfd = fd(i);
		if(bfd == -1) continue;
		ggz_write_char(bfd, op_server_gamestart);

		ggz_write_int(bfd, ARRAY_HEIGHT);
		ggz_write_int(bfd, ARRAY_WIDTH);

		for(int j = 0; j < ARRAY_HEIGHT; j++)
			for(int k = 0; k < ARRAY_WIDTH; k++)
				ggz_write_int(bfd, m_array[k][j]);
	}
}

void Geekgame::game_stop()
{
	for(int i = 0; i < players(); i++)
	{
		int bfd = fd(i);
		if(bfd == -1) continue;
		ggz_write_char(bfd, op_server_gamestop);
	}
}

void Geekgame::game_end()
{
	for(int i = 0; i < players(); i++)
	{
		int bfd = fd(i);
		if(bfd == -1) continue;
		ggz_write_char(bfd, op_server_gameend);
		//ggz_write_int(bfd, winner);
	}
}

int Geekgame::map_check(int x, int y)
{
	int sum, sum2, makescore;
	int i, j;

	makescore = 0;

	switch(m_mode)
	{
		case op_mode_easy:
			sum = 0;
			i = x / 32;
			for(j = 0; j < ARRAY_HEIGHT; j++)
				sum += m_array[i][j];
			if(!sum) makescore = 1;

			sum = 0;
			j = y / 32;
			for(i = 0; i < ARRAY_WIDTH; i++)
				sum += m_array[i][j];
			if(!sum) makescore = 1;
			break;
		case op_mode_matrix:
			sum = 0;
			i = x / 32;
			for(j = 0; j < ARRAY_HEIGHT; j++)
				sum += m_array[i][j];

			j = y / 32;
			for(i = 0; i < ARRAY_WIDTH; i++)
				sum += m_array[i][j];

			if(sum * 2 == 42) makescore = 1;
			break;
		case op_mode_havoc:
			sum = 0;
			for(j = y / 32 - 3; j <= y / 32 + 3; j++)
				for(i = x / 32 - 3; i <= x / 32 + 3; i++)
				{
					if((i < 0) || (i >= ARRAY_WIDTH)) continue;
					if((j < 0) || (j >= ARRAY_HEIGHT)) continue;
					if((i == x / 32) || (j == y / 32))
					{
						sum += m_array[i][j];
					}
				}
			if(sum == (sum & ~0x03)) makescore = 1;
			break;
		case op_mode_haxor:
			sum = 0;
			sum2 = 0;
			for(j = y / 32 - 3; j <= y / 32 + 3; j++)
			{
				if((j < 0) || (j >= ARRAY_HEIGHT)) continue;
				sum += m_array[x / 32][j];
			}
			for(i = x / 32 - 3; i <= x / 32 + 3; i++)
			{
				if((i < 0) || (i >= ARRAY_WIDTH)) continue;
				sum2 += m_array[i][y / 32];
			}
			if(sum == sum2) makescore = 1;
			break;
		default:
			makescore = 0;
	}

	return makescore;
}

