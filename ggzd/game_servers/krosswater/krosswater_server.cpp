// Krosswater - Server for the Krosswater game
// Copyright (C) 2001 - 2004 Josef Spillner, josef@ggzgamingzone.org
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

#ifdef HAVE_CONFIG_H
#  include <config.h>			/* Site-specific config */
#endif

// Header file
#include "krosswater_server.h"

// System includes
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>

// Stackpath includes
#include "cwpathitem.h"

// Zone includes
#include "ZoneGGZ.h"
#include <ggzdmod.h>

// Gives a console output of the map
static void printpath(int width, int height, int **field)
{
	return;
	for(int j = 0; j < height; j++)
	{
		for(int i = 0; i < width; i++)
		{
			if(field[i][j]) std::cout << "\e[1m\e[39m";
			else std::cout << "\e[0m\e[39m";
			std::cout << field[i][j];
		}
		std::cout << endl;
	}
	std::cout << "\e[0m\e[39m";
}

// Constructor: setup game and enter main loop
KrosswaterServer::KrosswaterServer()
: ZoneGGZModServer()
{
	ZONEDEBUG(">> Krosswater: server launched\n");
	ZoneRegister("Krosswater", ZoneGGZ::turnbased, 4);

	map = NULL;
	path = NULL;
	players = NULL;

	zoneMainLoop();
}

// Destructor: free allocated memory
KrosswaterServer::~KrosswaterServer()
{
	eraseMap();
}

// Erase the map
void KrosswaterServer::eraseMap()
{
	if(map)
	{
		for(int i = 0; i < map_x; i++)
			free(map[i]);
		free(map);
		map = NULL;
	}
	if(players)
	{
		free(players);
		players = NULL;
	}
	if(path)
	{
		delete path;
		path = NULL;
	}
}

// Handle input from game client
int KrosswaterServer::slotZoneInput(int fd, int i)
{
	char string[256];
	int op;
	int status;

	if(ggz_read_int(fd, &op) < 0)
	{
		ZONEERROR("error in protocol (3)\n");
		exit(-1);
	}

	m_fd = fd;

	status = -1;
	if(op == proto_helloworld)
	{
		ZONEDEBUG("Hiya!\n");
		ggz_read_string(fd, (char*)&string, sizeof(string));
		ZONEDEBUG(":: %s ::\n", string);
		status = 0;
	}

	if(op == proto_move)
	{
		ZONEDEBUG("MOVE!!\n");
		getMove();
		status = 0;
	}

	if(op == proto_map)
	{
		ZONEDEBUG("MAP!!\n");
		sendMap();
		status = 0;
	}

	if(op == proto_restart)
	{
		ZONEDEBUG("RESTART!!\n");
		sendRestart();
		status = 0;
	}

	return status;
}

// Read a move from a client
void KrosswaterServer::getMove()
{
	int fromx, fromy, tox, toy;
	int valid;
	int ret;

	if((!map) || (!players)) return;

	// Read move coordinates
	if((ggz_read_int(m_fd, &fromx) < 0)
	|| (ggz_read_int(m_fd, &fromy) < 0)
	|| (ggz_read_int(m_fd, &tox) < 0)
	|| (ggz_read_int(m_fd, &toy) < 0))
	{
		ZONEERROR("Move corrupted!\n");
		return;
	}
	ZONEDEBUG("Client moved from %i/%i to %i/%i\n", fromx, fromy, tox, toy);
	ZONEDEBUG("That is: %i -> %i\n", map[fromx][fromy], map[tox][toy]);

	// Evaluate move
	valid = 1;
	if((fromx <= 0) || (fromy < 0) || (fromx >= map_x) || (fromy >= map_y)) valid = 0;
	if((tox < 0) || (toy < 0) || (tox >= map_x) || (toy >= map_y)) valid = 0;
	if((valid) && (map[fromx][fromy] == 0)) valid = 0;
	if((valid) && (map[tox][toy] != 0)) valid = 0;
	if((valid) && (map[fromx][fromy] == 2)) valid = 0;

	if(ggzdmod_count_seats(ggzdmod, GGZ_SEAT_OPEN) +
		ggzdmod_count_seats(ggzdmod, GGZ_SEAT_RESERVED) > 0) valid = 0;
	if(ggzdmod_get_seat(ggzdmod, zoneTurn()).fd != m_fd) valid = 0;

	ZONEDEBUG("XXXXX valid = %i, m_numplayers = %i, players+bots = %i", valid, m_numplayers,
		ggzdmod_count_seats(ggzdmod, GGZ_SEAT_PLAYER) + ggzdmod_count_seats(ggzdmod, GGZ_SEAT_BOT));

	// Process new situation
	if(valid)
	{
		if(!path)
		{
			ZONEERROR("Pathitem is NULL!\n");
			return;
		}
		ret = doMove(fromx, fromy, tox, toy);
		if(ret)
		{
			ZONEDEBUG("Player %i won the game!\n", zoneTurn());
		}
		else zoneNextTurn(true);
	}
	else
	{
  		if(ggz_write_int(m_fd, ZoneGGZ::invalid) < 0)
		{
			ZONEERROR("couldn't send ZoneGGZ::invalid\n");
			return;
		}
	}
}

// Send a restart to all players
void KrosswaterServer::sendRestart()
{
	eraseMap();
	createMap();
	createPlayers();

	for(int i = 0; i < m_maxplayers; i++)
	{
		GGZSeat seat = ggzdmod_get_seat(ggzdmod, i);
		if(seat.type == GGZ_SEAT_PLAYER)
		{
			m_fd = seat.fd;
			sendMap();
		}
	}

}

// Create the map
void KrosswaterServer::createMap()
{
	map_x = 40;
	map_y = 20;

	map = (int**)malloc(map_x * sizeof(int));
	for(int i = 0; i < map_x; i++)
		map[i] = (int*)malloc(map_y * sizeof(int));

	srandom(time(NULL));
	for(int j = 0; j < map_y; j++)
		for(int i = 0; i < map_x; i++)
			map[i][j] = rand() % 2;

	ZONEDEBUG("path >>> create\n");
	path = new CWPathitem(map_x, map_y, map);
}

// Setup players and bots
void KrosswaterServer::createPlayers()
{
	players = (struct player_t*)malloc(sizeof(struct player_t) * m_maxplayers);
	for(int i = 0; i < m_maxplayers; i++)
	{
		players[i].x = 0;
		players[i].y = path->available(0, CWPathitem::force);
		map[players[i].x][players[i].y] = 2;
	}

	// Force map to have at least one goal
	path->available(map_x - 1, CWPathitem::force);
}

// Broadcast map to joining players
void KrosswaterServer::sendMap()
{
	// Create the map and pass it to Stackpath
	if(!map) createMap();

	// Place all players on the map
	if(!players) createPlayers();

	// Send map information to the client who joined
	if((ggz_write_int(m_fd, proto_map_respond) < 0)
	|| (ggz_write_int(m_fd, map_x) < 0)
	|| (ggz_write_int(m_fd, map_y) < 0))
	{
		ZONEERROR("Map control transmission corrupted!\n");
		return;
	}

	// Send map data
	for(int j = 0; j < map_y; j++)
		for(int i = 0; i < map_x; i++)
			if(ggz_write_int(m_fd, map[i][j]) < 0)
			{
				ZONEERROR("Map transmission corrupted!\n");
				return;
			}

	// Send player data
	if(ggz_write_int(m_fd, m_maxplayers) < 0)
	{
		ZONEERROR("Player control transmission corrupted!\n");
		return;
	}
	for(int i = 0; i < m_maxplayers; i++)
		if((ggz_write_int(m_fd, players[i].x) < 0)
		|| (ggz_write_int(m_fd, players[i].y) < 0))
		{
			ZONEERROR("Player transmission corrupted!\n");
			return;
		}
}

// Handle AI player
void KrosswaterServer::slotZoneAI()
{
	int fromx, fromy, tox, toy;
	int ret;

	if(!map) createMap();
	if(!players) createPlayers();

	do
	{
		fromx = rand() % map_x;
		fromy = rand() % map_y;
	}
	while(map[fromx][fromy] != 1);
	do
	{
		tox = rand() % map_x;
		toy = rand() % map_y;
	}
	while(map[tox][toy] != 0);

	ZONEDEBUG("AI moved from: %i/%i to: %i/%i\n", fromx, fromy, tox, toy);
	ret = doMove(fromx, fromy, tox, toy);
	if(ret)
	{
		ZONEDEBUG("AI (%i) won the game!\n", zoneTurn());
	}
	else zoneNextTurn(true);
}

// Accept a move and broadcast it
int KrosswaterServer::doMove(int fromx, int fromy, int tox, int toy)
{
	int yl, yr;
	Pathitem *backtrace;
	int ret;

	if((!map) || (!path)) return 0;

	// broadcast changes here!
	for(int i = 0; i < m_maxplayers; i++)
	{
		GGZSeat seat = ggzdmod_get_seat(ggzdmod, i);
		if((seat.type == GGZ_SEAT_PLAYER) && (i != zoneTurn()))
	  		if((ggz_write_int(seat.fd, proto_move_broadcast) < 0)
			|| (ggz_write_int(seat.fd, fromx) < 0)
			|| (ggz_write_int(seat.fd, fromy) < 0)
			|| (ggz_write_int(seat.fd, tox) < 0)
			|| (ggz_write_int(seat.fd, toy) < 0))
				ZONEERROR("couldn't send move broadcast!\n");
	}

	map[fromx][fromy] = 0;
	map[tox][toy] = 1;

	ZONEDEBUG("path >>> setValue\n");
	path->setValue(fromx, fromy, 0);
	path->setValue(tox, toy, 1);
	//yl = path->available(0, CWPathitem::force);
	yl = players[zoneTurn()].y;
	yr = path->available(map_x - 1, CWPathitem::noforce);
	ZONEDEBUG("path >>> process\n");
	path->process(0, yl, map_x - 1, yr, Pathitem::normal | Pathitem::nostop);
	path->postprocess(map_x - 1);
	ZONEDEBUG("path >>> evaluating\n");
	ZONEDEBUG("from: %i/%i to: %i/*\n", 0, yl, map_x - 1);
	backtrace = path->result();

	if(backtrace)
	{
		//cout << "## Found the path (" << backtrace->x() << ", " << backtrace->y() << ") !!! Doing backtrace..." << endl;

		// prepare for backtrace broadcast
		for(int i = 0; i < m_maxplayers; i++)
		{
			GGZSeat seat = ggzdmod_get_seat(ggzdmod, i);
			if(seat.type == GGZ_SEAT_PLAYER)
	  			if((ggz_write_int(seat.fd, proto_map_backtrace) < 0)
				|| (ggz_write_int(seat.fd, zoneTurn()) < 0))
					ZONEERROR("couldn't send backtrace control\n");
		}

		do
		{
			// send everyone the good message
			//cout << backtrace->x() << "," << backtrace->y() << endl;
			for(int i = 0; i < m_maxplayers; i++)
			{
				GGZSeat seat = ggzdmod_get_seat(ggzdmod, i);
				if(seat.type == GGZ_SEAT_PLAYER)
				{
	  				if((ggz_write_int(seat.fd, backtrace->x()) < 0)
	  				|| (ggz_write_int(seat.fd, backtrace->y()) < 0))
						ZONEERROR("couldn't send part of backtrace\n");
				}
			}
			backtrace = backtrace->parent();
		}
		while(backtrace->parent() != NULL);

		// mark the end
		for(int i = 0; i < m_maxplayers; i++)
		{
			GGZSeat seat = ggzdmod_get_seat(ggzdmod, i);
			if(seat.type == GGZ_SEAT_PLAYER)
	  			if(ggz_write_int(seat.fd, -1) < 0)
					ZONEERROR("couldn't send backtrace control\n");
		}
		ret = 1;
	}
	else
	{
		//cout << "## Ooops, no way found :(" << endl;
		ret = 0;
	}

	printpath(map_x, map_y, map);
	path->clear();

	if(ret)
	{
		GGZGameResult results[m_maxplayers];
		for(int i = 0; i < m_maxplayers; i++)
			results[i] = GGZ_GAME_LOSS;
		results[zoneTurn()] = GGZ_GAME_WIN;
		ggzdmod_report_game(ggzdmod, NULL, results, NULL);

		for(int i = 0; i < m_maxplayers; i++)
		{
			GGZSeat seat = ggzdmod_get_seat(ggzdmod, i);
			if(seat.type == GGZ_SEAT_PLAYER)
	  			if(ggz_write_int(seat.fd, ZoneGGZ::over) < 0)
					ZONEERROR("couldn't send ZoneGGZ::over\n");
		}
	}

	return ret;
}

