// Muehle - KDE Muehle (Nine Men's Morris) game for GGZ
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

// Header file
#include "ggzgameserver.h"

// System includes
#include <iostream>

// Global singleton object
GGZGameServer* self;

// Globla ggzdmod object
GGZdMod* ggzdmod;

// Constructor: setup the server
GGZGameServer::GGZGameServer () {
	self = this;
	ggzdmod = ggzdmod_new ( GGZDMOD_GAME );
	ggzdmod_set_handler ( ggzdmod, GGZDMOD_EVENT_STATE, &handle_state );
	ggzdmod_set_handler ( ggzdmod, GGZDMOD_EVENT_JOIN,  &handle_join  );
	ggzdmod_set_handler ( ggzdmod, GGZDMOD_EVENT_LEAVE, &handle_leave );
	ggzdmod_set_handler ( ggzdmod, GGZDMOD_EVENT_PLAYER_DATA, &handle_data );
	ggzdmod_set_handler ( ggzdmod, GGZDMOD_EVENT_ERROR, &handle_error );
	m_connected = 0;
}

// Destructor: cleanup server
GGZGameServer::~GGZGameServer () {
	if ( m_connected ) {
		(void) ggzdmod_disconnect ( ggzdmod );
	}
	ggzdmod_free ( ggzdmod );
}

// Connect to the GGZ main server
void GGZGameServer::connect () {
	int ret = ggzdmod_connect ( ggzdmod );
	if ( ret < 0 ) {
		std::cout << "GGZGameServer: Error: Couldn't connect" << std::endl;
		return;
	}
	m_connected = 1;
	(void) ggzdmod_loop ( ggzdmod );
}

// Virtual state change hook
void GGZGameServer::stateEvent () {
}

// Virtual player join hook
void GGZGameServer::joinEvent ( int player ) {
}

// Virtual player leave hook
void GGZGameServer::leaveEvent ( int player ) {
}

// Virtual game data hook
void GGZGameServer::dataEvent ( int player ) {
}

// Virtual error handling hook
void GGZGameServer::errorEvent () {
}

// Callback for the state hook
void GGZGameServer::handle_state ( GGZdMod* ggzdmod, GGZdModEvent event, void* data ) {
	std::cout << "GGZGameServer: stateEvent" << std::endl;
	self->stateEvent ();
}

// Callback for the player join hook
void GGZGameServer::handle_join ( GGZdMod* ggzdmod, GGZdModEvent event, void* data ) {
	int player = ((GGZSeat*)data)->num;
	std::cout << "GGZGameServer: joinEvent" << std::endl;
	self->joinEvent ( player );
}

// Callback for the player leave hook
void GGZGameServer::handle_leave ( GGZdMod* ggzdmod, GGZdModEvent event, void* data ) {
	int player = ((GGZSeat*)data)->num;
	std::cout << "GGZGameServer: leaveEvent" << std::endl;
	self->leaveEvent ( player );
}

// Callback for the game data hook
void GGZGameServer::handle_data ( GGZdMod* ggzdmod, GGZdModEvent event, void* data ) {
	std::cout << "GGZGameServer: dataEvent" << std::endl;
	self->dataEvent ( *(int* ) data);
}

// Callback for the error handling hook
void GGZGameServer::handle_error ( GGZdMod* ggzdmod, GGZdModEvent event, void* data ) {
	std::cout << "GGZGameServer: errorEvent" << std::endl;
	self->errorEvent ();
}

int GGZGameServer::fd ( int player ) {
	return ggzdmod_get_seat ( ggzdmod, player ).fd;
}

