#include "kryds.h"
#include "tictactoe_server.h"

#include <kggzdmod/module.h>
#include <kggzdmod/player.h>

#include <kdebug.h>

Kryds::Kryds(QObject *parent)
: QObject(parent)
{
	m_started = false;
	m_turn = -1;

	m_module = new KGGZdMod::Module("Kryds");

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
		//KGGZdMod::PlayerDataEvent e(event);
		// ...
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
		//KGGZdMod::ErrorEvent e(event);
		// ...
	}
}

void Kryds::nextPlayer()
{
	QList<KGGZdMod::Player*> players = m_module->players();

	m_turn = 1 - m_turn;

	if(players.at(m_turn)->fd() != -1)
	{
		// Human player

		tictactoe proto;
		proto.ggzcomm_set_fd(players.at(m_turn)->fd());
		proto.ggzcomm_reqmove(reqmove());
	}
	else
	{
		// Bot player

		int move;
		while(true)
		{
			move = random() % 9;
			if(m_board[move] == -1)
				break;
		}
		m_board[move] = m_turn;

		msgmove m;
		m.player = m_turn;
		m.move = move;

		tictactoe proto;
		proto.ggzcomm_set_fd(players.at(1 - m_turn)->fd());
		proto.ggzcomm_msgmove(m);

		detectGameOver();
	}
}

void Kryds::detectGameOver()
{
	bool over = true;

	for(int i = 0; i < m_board[i]; i++)
		if(m_board[i] != -1)
			over = false;

	// TODO: we check for full board but not for win yet

	if(!over)
		nextPlayer();
}
