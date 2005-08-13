// Game server handler class for ggzdmod++ library
// Copyright (C) 2003 - 2005 Josef Spillner <josef@ggzgamingzone.org>
//
// This library has its origin in ggzd/games_servers/muehle/ggz!
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

#ifndef GGZDMODPP_GAMESERVER_H
#define GGZDMODPP_GAMESERVER_H

// GGZdMod includes
#include <ggzdmod.h>

/* Class representing a game server module.
 * It is used by reimplementing the virtual methods, and then calling
 * connect().
 */
class GGZGameServer
{
	public:
		/* Constructor */
		GGZGameServer();
		/* Destructor */
		virtual ~GGZGameServer();
		/* Connect to the GGZ server:
		 * async=false: object goes into a loop
		 * async=true: idleEvent gets called regularly
		 */
		void connect(bool async);

	protected:
		/* Callback for idle events (only in async mode) */
		virtual void idleEvent();
		/* Callback for state change */
		virtual void stateEvent();
		/* A player has joined the given seat */
		virtual void joinEvent(int player);
		/* A player has left the given seat */
		virtual void leaveEvent(int player);
		/* The player on the given seat has sent some data */
		virtual void dataEvent(int player);
		/* A spectator has joined the given spectator seat */
		virtual void spectatorJoinEvent(int spectator);
		/* A spectator has left the given spectator seat */
		virtual void spectatorLeaveEvent(int spectator);
		/* The spectator on the given spectator seat has sent some data */
		virtual void spectatorDataEvent(int spectator);
		/* An error occurred */
		virtual void errorEvent();

		/* Returns file descriptor for the given seat number */
		int fd(int player);
		/* Returns number of available seats, independent of their status */
		int players();
		/* Returns file descriptor for the given spectator seat number */
		int spectatorfd(int spectator);
		/* Returns number of available spectators */
		int spectators();
		/* Returns the number of open seats */
		int open();

	private:
		static void handle_state(GGZdMod* ggzdmod, GGZdModEvent event, const void *data);
		static void handle_join(GGZdMod* ggzdmod, GGZdModEvent event, const void *data);
		static void handle_leave(GGZdMod* ggzdmod, GGZdModEvent event, const void *data);
		static void handle_data(GGZdMod* ggzdmod, GGZdModEvent event, const void *data);
		static void handle_error(GGZdMod* ggzdmod, GGZdModEvent event, const void *data);
		static void handle_spectator_join(GGZdMod* ggzdmod, GGZdModEvent event, const void *data);
		static void handle_spectator_leave(GGZdMod* ggzdmod, GGZdModEvent event, const void *data);
		static void handle_spectator_data(GGZdMod* ggzdmod, GGZdModEvent event, const void *data);
		int m_connected;
};

#endif

