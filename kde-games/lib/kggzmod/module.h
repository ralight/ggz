/*
    This file is part of the kggzmod library.
    Copyright (c) 2005, 2006 Josef Spillner <josef@ggzgamingzone.org>

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

#ifndef KGGZMOD_MODULE_H
#define KGGZMOD_MODULE_H

#include <kggzmod/request.h>
#include <kggzmod/event.h>

#include <qobject.h>

namespace KGGZMod
{

class ModulePrivate;
class Player;

/**
 * @short GGZ Gaming Zone game client to core client interface.
 *
 * Any game client intended to run on GGZ should instanciate exactly
 * one object of this class and connect its signals to the game
 * protocol handler. GGZ core client events are delivered by
 * \ref signalEvent but can be ignored. If needed, the \ref Event
 * class provides more information.
 * Messages from the game
 * server are emitted by \ref signalNetwork. This signal can either
 * lead to a networking routine within the game or to an instance of a
 * ggzcommgen-generated protocol handler class, which itself provides
 * signals for the game. The third signal, \ref signalError, should
 * terminate the network gaming with an appropriate error message.
 * When \ref signalError is emitted, the internal GGZ network
 * connections have already been disconnected.
 *
 * In addition to the signals, the current game state can be
 * queried, as can the list of players and game spectators
 * as \ref Player objects.
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
		Module(QString name);
		~Module();

		enum State
		{
			created,
			connected,
			waiting,
			playing,
			done
		};

		void sendRequest(Request request);

		QValueList<Player*> players() const;
		QValueList<Player*> spectators() const;
		State state() const;

		static bool isGGZ();

		Player *self() const;

	signals:
		void signalEvent(KGGZMod::Event event);
		void signalError();
		void signalNetwork(int fd);

	private:
		ModulePrivate *d;
};

}

#endif

