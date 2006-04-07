// Game server handler class for ggzdmod++ library
// Copyright (C) 2003 - 2006 Josef Spillner <josef@ggzgamingzone.org>
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

#include <string>

/* Forward declarations of internal classes */
class GGZGameServerPrivate;

/* Class representing a player, who can be playing or spectating */
class Client
{
	public:
		std::string name;
		int fd;
		bool spectator;
		int number;
};

/* Class representing a spectator seat.
 * If player is NULL, the spectator seat is empty.
 */
class Spectator
{
	public:
		int number;
		Client *client;
};

/* Class representing a seat.
 * Seats can be empty or filled with an active player or bot...
 */
class Seat : public Spectator
{
	public:
		/* Seat types */
		enum SeatType
		{
			none,
			open,
			bot,
			player,
			reserved,
			abandoned
		};

		SeatType type;
};

/* Class representing a game server module.
 * It is used by reimplementing the virtual methods, and then calling
 * connect().
 */
class GGZGameServer
{
	friend class GGZGameServerPrivate;

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
		/* Possible game states */
		enum State
		{
			created,
			waiting,
			playing,
			done
		};

		/* Callback for idle events (only in async mode) */
		virtual void idleEvent();
		/* Callback for state change */
		virtual void stateEvent();
		/* An error occurred */
		virtual void errorEvent();

		/* A player or spectator has joined the game */
		virtual void joinEvent(Client *client);
		/* A player or spectator has left game */
		virtual void leaveEvent(Client *client);
		/* The player or spectator has sent some data */
		virtual void dataEvent(Client *client);

		/* A seat change happened */
		virtual void seatEvent(Seat *seat);
		/* A spectator seat change happened */
		virtual void spectatorEvent(Spectator *spectator);

		/* Current game state */
		State state();
		/* Announce a state transition */
		void changeState(State state);

		/* Returns number of available seats, independent of their status */
		int players();
		/* Returns number of available spectator seats */
		int spectators();
		/* Returns the number of seats of one type (convenience method) */
		int playercount(Seat::SeatType type);
		/* Returns the number of active spectators (convenience method) */
		int spectatorcount();

		/* Returns seat object for the given seat number */
		Seat *seat(int number) const;
		/* Returns spectator seat object for the given spectator seat number */
		Spectator *spectator(int number) const;

		/* More convenience methods */
		int fd(int number)
		{
			Seat *s = seat(number);
			if(!s) return -1;
			if(!s->client) return -1;
			return s->client->fd;
		}
		int spectatorfd(int number)
		{
			Spectator *s = spectator(number);
			if(!s) return -1;
			if(!s->client) return -1;
			return s->client->fd;
		}
		int open(){return playercount(Seat::open);}

	private:
		GGZGameServerPrivate *m_private;
		int m_connected;
};

#endif

