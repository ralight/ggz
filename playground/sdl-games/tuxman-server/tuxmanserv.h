// Tuxman Server
// Copyright (C) 2003 Josef Spillner <josef@ggzgamingzone.org>
// Tuxman Copyright (C) 2003 Robert Strobl <badwolf@acoderz.de>
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

#ifndef TUXMAN_SERVER_H
#define TUXMAN_SERVER_H

#define TUXMANDATA "/usr/local/share/ggzd/tuxmanserv"

// GGZ includes
#include "ggzgameserver.h"

// Forward declaration
class Net;
class Pacman;

// Muehle server object
class TuxmanServer : public GGZGameServer
{
	public:
		TuxmanServer();
		~TuxmanServer();
		void joinEvent(int player);
		void leaveEvent( int player);
		void dataEvent(int player);
		void errorEvent();

	private:
		Net *net;
		Pacman *pac;
		bool join_lock;
};

#endif

