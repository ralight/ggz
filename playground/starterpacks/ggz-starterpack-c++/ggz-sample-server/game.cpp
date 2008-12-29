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

void Kryds::slotEvent(const KGGZdMod::Event& event)
{
	kDebug() << "GGZ event:" << event.type();

	if(event.type() == KGGZdMod::Event::state)
	{
		KGGZdMod::StateEvent se(event);
		kDebug() << "State change: from" << se.oldstate() << "to" << se.state();
	}
	else if(event.type() == KGGZdMod::Event::playerseat)
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
			if(!m_started)
			{
				m_started = true;

				m_module->sendRequest(KGGZdMod::StateRequest(KGGZdMod::Module::playing));

				m_turn = 1;
				for(int i = 0; i < 9; i++)
					m_board[i] = -1;

				nextPlayer();
			}
		}
		else
		{
			kDebug() << "Not all seats full, delay/interrupt game";
		}
	}
	else if(event.type() == KGGZdMod::Event::spectatorseat)
	{
		//KGGZdMod::SpectatorSeatEvent e(event);
		// ...
	}
	else if(event.type() == KGGZdMod::Event::playerdata)
	{
		KGGZdMod::PlayerDataEvent e(event);

		// FIXME: introduce findPlayerBySeat() method?
		// FIXME: or can we rely on at() method?
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
	else if(event.type() == KGGZdMod::Event::spectatordata)
	{
		//KGGZdMod::SpectatorDataEvent e(event);
		// ...
	}
	else if(event.type() == KGGZdMod::Event::savegame)
	{
		//KGGZdMod::SavegameEvent e(event);
		// ...
	}
	else if(event.type() == KGGZdMod::Event::error)
	{
		KGGZdMod::ErrorEvent e(event);
		kError() << e.message();

		// share GGZ error handler with game client handler
		shutdown();
	}
}

void Kryds::handleInput(KGGZdMod::Player *p)
{
	kDebug() << "Input from player at #" << p->seat();

	m_currentplayer = p;

	tictactoe proto;
	connect(&proto,
		SIGNAL(signalNotification(ggz_starterpackOpcodes::Opcode, const msg&)),
		SLOT(slotNotification(ggz_starterpackOpcodes::Opcode, const msg&)));
	connect(&proto, SIGNAL(signalError()), SLOT(slotError()));

	proto.ggzcomm_set_fd(p->fd());
	proto.ggzcomm_network_main();

	m_currentplayer = 0;
}

void Kryds::slotNotification(ggz_starterpackOpcodes::Opcode messagetype, const msg& message)
{
	if(messagetype == ggz_starterpackOpcodes::message_sndmove)
	{
		const sndmove *m = static_cast<const sndmove*>(&message);
		kDebug() << "Move:" << m->move_c;

		int ret = TTT_OK;
		if(m_module->state() != KGGZdMod::Module::playing)
			ret = TTT_ERR_STATE;
		else if(m_turn != m_currentplayer->seat())
			ret = TTT_ERR_TURN;
		else if((m->move_c < 0) || (m->move_c >= 9))
			ret = TTT_ERR_BOUND;
		else if(m_board[m->move_c] != -1)
			ret = TTT_ERR_FULL;

		rspmove rsp;
		rsp.status = ret;

		msgmove msg;
		msg.player = m_turn;
		msg.move = m->move_c;

		tictactoe proto;
		proto.ggzcomm_set_fd(m_currentplayer->fd());
		proto.ggzcomm_rspmove(rsp);

		QList<KGGZdMod::Player*> players = m_module->players();
		KGGZdMod::Player *opp = players.at(1 - m_turn);
		if(opp->fd() != -1)
		{
			proto.ggzcomm_set_fd(opp->fd());
			proto.ggzcomm_msgmove(msg);
		}

		if(ret == TTT_OK)
		{
			m_board[m->move_c] = m_turn;
			detectGameOver();
		}
	}
	else if(messagetype == ggz_starterpackOpcodes::message_reqsync)
	{
	}
}

void Kryds::slotError()
{
	kError() << "Something bad has happened with a game client!";
}

void Kryds::shutdown()
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
