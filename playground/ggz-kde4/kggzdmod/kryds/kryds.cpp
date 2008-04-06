#include "kryds.h"

#include <kggzdmod/module.h>
#include <kggzdmod/player.h>

#include <kdebug.h>

#include <QCoreApplication>

// FIXME: these should appear in protocol
#define TTT_OK           0
#define TTT_ERR_STATE   -1
#define TTT_ERR_TURN    -2
#define TTT_ERR_BOUND   -3
#define TTT_ERR_FULL    -4

#define TTT_NO_WINNER    2

Kryds::Kryds(QObject *parent)
: QObject(parent)
{
	m_started = false;
	m_turn = -1;
	m_currentplayer = 0;

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

	for(int i = 0; i < 9; i++)
		if(m_board[i] == -1)
			over = false;

	// TODO: we check for full board but not for win yet

	if(!over)
		nextPlayer();
	else
	{
		msggameover over;
		over.winner = TTT_NO_WINNER;

		tictactoe proto;
		QList<KGGZdMod::Player*> players = m_module->players();
		for(int i = 0; i < players.size(); i++)
		{
			KGGZdMod::Player *p = players.at(i);
			if(p->fd() != -1)
			{
				proto.ggzcomm_set_fd(p->fd());
				proto.ggzcomm_msggameover(over);
			}
		}
	}
}

void Kryds::handleInput(KGGZdMod::Player *p)
{
	kDebug() << "Input from player at #" << p->seat();

	m_currentplayer = p;

	tictactoe proto;
	connect(&proto,
		SIGNAL(signalNotification(tictactoeOpcodes::Opcode, const msg&)),
		SLOT(slotNotification(tictactoeOpcodes::Opcode, const msg&)));
	connect(&proto, SIGNAL(signalError()), SLOT(slotError()));

	proto.ggzcomm_set_fd(p->fd());
	proto.ggzcomm_network_main();

	m_currentplayer = 0;
}

void Kryds::slotNotification(tictactoeOpcodes::Opcode messagetype, const msg& message)
{
	if(messagetype == tictactoeOpcodes::message_sndmove)
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
	else if(messagetype == tictactoeOpcodes::message_reqsync)
	{
	}
}

void Kryds::slotError()
{
	kError() << "Something bad has happened with a game client!";

	// FIXME: do something with m_currentplayer, like disconnecting
}

void Kryds::shutdown()
{
	kError() << "Something bad has happened with the GGZ connection!";
	//deleteLater();
	// FIXME: deleteLater would require Qt mainloop, but we use ggzdmod loop
	// TODO: make it possible to hook in Qt mainloop

	if(m_module)
	{
		//m_module->sendRequest(KGGZdMod::StateRequest(KGGZdMod::Module::done));
		// FIXME: the above can only be done for game client disconnects
		// otherwise it would trigger an error immediately again!

		m_module->disconnect();
		delete m_module;
		m_module = 0;
	}

	QCoreApplication::exit();
}
