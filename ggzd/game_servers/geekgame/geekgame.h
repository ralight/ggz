// Geekgame - a game which only real geeks understand
// Copyright (C) 2002, 2003 Josef Spillner, josef@ggzgamingzone.org
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

#ifndef GEEKGAME_SERVER_H
#define GEEKGAME_SERVER_H

// GGZ includes
#include "ggzgameserver.h"

// Definitions
#define ARRAY_WIDTH 20
#define ARRAY_HEIGHT 10

// Geekgame server object
class Geekgame : public GGZGameServer
{
	public:
		Geekgame();
		~Geekgame();
		void stateEvent();
		void joinEvent(int player);
		void leaveEvent(int player);
		void spectatorJoinEvent(int spectator);
		void spectatorLeaveEvent(int spectator);
		void spectatorDataEvent(int spectator);
		void dataEvent(int player);
		void errorEvent();

	private:
		void game_start();
		void game_stop();
		void game_end();

		int map_check(int x, int y);

		int m_players;
		int m_valid[2];
		int m_array[ARRAY_WIDTH][ARRAY_HEIGHT];
		int m_mode;
		char *ruleset;
};

#endif

