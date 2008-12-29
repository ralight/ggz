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

#include <time.h>

#include <QCoreApplication>

Game::Game(QObject *parent)
: QObject(parent)
{
	m_started = false;
	m_turn = -1;
	m_currentplayer = 0;

	m_module = new KGGZdMod::Module("Sample Server C++");

	connect(m_module,
		SIGNAL(signalEvent(const KGGZdMod::Event&)),
		SLOT(slotEvent(const KGGZdMod::Event&)));

	srandom(time(NULL));

	m_module->loop();
}

void Game::slotEvent(const KGGZdMod::Event& event)
{
	kDebug() << "GGZ event:" << event.type();

	/* Only a few events are shown here, there are many more available */
	if(event.type() == KGGZdMod::Event::playerseat)
	{
		KGGZdMod::PlayerSeatEvent pse(event);
		kDebug() << "Player seat change at #" << pse.seat().seat() << ": from"
			<< pse.oldseat().name() << "to" << pse.seat().name() <<
			"(seat change type " << pse.change() << ")";

		int open = 0;
		QList<KGGZdMod::Player*> players = m_module->players();
		for(int i = 0; i < players.size(); i++)
		{
			kDebug() << "* player at #" << i << "is" << players.at(i)->type();
			if(players.at(i)->type() == KGGZdMod::Player::open)
				open++;
		}

		if(!open)
		{
			kDebug() << "All seats full, start/continue game";
		}
		else
		{
			kDebug() << "Not all seats full, delay/interrupt game";
		}
	}
	else if(event.type() == KGGZdMod::Event::playerdata)
	{
		KGGZdMod::PlayerDataEvent e(event);

		QList<KGGZdMod::Player*> players = m_module->players();
		for(int i = 0; i < players.size(); i++)
		{
			if(players.at(i)->seat() == e.seat().seat())
			{
				KGGZdMod::Player *p = players.at(i);
				handleInput(p);
				break;
			}
		}
	}
	else if(event.type() == KGGZdMod::Event::error)
	{
		KGGZdMod::ErrorEvent e(event);
		kError() << e.message();

		// share GGZ error handler with game client handler
		shutdown();
	}
}

void Game::handleInput(KGGZdMod::Player *p)
{
	kDebug() << "Input from player at #" << p->seat();

	m_currentplayer = p;

	ggz_starterpack proto;
	connect(&proto,
		SIGNAL(signalNotification(ggz_starterpackOpcodes::Opcode, const msg&)),
		SLOT(slotNotification(ggz_starterpackOpcodes::Opcode, const msg&)));
	connect(&proto, SIGNAL(signalError()), SLOT(slotError()));

	proto.ggzcomm_set_fd(p->fd());
	proto.ggzcomm_network_main();

	m_currentplayer = 0;
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
	kError() << "Something bad has happened with a game client!";
}

void Game::shutdown()
{
	kError() << "Something bad has happened with the GGZ connection!";

	if(m_module)
	{
		m_module->disconnect();
		delete m_module;
		m_module = 0;
	}

	QCoreApplication::exit();
}
