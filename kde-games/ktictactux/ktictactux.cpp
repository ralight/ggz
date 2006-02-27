//////////////////////////////////////////////////////////////////////
// KTicTacTux
// Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
// Published under GNU GPL conditions
//////////////////////////////////////////////////////////////////////

// Header file
#include "ktictactux.h"

// KTicTacTux includes
#include "qwhiteframe.h"

// KDE includes
#include <klocale.h>
#include <kmessagebox.h>
#include <kconfig.h>
#include <kapplication.h>

// Qt includes
#include <qlayout.h>
#include <qpixmap.h>
#include <qsocketnotifier.h>

// System includes
#include <stdio.h>
#include <stdlib.h>

// Cons... Konstructor :-)
KTicTacTux::KTicTacTux(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox, *vbox2;
	QHBoxLayout *hbox[3];
	QWidget *container;

	container = new QWidget(this);
	container->setErasePixmap(QPixmap(QString("%1/ktictactux/bg.png").arg(GGZDATADIR)));

	vbox = new QVBoxLayout(this);
	vbox->add(container);

	vbox2 = new QVBoxLayout(container);
	vbox2->addStretch(1);
	for(int j = 0; j < 3; j++)
	{
		hbox[j] = new QHBoxLayout(vbox2);
		hbox[j]->addStretch(1);
		for(int i = 0; i < 3; i++)
		{
			frame[i][j] = new QWhiteFrame(j * 3 + i, container);
			frame[i][j]->setFixedSize(64, 64);
			hbox[j]->add(frame[i][j]);
			connect(frame[i][j], SIGNAL(signalSelected(QWidget *)), SLOT(slotSelected(QWidget *)));
		}
		hbox[j]->addStretch(1);
	}
	vbox2->addStretch(1);

	//setFixedSize(210, 210);
	setCaption("KTicTacTux");
	show();

	//m_firstid = frame[0][0]->winId();
	m_turn = 0;

	m_score_opp = 0;
	m_score_you = 0;

	proto = new KTicTacTuxProto(this);
}

// Destructor
KTicTacTux::~KTicTacTux()
{
	proto->shutdown();
	delete proto;
}

// Evaluate your turn (after click)
void KTicTacTux::slotSelected(QWidget *widget)
{
	QWhiteFrame *tmp;
	int id;

	if(proto->state != proto->statemove) return;
	if(m_turn != proto->num) return;

	//id = widget->winId() - m_firstid;
	tmp = reinterpret_cast<QWhiteFrame*>(widget);
	id = tmp->id();

	if(proto->board[id % 3][id / 3] == proto->player) return;
	if(proto->board[id % 3][id / 3] == proto->opponent) return;

	if(m_opponent == PLAYER_AI)
	{
		proto->board[id % 3][id / 3] = proto->player;
		drawBoard();
		getNextTurn();
	}
	else
	{
		proto->move = id;
		proto->sendMyMove();
	}

	gameOver();
}

// Prepare your turn
void KTicTacTux::yourTurn()
{
	if((m_opponent == PLAYER_AI) || (proto->state == proto->statemove)) emit signalStatus(i18n("Your turn"));
	proto->state = proto->statemove;
}

// Handle the opponent's turn
void KTicTacTux::opponentTurn()
{
	if(m_opponent == PLAYER_AI) emit signalStatus(i18n("AI's turn"));
	else emit signalStatus(i18n("Opponent's turn"));

	if(gameOver()) return;

	if(m_opponent == PLAYER_AI)
	{
		proto->board[m_x][m_y] = proto->opponent;
		drawBoard();
		getNextTurn();
	}
}

// Get the player who occupies the seat
int KTicTacTux::getPlayer(int seat)
{
	switch(seat)
	{
		case 0:
			return PLAYER_HUMAN;
			break;
		case 1:
			return m_opponent;
			break;
	}

	return PLAYER_NONE;
}

// Turn switch (not soooo difficult for 2 players)
void KTicTacTux::getNextTurn()
{
	m_turn = (++m_turn) % 2;

	if(m_turn == proto->num) yourTurn();
	else opponentTurn();

	drawBoard();
}

// Check for game over, and show dialogs
int KTicTacTux::gameOver()
{
	m_x = -1;
	KConfig *conf;

	// Check for draw (no empty fields left)
	for(int j = 0; j < 3; j++)
		for(int i = 0; i < 3; i++)
			if(proto->board[i][j] == proto->none)
			{
				m_x = i;
				m_y = j;
			}

	conf = kapp->config();
	conf->setGroup("Score");

	// evaluate if game is still in progress
	if(m_x != -1)
	{
		getAI();
		if(m_winner)
		{
			emit signalStatus(i18n("Game Over!"));

			if((m_opponent == PLAYER_NETWORK) && (proto->num < 0))
			{
				announce(i18n("The game is over."));
			}
			else
			{
				if(m_winner == proto->opponent)
				{
					m_score_opp++;
					if(m_opponent == PLAYER_NETWORK) conf->writeEntry("humanwon", conf->readNumEntry("humanwon") + 1);
					else conf->writeEntry("aiwon", conf->readNumEntry("aiwon") + 1);
					conf->sync();
					announce(i18n("You lost the game."));
				}
				else
				{
					m_score_you++;
					if(m_opponent == PLAYER_NETWORK) conf->writeEntry("humanlost", conf->readNumEntry("humanlost") + 1);
					else conf->writeEntry("ailost", conf->readNumEntry("ailost") + 1);
					conf->sync();
					announce(i18n("You are the winner!"));
				}
			}
			emit signalGameOver();
			return 1;
		}
		else return 0;
	}
	else
	{
		emit signalStatus(i18n("Game Over!"));

		if(m_opponent == PLAYER_NETWORK) conf->writeEntry("humantied", conf->readNumEntry("humantied") + 1);
		else conf->writeEntry("aitied", conf->readNumEntry("aitied") + 1);
		conf->sync();
		announce(i18n("The game is over. There is no winner."));
		emit signalGameOver();
		return 1;
	}

	return 0;
}

// Ask for yet another game (some people can't get enough)
void KTicTacTux::announce(QString str)
{
	int ret;

	if(m_opponent == PLAYER_NETWORK) return;

	// Announce the new score
	emit signalScore(i18n(QString("Score: you %1, opponent %2")).arg(m_score_you).arg(m_score_opp));

	ret = KMessageBox::questionYesNo(this, str + "\n\n" + i18n("Play another game?"), i18n("Game over"));

	switch(ret)
	{
		case KMessageBox::Yes:
			init();
			break;
		case KMessageBox::No:
			exit(-1);
			break;
	}
}

// Initialize either network or AI mode
void KTicTacTux::init()
{
	QSocketNotifier *sn;

	proto->init();

	if(m_opponent == PLAYER_NETWORK)
	{
		proto->connect();
		sn = new QSocketNotifier(proto->fdcontrol, QSocketNotifier::Read, this);
		connect(sn, SIGNAL(activated(int)), SLOT(slotDispatch()));
	}
	else
	{
		m_turn++;
		proto->seats[0] = getPlayer(0);
    	proto->seats[1] = getPlayer(1);
		getNextTurn();
	}
}

// Two functions in one: get AI moves and check for winner
void KTicTacTux::getAI()
{
	m_winner = proto->none;
	int c;

	// Special case detected by Rich: C, LR, UL, LC!
	c = proto->board[1][1];
	if(c != proto->none)
	{
		for(int j = 0; j < 3; j += 2)
			for(int i = 0; i < 3; i += 2)
			{
				if((proto->board[i][j] == c)
				&& (proto->board[2 - i][2 - j] != c)
				&& (proto->board[2 - i][2 - j] != proto->none))
				{
					if(proto->board[i][2 - j] == proto->none)
					{
						m_x = i;
						m_y = 2 - j;
					}
					if(proto->board[2 - i][j] == proto->none)
					{
						m_x = 2 - i;
						m_y = j;
					}
				}
			}
	}

	m_seewinner = 0;

	// Go straight for the middle if possible
	if(c == proto->none)
	{
		m_x = 1;
		m_y = 1;
		m_seewinner = 1;
	}

	// Normal AI operations
	getAIAt(0, 0, 1, 1);
	getAIAt(2, 0, -1, 1);
	for(int i = 0; i < 3; i++)
	{
		getAIAt(i, 0, 0, 1);
		getAIAt(0, i, 1, 0);
	}
}

// Try to find 3rd field in any row of 2
void KTicTacTux::getAIAt(int xo, int yo, int xp, int yp)
{
	int x, y;

	for(int i = 0; i < 3; i++)
	{
		x = xo + xp * i;
		y = yo + yp * i;
		if((proto->board[trip(x)][trip(y)] != proto->none)
		&& (proto->board[trip(x + xp)][trip(y + yp)] == proto->board[trip(x)][trip(y)]))
		{
			if(proto->board[trip(x + xp * 2)][trip(y + yp * 2)] != proto->board[trip(x)][trip(y)])
			{
				if((proto->board[trip(x + xp * 2)][trip(y + yp * 2)] == proto->none) && (!m_seewinner))
				{
					m_x = trip(x + xp * 2);
					m_y = trip(y + yp * 2);
					// take unlimited chance
					if(proto->board[trip(x)][trip(y)] == proto->opponent) m_seewinner = 1;
				}
			}
			else m_winner = proto->board[trip(x + xp * 2)][trip(y + yp * 2)];
		}
	}
}

// Keep value in range 0..2, with rotation
int KTicTacTux::trip(int value)
{
	int ret;

	ret = value % 3;
	if(ret < 0) ret = 3 + ret;
	return ret;
}

// Specify the opponent type (network or AI)
void KTicTacTux::setOpponent(int type)
{
	m_opponent = type;
	if(m_opponent == PLAYER_NETWORK)
		emit signalScore(i18n("Network game"));
	else proto->num = 0;
	emit signalStatus(i18n("Waiting for opponent!"));
}

// Synchronization
void KTicTacTux::sync()
{
	proto->sendSync();
}

// Statistics
void KTicTacTux::statistics()
{
	proto->getStatistics();

	emit signalNetworkScore(proto->stats[0], proto->stats[1], proto->stats[2]);
}

// Handle network input
void KTicTacTux::network()
{
	QSocketNotifier *sn;

	sn = new QSocketNotifier(proto->fd, QSocketNotifier::Read, this);
	connect(sn, SIGNAL(activated(int)), SLOT(slotNetwork()));
}

// Network data
void KTicTacTux::slotNetwork()
{
	int op;

	op = proto->getOp();

	switch(op)
	{
		case KTicTacTuxProto::msgseat:
			proto->getSeat();
			break;
		case KTicTacTuxProto::msgplayers:
			proto->getPlayers();
			proto->state = proto->statewait;
			if((proto->num >= 0) && (proto->names[!proto->num][0]))
				emit signalScore(i18n("Network game with %1").arg(proto->names[!proto->num]));
			break;
		case KTicTacTuxProto::reqmove:
			proto->state = proto->statemove;
			m_turn = proto->num;
			emit signalStatus(i18n("Your move"));
			break;
		case KTicTacTuxProto::rspmove:
			switch(proto->getMoveStatus())
			{
				case KTicTacTuxProto::errstate:
					emit signalStatus(i18n("*server*"));
					break;
				case KTicTacTuxProto::errturn:
					emit signalStatus(i18n("*turn*"));
					break;
				case KTicTacTuxProto::errbound:
					emit signalStatus(i18n("*bounds*"));
					break;
				case KTicTacTuxProto::errfull:
					emit signalStatus(i18n("*occupied*"));
					break;
				default:
					emit signalStatus(i18n("Move OK"));
			}
			getNextTurn();
			break;
		case KTicTacTuxProto::msgmove:
			proto->getOpponentMove();
			if(proto->num < 0) emit signalStatus(i18n("Watching the game"));
			break;
		case KTicTacTuxProto::sndsync:
			proto->getSync();
			break;
//		case proto->sndstats:
//			proto->getStatistics();
//			emit signalNetworkScore(proto->stats[0], proto->stats[1]);
//			break;
		case KTicTacTuxProto::msggameover:
			proto->getGameOver();
			proto->state = proto->statedone;
			gameOver();
			break;
	}
	drawBoard();
}

// Draw the game board
void KTicTacTux::drawBoard()
{
	for(int i = 0; i < 9; i++)
	{
		switch(proto->board[i % 3][i / 3])
		{
			case KTicTacTuxProto::player:
				frame[i % 3][i / 3]->setPaletteBackgroundPixmap(QPixmap(m_t1));
				break;
			case KTicTacTuxProto::opponent:
				frame[i % 3][i / 3]->setPaletteBackgroundPixmap(QPixmap(m_t2));
				break;
			default:
				frame[i % 3][i / 3]->setPaletteBackgroundPixmap(NULL);
		}
	}
	update();
}

// Sets the theme
void KTicTacTux::setTheme(QString t1, QString t2)
{
	m_t1 = t1;
	m_t2 = t2;
	drawBoard();
}

// Evaluate network control input
void KTicTacTux::slotDispatch()
{
	proto->dispatch();
}

GGZMod *KTicTacTux::getMod()
{
	return proto->mod;
}

