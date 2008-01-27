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

// GGZ includes
#include <ggz.h>

// System includes
#include <iostream>
#include <cstring>

// Constructor: inherit from ggzgameserver
MuehleServer::MuehleServer ()
: GGZGameServer () {
	m_net = new MuehleNet ();
	m_web = MuehleLoader::loadVariant ( "classic" );
	m_players = 0;
	m_valid[0] = 1;
	m_valid[1] = 1;
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

	if ( m_players == 2 ) {
		m_net->write ( fd ( 0 ), "white.\n");
		m_net->write ( fd ( 1 ), "black.\n");
	}
}

// Player leave event
void MuehleServer::leaveEvent ( int player ) {
	std::cout << "Muehle: leaveEvent" << std::endl;

	if ( m_players == 2 ) {
		m_net->write ( fd ( !player ), "loose.\n" );
		m_players--;
	}
	if ( m_players == 1 ) {
		delete this;
	}
}

// Spectator join event
void MuehleServer::spectatorJoinEvent ( int spectator ) {
	std::cout << "Muehle: spectatorJoinEvent" << std::endl;

	m_net->write ( spectatorfd ( spectator ), "spectator.\n");
}

// Spectator leave event
void MuehleServer::spectatorLeaveEvent ( int spectator ) {
	std::cout << "Muehle: spectatorLeaveEvent" << std::endl;
}

// Spectator data event (ignored)
void MuehleServer::spectatorDataEvent ( int spectator ) {
}

// Game data event
void MuehleServer::dataEvent ( int player ) {
	char data[1024];
	int ret;

	std::cout << "Muehle: dataEvent" << std::endl;

	ret = ggz_read_string( fd ( player ), data, 1024 );
	if ( ret )
		delete this;

	if ( data[ strlen ( data ) - 1 ] == '\n')
		data[ strlen ( data ) - 1 ] = 0;

	std::cout << "DEBUG: " << data << std::endl;

	if ( ( m_players == 2 ) && ( !m_valid[ 0 ] ) && ( !m_valid[ 1 ] ) )
		m_net->write ( fd ( !player ), data);
	else if ( !m_valid[ player ] )
		m_net->write ( fd (player), "invalid.\n");

	for( int i = 0; i < spectators (); i++ )
		m_net->write ( spectatorfd ( i ), data );

	m_valid[ player ] = 0;
}

// Error handling event
void MuehleServer::errorEvent () {
	std::cout << "Muehle: errorEvent" << std::endl;
}

