// ZoneServer - small C++ game support library
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

#ifndef ZONE_GGZ_MOD_SERVER_H
#define ZONE_GGZ_MOD_SERVER_H

// GGZdmod includes
#include <ggzdmod.h>

class ZoneGGZModServer
{
	public:
		ZoneGGZModServer();
		virtual ~ZoneGGZModServer();
		virtual int slotZoneInput(int fd, int i);
		virtual void slotZoneAI();
		int zoneMainLoop();
		void ZoneRegister(char* gamename, int gamemode, int maxplayers);
		void zoneNextTurn(bool advance);

		int zoneTurn();

		int m_maxplayers;
		int m_numplayers;

	protected:
		// Global ggzdmod object
		GGZdMod *ggzdmod;

	private:
		int game_send_seat(int seat);
		int game_send_players();
		void game_send_rules();
		int game_update(int event, const void* data);

		static void hook_event(GGZdMod *ggzdmod, GGZdModEvent event,
				       const void* data);
		static void hook_data(GGZdMod *ggzdmod, GGZdModEvent event,
				      const void* data);

		int m_gamemode;
		int* m_players;
		int m_turn;
		int m_state;
		char* m_gamename;
		int ggz_sock;
		int m_ready;
};

#endif

