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

#ifdef HAVE_CONFIG_H
#  include <config.h>			/* Site-specific config */
#endif

// Header file
#include "keepalive.h"

// Keepalive includes
#include "world.h"

// GGZdMod includes
#include <ggzdmod.h>

// Prototypes
static void hook_events(GGZdMod *ggzdmod, GGZdModEvent event, void *data);
static void hook_data(GGZdMod *ggzdmod, GGZdModEvent event, void *data);

// Global self object
Keepalive *me;

// Global GGZdMod object
GGZdMod *ggzdmod;

// Constructor
Keepalive::Keepalive()
{
	me = this;

	m_world = new World();

	ggzdmod = ggzdmod_new(GGZDMOD_GAME);
	ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_STATE, hook_events);
	ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_JOIN, hook_events);
	ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_LEAVE, hook_events);
	ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_LOG, hook_events);
	ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_PLAYER_DATA, hook_data);
	ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_ERROR, hook_events);
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
	ggzdmod_loop(ggzdmod);
	ggzdmod_disconnect(ggzdmod);
}

// Handler for game data
void Keepalive::hookData(void *data)
{
	char *name;

	name = ggzdmod_get_seat(ggzdmod, *(int*)data).name;
	m_world->receive(name, data);
}

// Handler for joining players
void Keepalive::hookJoin(void *data)
{
	GGZSeat seat = ggzdmod_get_seat(ggzdmod, *(int*)data);
	m_world->addPlayer(seat.name, seat.fd);
}

// Handler for leaving players
void Keepalive::hookLeave(void *data)
{
	GGZSeat seat = ggzdmod_get_seat(ggzdmod, *(int*)data);
	m_world->removePlayer(seat.name);
}

// HAndler for state changes
void Keepalive::hookState(void *data)
{
}

// HAndler for errors
void Keepalive::hookError(void *data)
{
}

// Handler for logging events
void Keepalive::hookLog(void *data)
{
}

// Callback for events
void hook_events(GGZdMod *ggzdmod, GGZdModEvent event, void *data)
{
	int player;
	switch(event)
	{
		case GGZDMOD_EVENT_STATE:
			me->hookState(data);
			break;
		case GGZDMOD_EVENT_JOIN:
			player = ((GGZSeat*)data)->num;
			me->hookJoin(&player);
			break;
		case GGZDMOD_EVENT_LEAVE:
			player = ((GGZSeat*)data)->num;
			me->hookLeave(&player);
			break;
		case GGZDMOD_EVENT_LOG:
			me->hookLog(data);
			break;
		case GGZDMOD_EVENT_ERROR:
			me->hookError(data);
			break;
	}
}

// Callback for game data
void hook_data(GGZdMod *ggzdmod, GGZdModEvent event, void *data)
{
	me->hookData(data);
}

