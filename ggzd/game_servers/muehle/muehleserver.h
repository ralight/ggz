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

#ifndef MUEHLE_SERVER_H
#define MUEHLE_SERVER_H

// GGZ includes
#include "ggzgameserver.h"

// Forward declarations
class QWeb;
class MuehleNet;

// Muehle server object
class MuehleServer : public GGZGameServer {
	public:
		MuehleServer ();
		~MuehleServer ();
		void stateEvent ();
		void joinEvent ( int player );
		void leaveEvent ( int player );
		void dataEvent ( int player );
		void errorEvent ();
	private:
		QWeb* m_web;
		MuehleNet* m_net;
		int m_players;
		int m_valid[2];
};

#endif

