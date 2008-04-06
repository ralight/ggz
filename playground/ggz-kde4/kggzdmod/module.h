/*
    This file is part of the kggzdmod library.
    Copyright (c) 2007 Josef Spillner <josef@ggzgamingzone.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef KGGZDMOD_MODULE_H
#define KGGZDMOD_MODULE_H

#include <kggzdmod/request.h>
//#include <kggzdmod/event.h>

#include <QObject>
#include <QList>

namespace KGGZdMod
{

class ModulePrivate;
class Player;
class Event;

/**
 * @short GGZ Gaming Zone game server to main server interface.
 *
 * Any game server intended to run on GGZ should instantiate exactly
 * one object of this class and connect its signals to the game
 * protocol handler.
 * Messages from the game clients are reported through \ref signalEvent,
 * as are other kinds of events.
 * Network input can either lead to a networking routine within the game
 * or to an instance of a ggzcommgen-generated protocol handler class,
 * which itself provides signals for the game.
 *
 * In addition to the signal, the current game state can be
 * queried, as can the list of players and game spectators
 * as \ref PlayerSeat and \ref SpectatorSeat objects.
 *
 * Finally, the object of this class is used to send
 * request to GGZ, for changing the state, retrieving player
 * information and similar actions. The \ref Request class
 * has more details.
 *
 * @author Josef Spillner (josef@ggzgamingzone.org)
 */
class Module : public QObject
{
	Q_OBJECT
	public:
		/**
		 * Initialisation of online gaming through GGZ.
		 *
		 * Before online gaming is activated, the validity of
		 * the GGZ environment should be checked with the static
		 * \ref isGGZ method.
		 *
		 * @param name Name of the game server
		 */
		Module(QString name);
		~Module();

		/**
		 * The state a GGZ game can be in. These states are controlled
		 * by the GGZ server (for \ref created and
		 * \ref waiting) and afterwards by the game server which toggles
		 * between \ref waiting and \ref playing until finally reaching
		 * \ref done.
		 */
		enum State
		{
			created,	/**< The initial state. */
			waiting,	/**< The connection to the GGZ server has been established. */
			playing,	/**< The game server is now playing. */
			done,		/**< The game is over. */
			restored	/**< A savegame is being restored. */
		};

		/**
		 * Sends a request to the GGZ server.
		 *
		 * The request is sent to the GGZ server.
		 * In most cases, an event will be delivered back to the game server.
		 *
		 * @param request The request to the GGZ server
		 */
		void sendRequest(Request request);

		/**
		 * Returns the list of seats on the table.
		 *
		 * This includes all active players, bots, open seats
		 * and abandoned/reserved seats.
		 */
		QList<Player*> players() const;

		/**
		 * Returns the list of game spectators.
		 */
		QList<Player*> spectators() const;

		/**
		 * Returns the current state the game is in.
		 */
		State state() const;

		/**
		 * Checks if the game is started in a GGZ environment.
		 *
		 * Calling \ref Module should only be done in case a
		 * GGZ environment has been detected.
		 *
		 * @return \b true if the game runs on GGZ, \b false otherwise
		 */
		static bool isGGZ();

		/**
		 * Returns the single instance of this class. If no instance
		 * exists yet, \b null is returned. However, if multiple
		 * instances exist, this method might return one of the
		 * existing instances, but also \b null, i.e. the behaviour
		 * is undefined.
		 * In most scenarios, there will be exactly one instance.
		 *
		 * @return Module instance, or \b null if not applicable
		 */
		static Module *instance();

	signals:
		/**
		 * An event from the GGZ server has happened.
		 *
		 * @param event The event from the GGZ server
		 */
 		void signalEvent(const KGGZdMod::Event& event);

	private:
		ModulePrivate *d;
};

}

#endif

