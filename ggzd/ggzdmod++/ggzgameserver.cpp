// Game server handler class for ggzdmod++ library
// Copyright (C) 2003 - 2006 Josef Spillner <josef@ggzgamingzone.org>
//
// This library has its origin in ggzd/games_servers/muehle/ggz!
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
#include "ggzgameserver.h"

// GGZ includes
#include <ggzdmod.h>

// System includes
#include <iostream>
#include <map>

// Internal class for callback handling
class GGZGameServerPrivate
{
		public:
			GGZGameServerPrivate(GGZGameServer *parent);
			~GGZGameServerPrivate();
			inline GGZdMod *ggzdmod() const {return m_ggzdmod;}
			static bool exists() {return (GGZGameServerPrivate::m_parent != NULL);}
			std::map<int,Seat*> seatmap;
			std::map<int,Spectator*> spectatormap;
			std::map<int,Client*> clientmap;
		private:
			static void handle_state(GGZdMod* ggzdmod,
				GGZdModEvent event, const void *data);
			static void handle_error(GGZdMod* ggzdmod,
				GGZdModEvent event, const void *data);
			static void handle_join(GGZdMod* ggzdmod,
				GGZdModEvent event, const void *data);
			static void handle_leave(GGZdMod* ggzdmod,
				GGZdModEvent event, const void *data);
			static void handle_data(GGZdMod* ggzdmod,
				GGZdModEvent event, const void *data);
			static void handle_seat(GGZdMod* ggzdmod,
				GGZdModEvent event, const void *data);
			static void handle_spectator_join(GGZdMod* ggzdmod,
				GGZdModEvent event, const void *data);
			static void handle_spectator_leave(GGZdMod* ggzdmod,
				GGZdModEvent event, const void *data);
			static void handle_spectator_data(GGZdMod* ggzdmod,
				GGZdModEvent event, const void *data);
			static void handle_spectator_seat(GGZdMod* ggzdmod,
				GGZdModEvent event, const void *data);
			void selfcheck();
			// Parent object (which is a singleton in 'self')
			static GGZGameServer *m_parent;
			// Global ggzdmod object
			GGZdMod *m_ggzdmod;
};

GGZGameServer *GGZGameServerPrivate::m_parent;

// Constructor: setup the server
GGZGameServer::GGZGameServer()
{
	if(GGZGameServerPrivate::exists())
	{
		std::cerr << "GGZGameServer: Error: double initialization" << std::endl;
	}

	m_private = new GGZGameServerPrivate(this);
	m_connected = 0;
}

// Destructor: cleanup server
GGZGameServer::~GGZGameServer()
{
	if(m_connected)
	{
		(void)ggzdmod_disconnect(m_private->ggzdmod());
	}
	delete m_private;
}

// Connect to the GGZ main server
void GGZGameServer::connect(bool async)
{
	int ret = ggzdmod_connect(m_private->ggzdmod());
	if(ret < 0)
	{
		std::cout << "GGZGameServer: Error: Couldn't connect" << std::endl;
		return;
	}
	m_connected = 1;
	if(async)
	{
		while(ggzdmod_dispatch(m_private->ggzdmod()) != -1)
		{
			idleEvent();
		}
	}
	else
	{
		(void)ggzdmod_loop(m_private->ggzdmod());
	}
}

// Virtual state change hook
void GGZGameServer::stateEvent()
{
}

// Virtual player special hook: idle method
void GGZGameServer::idleEvent()
{
}

// Virtual error handling hook
void GGZGameServer::errorEvent()
{
}

// Virtual player/spectator join hook
void GGZGameServer::joinEvent(Client *client)
{
}

// Virtual player/spectator leave hook
void GGZGameServer::leaveEvent(Client *client)
{
}

// Virtual player/spectator game data hook
void GGZGameServer::dataEvent(Client *client)
{
}

// Virtual seat change data hook
void GGZGameServer::seatEvent(Seat *seat)
{
}

// Virtual spectator seat change data hook
void GGZGameServer::spectatorEvent(Spectator *spectator)
{
}

int GGZGameServer::players()
{
	return ggzdmod_get_num_seats(m_private->ggzdmod());
}

int GGZGameServer::playercount(Seat::SeatType type)
{
	return ggzdmod_count_seats(m_private->ggzdmod(), (GGZSeatType)type);
}

int GGZGameServer::spectators()
{
	return ggzdmod_count_spectators(m_private->ggzdmod());
}

int GGZGameServer::spectatorcount()
{
	return ggzdmod_count_spectators(m_private->ggzdmod());
}

Seat *GGZGameServer::seat(int number) const
{
	GGZSeat s = ggzdmod_get_seat(m_private->ggzdmod(), number);
	if(s.num == -1) return NULL;

	Seat *seat;
	seat = m_private->seatmap[s.num];
	if(!seat)
	{
		seat = new Seat();
		seat->number = s.num;
		seat->type = (Seat::SeatType)s.type;
		Client *client;
		client = m_private->clientmap[s.fd];
		if(!client)
		{
			client = new Client();
			client->fd = s.fd;
			client->name = s.name;
			client->number = s.num;
			client->spectator = false;
		}
		seat->client = client;
		m_private->seatmap[s.num] = seat;
	}

	return seat;
}

Spectator *GGZGameServer::spectator(int number) const
{
	GGZSpectator s = ggzdmod_get_spectator(m_private->ggzdmod(), number);
	if(s.num == -1) return NULL;

	Spectator *spectator;
	spectator = m_private->spectatormap[s.num];
	if(!spectator)
	{
		spectator = new Spectator();
		spectator->number = s.num;
		Client *client;
		client = m_private->clientmap[s.fd];
		if(!client)
		{
			client = new Client();
			client->fd = s.fd;
			client->name = s.name;
			client->number = s.num;
			client->spectator = true;
		}
		spectator->client = client;
		m_private->spectatormap[s.num] = spectator;
	}

	return spectator;
}

GGZGameServer::State GGZGameServer::state()
{
	return (State)ggzdmod_get_state(m_private->ggzdmod());
}

void GGZGameServer::changeState(State state)
{
	ggzdmod_set_state(m_private->ggzdmod(), (GGZdModState)state);
}

// Implementation of the internal class
GGZGameServerPrivate::GGZGameServerPrivate(GGZGameServer *parent)
{
	m_parent = parent;

	m_ggzdmod = ggzdmod_new(GGZDMOD_GAME);

	ggzdmod_set_handler(m_ggzdmod, GGZDMOD_EVENT_STATE, &handle_state);
	ggzdmod_set_handler(m_ggzdmod, GGZDMOD_EVENT_ERROR, &handle_error);

	ggzdmod_set_handler(m_ggzdmod, GGZDMOD_EVENT_JOIN, &handle_join);
	ggzdmod_set_handler(m_ggzdmod, GGZDMOD_EVENT_LEAVE, &handle_leave);
	ggzdmod_set_handler(m_ggzdmod, GGZDMOD_EVENT_PLAYER_DATA, &handle_data);
	ggzdmod_set_handler(m_ggzdmod, GGZDMOD_EVENT_SEAT, &handle_seat);

	ggzdmod_set_handler(m_ggzdmod, GGZDMOD_EVENT_SPECTATOR_JOIN, &handle_spectator_join);
	ggzdmod_set_handler(m_ggzdmod, GGZDMOD_EVENT_SPECTATOR_LEAVE, &handle_spectator_leave);
	ggzdmod_set_handler(m_ggzdmod, GGZDMOD_EVENT_SPECTATOR_DATA, &handle_spectator_data);
	ggzdmod_set_handler(m_ggzdmod, GGZDMOD_EVENT_SPECTATOR_SEAT, &handle_spectator_seat);

	selfcheck();
}

// Destructor of the internal class
GGZGameServerPrivate::~GGZGameServerPrivate()
{
	ggzdmod_free(m_ggzdmod);
}

// Prevent wrong ggzdmod libraries from being linked
void GGZGameServerPrivate::selfcheck()
{
	bool error = false;

	if((int)Seat::none != (int)GGZ_SEAT_NONE) error = true;
	if((int)Seat::open != (int)GGZ_SEAT_OPEN) error = true;
	if((int)Seat::bot != (int)GGZ_SEAT_BOT) error = true;
	if((int)Seat::player != (int)GGZ_SEAT_PLAYER) error = true;
	if((int)Seat::reserved != (int)GGZ_SEAT_RESERVED) error = true;
	if((int)Seat::abandoned != (int)GGZ_SEAT_ABANDONED) error = true;

	if((int)GGZGameServer::created != (int)GGZDMOD_STATE_CREATED) error = true;
	if((int)GGZGameServer::waiting != (int)GGZDMOD_STATE_WAITING) error = true;
	if((int)GGZGameServer::playing != (int)GGZDMOD_STATE_PLAYING) error = true;
	if((int)GGZGameServer::done != (int)GGZDMOD_STATE_DONE) error = true;

	if(error)
	{
		std::cerr << "GGZGameServer: Error: library mismatch" << std::endl;
	}
}

// Callback for the state hook
void GGZGameServerPrivate::handle_state(GGZdMod* ggzdmod, GGZdModEvent event, const void *data)
{
	std::cout << "GGZGameServer: state event" << std::endl;
	m_parent->stateEvent();
}

// Callback for the error handling hook
void GGZGameServerPrivate::handle_error(GGZdMod* ggzdmod, GGZdModEvent event, const void *data)
{
	std::cout << "GGZGameServer: error event" << std::endl;
	m_parent->errorEvent();
}

// Callback for the player join hook
void GGZGameServerPrivate::handle_join(GGZdMod* ggzdmod, GGZdModEvent event, const void *data)
{
	GGZSeat oldseat = *(GGZSeat*)data;
	std::cout << "GGZGameServer: join event" << std::endl;
	Seat *seat = m_parent->seat(oldseat.num);
	m_parent->joinEvent(seat->client);
}

// Callback for the player leave hook
void GGZGameServerPrivate::handle_leave(GGZdMod* ggzdmod, GGZdModEvent event, const void *data)
{
	GGZSeat oldseat = *(GGZSeat*)data;
	std::cout << "GGZGameServer: leave event" << std::endl;
	Seat *seat = m_parent->seat(oldseat.num);
	m_parent->leaveEvent(seat->client);
}

// Callback for the game data hook
void GGZGameServerPrivate::handle_data(GGZdMod* ggzdmod, GGZdModEvent event, const void *data)
{
	int num = *(int*)data;
	std::cout << "GGZGameServer: data event" << std::endl;
	Seat *seat = m_parent->seat(num);
	m_parent->dataEvent(seat->client);
}

// Callback for the game seat change hook
void GGZGameServerPrivate::handle_seat(GGZdMod* ggzdmod, GGZdModEvent event, const void *data)
{
	GGZSeat oldseat = *(GGZSeat*)data;
	std::cout << "GGZGameServer: seat event" << std::endl;
	Seat *seat = m_parent->seat(oldseat.num);
	m_parent->seatEvent(seat);
}

// Callback for the spectator join hook
void GGZGameServerPrivate::handle_spectator_join(GGZdMod* ggzdmod,GGZdModEvent event, const void *data)
{
	std::cout << "GGZGameServer: spectator join event" << std::endl;
	GGZSpectator oldspectator = *(GGZSpectator*)data;
	Spectator *spectator = m_parent->spectator(oldspectator.num);
	m_parent->joinEvent(spectator->client);
}

// Callback for the spectator leave hook
void GGZGameServerPrivate::handle_spectator_leave(GGZdMod* ggzdmod, GGZdModEvent event, const void *data)
{
	std::cout << "GGZGameServer: spectator leave event" << std::endl;
	GGZSpectator oldspectator = *(GGZSpectator*)data;
	Spectator *spectator = m_parent->spectator(oldspectator.num);
	m_parent->leaveEvent(spectator->client);
}

// Callback for the spectator data hook
void GGZGameServerPrivate::handle_spectator_data(GGZdMod* ggzdmod, GGZdModEvent event, const void *data)
{
	std::cout << "GGZGameServer: spectator data event" << std::endl;
	int num = *(int*)data;
	Spectator *spectator = m_parent->spectator(num);
	m_parent->dataEvent(spectator->client);
}

// Callback for the spectator seat change hook
void GGZGameServerPrivate::handle_spectator_seat(GGZdMod* ggzdmod, GGZdModEvent event, const void *data)
{
	GGZSpectator oldspectator = *(GGZSpectator*)data;
	std::cout << "GGZGameServer: spectator seat event" << std::endl;
	Spectator *spectator = m_parent->spectator(oldspectator.num);
	m_parent->spectatorEvent(spectator);
}

