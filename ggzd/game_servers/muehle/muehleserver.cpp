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
#include "muehleserver.h"

// Muehle includes
#include "loader.h"
#include "net.h"

// System includes
#include <iostream>

// Constructor: inherit from ggzgameserver
MuehleServer::MuehleServer ()
: GGZGameServer () {
	m_net = new MuehleNet ();
	m_web = MuehleLoader::loadVariant ( "twelvemensmorris" );
	m_players = 0;
}

// Destructor
MuehleServer::~MuehleServer () {
	if ( m_web )
		delete m_web;
	if ( m_net )
		delete m_net;
}

// State change hook
void MuehleServer::stateEvent () {
	std::cout << "Muehle: stateEvent" << std::endl;
}

// Player join hook
void MuehleServer::joinEvent ( int player ) {
	std::cout << "Muehle: joinEvent" << std::endl;

	m_players++;

	if ( m_players == 2) {
		m_net->write ( fd ( 0 ), "white.\n");
		m_net->write ( fd ( 1 ), "black.\n");
	}
}

// Player leave event
void MuehleServer::leaveEvent ( int player ) {
	std::cout << "Muehle: leaveEvent" << std::endl;
}

// Game data event
void MuehleServer::dataEvent ( int player, void* data ) {
	std::cout << "Muehle: dataEvent" << std::endl;

	m_net->write ( fd ( !player ), ( const char* ) data);
}

// Error handling event
void MuehleServer::errorEvent () {
	std::cout << "Muehle: errorEvent" << std::endl;
}

