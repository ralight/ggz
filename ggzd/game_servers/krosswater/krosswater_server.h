// Krosswater - Server for the Krosswater game
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

class CWPathitem;

struct player_t
{
	int x, y;
};

class KrosswaterServer : public ZoneGGZModServer
{
	public:
		KrosswaterServer();
		~KrosswaterServer();
		int slotZoneInput(int fd, int i);
		void slotZoneAI();

		enum KrosswaterProto
		{
			proto_helloworld = 101,
			proto_move = 102,
			proto_map = 103,
			proto_map_respond = 104,
			proto_map_backtrace = 105,
			proto_move_broadcast = 106,
			proto_restart = 107
		};

	private:
		void getMove();
		void sendMap();
		void sendRestart();
		int doMove(int fromx, int fromy, int tox, int toy);
		void eraseMap();

		int m_fd;
		int map_x, map_y;
		int **map;
		CWPathitem *path;
		player_t *players;
};

