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
#include "keepalive.h"

// Keepalive includes
#include "world.h"

// GGZdMod includes
#include "ggzdmod.h"

// System includes
#include <iostream>
#include <unistd.h>

// Prototypes
static void hook_events(GGZdMod *ggzdmod, GGZdModEvent event, const void *data);
static void hook_data(GGZdMod *ggzdmod, GGZdModEvent event, const void *data);

// Global self object
Keepalive *me;

// Global GGZdMod object
GGZdMod *ggzdmod;

// Constructor
Keepalive::Keepalive()
{
	me = this;

	m_valid = 0;
	m_world = NULL;

	ggzdmod = ggzdmod_new(GGZDMOD_GAME);
	ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_STATE, hook_events);
	ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_JOIN, hook_events);
	ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_LEAVE, hook_events);
	ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_PLAYER_DATA, hook_data);
	ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_ERROR, hook_events);
#ifdef GGZSPECTATORS
	ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_SPECTATOR_JOIN, hook_events);
	ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_SPECTATOR_LEAVE, hook_events);
	ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_SPECTATOR_DATA, hook_data);
#endif
}

// Destructor
Keepalive::~Keepalive()
{
	ggzdmod_free(ggzdmod);

	delete m_world;
}

// Enter main loop
void Keepalive::loop()
{
	int ret;

	ret = ggzdmod_connect(ggzdmod);
	if(ret < 0) return;

	m_world = new World();
	m_valid = 1;

	ggzdmod_loop(ggzdmod);
	ggzdmod_disconnect(ggzdmod);
}

// Handler for game data
void Keepalive::hookData(const void *data)
{
	const char *name;
	const int *num_ptr = (const int *)data;
	int num = *num_ptr;

	name = ggzdmod_get_seat(ggzdmod, num).name;
	m_world->receive(name, &num);
}

// Handler for joining players
void Keepalive::hookJoin(const void *data)
{
	GGZSeat seat = ggzdmod_get_seat(ggzdmod, *(int*)data);
	m_world->addPlayer(seat.name, seat.fd);
}

// Handler for leaving players
void Keepalive::hookLeave(const void *data)
{
	GGZSeat seat = *(GGZSeat*)data;
	m_world->removePlayer(seat.name);
}

#ifdef GGZSPECTATORS
// Handler for joining spectators
void Keepalive::hookSpectatorJoin(const void *data)
{
	GGZSpectator spectator = ggzdmod_get_spectator(ggzdmod, *(int*)data);
	m_world->addSpectator(spectator.name, spectator.fd);
}

// Handler for leaving spectators
void Keepalive::hookSpectatorLeave(const void *data)
{
	GGZSpectator spectator = *(GGZSpectator*)data;
	m_world->removeSpectator(spectator.name);
}

// Handler for spectator data
void Keepalive::hookSpectatorData(const void *data)
{
	const char *name;
	const int *num_ptr = (const int *)data;
	int num = *num_ptr;

	name = ggzdmod_get_spectator(ggzdmod, num).name;
	m_world->receive(name, &num);
}
#endif

// Handler for state changes
void Keepalive::hookState(const void *data)
{
}

// Handler for errors
void Keepalive::hookError(const void *data)
{
	close(ggzdmod_get_fd(ggzdmod));
	m_valid = 0;
	std::cerr << "An error occured: " << (char*)data << std::endl;
}

// Callback for events
void hook_events(GGZdMod *ggzdmod, GGZdModEvent event, const void *data)
{
	int player;

	if(!me->m_valid) return;

	switch(event)
	{
		case GGZDMOD_EVENT_STATE:
			me->hookState(data);
			break;
		case GGZDMOD_EVENT_JOIN:
//std::cout << "Join event gives us: " << ((GGZSeat*)data)->name << std::endl;
			player = ((GGZSeat*)data)->num;
//std::cout << "Join event number: " << player << std::endl;
			me->hookJoin(&player);
			break;
		case GGZDMOD_EVENT_LEAVE:
//std::cout << "Leave event gives us: " << ((GGZSeat*)data)->name << std::endl;
			//player = ((GGZSeat*)data)->num;
//std::cout << "Leave event number: " << player << std::endl;
			//me->hookLeave(&player);
			me->hookLeave(data);
			break;
		case GGZDMOD_EVENT_ERROR:
			me->hookError(data);
			break;
#ifdef GGZSPECTATORS
		case GGZDMOD_EVENT_SPECTATOR_JOIN:
			player = ((GGZSpectator*)data)->num;
			me->hookSpectatorJoin(&player);
			break;
		case GGZDMOD_EVENT_SPECTATOR_LEAVE:
			me->hookSpectatorLeave(data);
			break;
#endif
		default:
			// FIXME: error
			break;
	}
}

// Callback for game data
void hook_data(GGZdMod *ggzdmod, GGZdModEvent event, const void *data)
{
	if(!me->m_valid) return;

	switch(event)
	{
		case GGZDMOD_EVENT_PLAYER_DATA:
			me->hookData(data);
			break;
		case GGZDMOD_EVENT_SPECTATOR_DATA:
			me->hookSpectatorData(data);
			break;
		default:
			// FIXME: error
			break;
	}
}

