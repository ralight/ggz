// ZoneServer - small C++ game support library
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

// Zone includes
#include "ZoneGGZModServer.h"
#include "ZoneGGZ.h"

// System includes
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Global singleton Zone object
ZoneGGZModServer *self;

// Constructor: initialize basic stuff and setup hooks
ZoneGGZModServer::ZoneGGZModServer()
{
	m_gamemode = ZoneGGZ::unitialized;
	m_turn = -1;
	m_state = ZoneGGZ::preinit;
	m_numplayers = 0;
	m_players = NULL;
	m_gamename = NULL;
	m_ready = 0;

	ggzdmod = ggzdmod_new(GGZDMOD_GAME);
	ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_STATE, &hook_event);
	ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_JOIN, &hook_event);
	ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_LEAVE, &hook_event);
	ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_PLAYER_DATA, &hook_data);

	self = this;
}

// Destructor: free allocated memory
ZoneGGZModServer::~ZoneGGZModServer()
{
	ggzdmod_free(ggzdmod);

	if(m_players) free(m_players);
}

// Delegate for actual game server
int ZoneGGZModServer::slotZoneInput(int fd, int i)
{
	return 0; /* silence compiler warning */
}

// Delegate for actual game server
void ZoneGGZModServer::slotZoneAI()
{
}

// Start game server and enter the main loop
int ZoneGGZModServer::zoneMainLoop()
{
	if(!m_gamename)
	{
		ZONEERROR("server module not registered yet!\n");
		return -1;
	}

	if(ggzdmod_connect(ggzdmod) < 0)
	{
		ZONEERROR("ggzdmod_connect\n");
		return -1;
	}

	ZONEDEBUG("entering main loop\n");
	ggzdmod_loop(ggzdmod);

	ggzdmod_disconnect(ggzdmod);

	return 0;
}

// Handle updates for the game server
int ZoneGGZModServer::game_update(int event, void* data)
{
	int seat;

	switch(event)
	{
		case ZoneGGZ::launch:
			if((m_state != ZoneGGZ::init) && (m_state != ZoneGGZ::preinit))
			{
				ZONEERROR("launching game when state not init/preinit\n");
				return -1;
			}
			m_state = ZoneGGZ::wait;
			break;
		case ZoneGGZ::join:
			ZONEDEBUG("## pre-join\n");
			if (m_state != ZoneGGZ::wait) return -1;
			ZONEDEBUG("## post-join\n");
			seat = *(int*)data;
			game_send_seat(seat);
			game_send_players();
			if(m_gamemode != ZoneGGZ::unitialized) game_send_rules();
			if(m_ready == 1)
			{
				m_ready = 2;
				zoneNextTurn();
			}
			break;
		case ZoneGGZ::leave:
			if (m_state == ZoneGGZ::playing)
			{
				m_state = ZoneGGZ::wait;
				game_send_players();
			}
			break;
	}

	return 0;
}

// Send out seat assignment
int ZoneGGZModServer::game_send_seat(int seat)
{
	GGZSeat ggzseat = ggzdmod_get_seat(ggzdmod, seat);
	int fd = ggzseat.fd;

	ZONEDEBUG("Sending player %d's seat num.\n", seat);

	if (ggz_write_int(fd, ZoneGGZ::seat) < 0 || ggz_write_int(fd, seat) < 0) return -1;

	ZONEDEBUG("RAW OUTPUT: %i (seat)\n", ZoneGGZ::seat);
	ZONEDEBUG("RAW OUTPUT: %i (seatnumber)\n", seat);

	return 0;
}

// Send out player list to everybody
int ZoneGGZModServer::game_send_players()
{
	int i, j, fd;

	m_numplayers = 0;

	for(i = 0; i < m_maxplayers; i++)
	{
		GGZSeat seat = ggzdmod_get_seat(ggzdmod, i);
		fd = seat.fd;
		if(seat.type = GGZ_SEAT_PLAYER)
		{
			m_numplayers++;
			ZONEDEBUG("seat number: %i (step %i) => %i\n", fd, i, m_numplayers);
		}
	}
	ZONEDEBUG("%i players found; sending number to clients...\n", m_numplayers);

	if(m_players) free(m_players);
	m_players = (int*)malloc(m_numplayers * sizeof(int));

	for (j = 0; j < m_numplayers; j++)
	{
		m_players[j] = 1;
		GGZSeat seat = ggzdmod_get_seat(ggzdmod, j);
		fd = seat.fd;
		if(fd == -1)
		{
			ZONEDEBUG("Player %i is a un/bot-assigned (%i).\n", j, seat.type);
			continue;
		}

		ZONEDEBUG("Sending player list to player %d (%i)\n", j, fd);

		if(ggz_write_int(fd, ZoneGGZ::players) < 0)
		{
			ZONEERROR("couldn't send ZoneGGZ::players\n");
			return -1;
		}
		if(ggz_write_int(fd, m_numplayers) < 0)
		{
			ZONEERROR("couldn't send player number\n");
			return -1;
		}

		m_ready = 1;
		for(i = 0; i < m_numplayers; i++)
		{
			GGZSeat seat2;
			if(ggz_write_int(fd, seat2.type) < 0)
			{
				ZONEERROR("couldn't send seat assignments\n");
				return -1;
			}
			ZONEDEBUG("RAW OUTPUT: %i (ggz_seats[i].assign)\n", seat2.type);

			if(seat2.type == GGZ_SEAT_OPEN) m_ready = 0;
			else
			{
				if(ggz_write_string(fd, seat2.name) < 0)
				{
					ZONEERROR("couldn't send player names\n");
					return -1;
				}
			}
		}
	}
	return 0;
}

// Broadcast the game rules
void ZoneGGZModServer::game_send_rules()
{
	int i, fd;

	ZONEDEBUG("-- sending rules now ;)\n");

	for(i = 0; i < m_numplayers; i++)
	{
		GGZSeat seat = ggzdmod_get_seat(ggzdmod, i);
		fd = seat.fd;
		if(fd == -1)
		{
			ZONEDEBUG("Skipping %i\n", i);
			continue;
		}
		if(ggz_write_int(fd, ZoneGGZ::rules) < 0)
		{
			ZONEERROR("couldn't send ZoneGGZ::rules\n");
			return;
		}
		if((ggz_write_int(fd, m_gamemode) < 0) || (ggz_write_int(fd, m_maxplayers) < 0))
		{
			ZONEERROR("couldn't send gamemode, maxplayers\n");
			return;
		}
	}

	return;
}

// Register a game with name and player count
void ZoneGGZModServer::ZoneRegister(char* gamename, int gamemode, int maxplayers)
{
	ZONEDEBUG("registering...\n");
	if(m_gamename)
	{
		ZONEERROR("cannot register twice!\n");
		return;
	}
	m_gamename = strdup(gamename);
	m_gamemode = gamemode;
	m_maxplayers = maxplayers;
}

// Request next turn
void ZoneGGZModServer::zoneNextTurn()
{
	int fd;
	int counter;
	GGZSeat seat;

	if((m_gamemode == ZoneGGZ::turnbased) && (m_turn != -1))
	{
		seat = ggzdmod_get_seat(ggzdmod, m_turn);
		if(seat.type == GGZ_SEAT_PLAYER)
		{
			fd = seat.fd;
			if(ggz_write_int(fd, ZoneGGZ::turnover) < 0)
			{
				ZONEERROR("Couldn't send ZoneGGZ::turnover\n");
				return;
			}
		}
	}

	counter = 0;
	do
	{
		m_turn++;
		if(m_turn == m_numplayers) m_turn = 0;
		seat = ggzdmod_get_seat(ggzdmod, m_turn);
		ZONEDEBUG("Next player: %i (%i)\n", m_turn, seat.type);
		if(seat.type == GGZ_SEAT_BOT)
		{
			ZONEDEBUG("== AI!\n");
			slotZoneAI();
		}
		counter++;
		if(counter == m_numplayers + 1)
		{
			ZONEERROR("Loop detected!!!\n");
			return;
		}
	}
	while(seat.type != GGZ_SEAT_PLAYER);

	if((m_gamemode == ZoneGGZ::turnbased) && (seat.type == GGZ_SEAT_PLAYER))
	{
		fd = seat.fd;
		if(ggz_write_int(fd, ZoneGGZ::turn) < 0)
		{
			ZONEERROR("Couldn't send ZoneGGZ::turn\n");
			return;
		}
	}
}

// Return the current turn
int ZoneGGZModServer::zoneTurn()
{
	return m_turn;
}

// Callback for events
void ZoneGGZModServer::hook_event(GGZdMod *ggzdmod, GGZdModEvent event, void* data)
{
	switch(event)
	{
		case GGZDMOD_EVENT_STATE:
			if(*(GGZdModState*)data == GGZDMOD_STATE_CREATED)
				self->game_update(ZoneGGZ::launch, data);
			break;
		case GGZDMOD_EVENT_JOIN:
			self->game_update(ZoneGGZ::join, data);
			break;
		case GGZDMOD_EVENT_LEAVE:
			self->game_update(ZoneGGZ::leave, data);
			break;
	}
}

// Callback for game data
void ZoneGGZModServer::hook_data(GGZdMod *ggzdmod, GGZdModEvent event, void* data)
{
	//self->game_input();
}

