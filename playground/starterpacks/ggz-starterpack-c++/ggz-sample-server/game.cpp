//
// GGZ Starterpack for C++ - Sample Server
// Copyright (C) 2008 GGZ Development Team
//
// This code is made available as public domain; you can use it as a base
// for your own game, as long as its licence is compatible with the libraries
// you use.

#include "game.h"

#include <kggzdmod/module.h>
#include <kggzdmod/player.h>

#include <kdebug.h>

#include <QCoreApplication>

Game::Game(QObject *parent)
: QObject(parent)
{
	/* Here we initialise the GGZ server module, which has a single signal. */

	m_module = new KGGZdMod::Module("Sample Server C++");

	connect(m_module,
		SIGNAL(signalEvent(const KGGZdMod::Event&)),
		SLOT(slotEvent(const KGGZdMod::Event&)));

	m_module->loop();
}

bool Game::allSeatsFull()
{
	/* Helper method: Check if all seats are full. */

	int open = 0;
	QList<KGGZdMod::Player*> players = m_module->players();
	for(int i = 0; i < players.size(); i++)
	{
		kDebug() << "* player at #" << i << "is" << players.at(i)->type();
		if(players.at(i)->type() == KGGZdMod::Player::open)
			open++;
	}

	return (open == 0);
}

KGGZdMod::Player *Game::findPlayerBySeat(int seatnum)
{
	/* Helper method: return the n-th seat. */

	QList<KGGZdMod::Player*> players = m_module->players();
	for(int i = 0; i < players.size(); i++)
	{
		if(players.at(i)->seat() == seatnum)
		{
			return players.at(i);
		}
	}
	return NULL;
}

void Game::slotEvent(const KGGZdMod::Event& event)
{
	/* Here, we handle events which come from the GGZ server, or more precisely
	from the game table within the server. Only a few events are shown here, there
	are many more available, e.g. for spectators or for resuming interrupted
	games. In the starterpack server, all we do is wait for joining players and
	then send them the HELLO message. */

	kDebug() << "GGZ event:" << event.type();

	if(event.type() == KGGZdMod::Event::playerseat)
	{
		KGGZdMod::PlayerSeatEvent pse(event);
		kDebug() << "Player seat change at #" << pse.seat().seat() << ": from"
			<< pse.oldseat().name() << "to" << pse.seat().name() <<
			"(seat change type " << pse.change() << ")";

		if(allSeatsFull())
		{
			kDebug() << "All seats full, start/continue game";

			KGGZdMod::Player *p = findPlayerBySeat(pse.seat().seat());
			ggz_starterpack proto;
			proto.ggzcomm_set_fd(p->fd());
			proto.ggzcomm_hello(hello());
		}
		else
		{
			kDebug() << "Not all seats full, delay/interrupt game";
		}
	}
	else if(event.type() == KGGZdMod::Event::playerdata)
	{
		KGGZdMod::PlayerDataEvent e(event);

		KGGZdMod::Player *p = findPlayerBySeat(e.seat().seat());
		handleInput(p);
	}
	else if(event.type() == KGGZdMod::Event::error)
	{
		KGGZdMod::ErrorEvent e(event);
		kError() << e.message();
		shutdown();
	}
}

void Game::handleInput(KGGZdMod::Player *p)
{
	/* Here we handle any input from the client. We don't know yet what the input
	is at that point, therefore we create a message parser from the auto-generated
	class ggz_starterpack. The protocol is defined in the definition file
	../common/ggz_starterpack.protocol. The protocol can generate notifications
	for each valid message, and errors for invalid messages. */

	kDebug() << "Input from player at #" << p->seat();

	ggz_starterpack proto;
	connect(&proto,
		SIGNAL(signalNotification(ggz_starterpackOpcodes::Opcode, const msg&)),
		SLOT(slotNotification(ggz_starterpackOpcodes::Opcode, const msg&)));
	connect(&proto, SIGNAL(signalError()), SLOT(slotError()));

	proto.ggzcomm_set_fd(p->fd());
	proto.ggzcomm_network_main();
}

void Game::slotNotification(ggz_starterpackOpcodes::Opcode messagetype, const msg& message)
{
	/* Here we would receive messages from the client. But the starterpack
	protocol doesn't involve such messages. */

	Q_UNUSED(messagetype);
	Q_UNUSED(message);
}

void Game::slotError()
{
	/* Here we have to decide what to do if we receive an invalid message.
	For KGGZPacket-based protocols, we can assume that the network connection is
	all fine, else we would have received an error in slotEvent already.
	But maybe the client speaks a different protocol or tries to cheat.
	Therefore, just like in the case of connection errors, we simply terminate
	the game server. */

	kError() << "Something bad has happened with a game client!";

	shutdown();
}

void Game::shutdown()
{
	/* Here we terminate the game server, because we either have a broken
	network connection or received an unknown or invalid message from the
	client. Of course, real games would do something more sophisticated
	like just cutting off the player who caused the trouble, maybe
	replacing him/her with a bot. */

	kError() << "Something bad has happened with the GGZ connection!";

	if(m_module)
	{
		m_module->disconnect();
		delete m_module;
		m_module = 0;
	}

	QCoreApplication::exit();
}
