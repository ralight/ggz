//////////////////////////////////////////////////////////////////////
// KCC
// Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
// Published under GNU GPL conditions
//////////////////////////////////////////////////////////////////////

// Header file
#include "kcc.h"

// KDE includes
#include <klocale.h>
#include <kmessagebox.h>
#include <kconfig.h>
#include <kapplication.h>
#include <kdebug.h>

// Qt includes
#include <qlayout.h>
#include <qpixmap.h>
#include <qbitmap.h>
#include <qsocketnotifier.h>
#include <qpainter.h>

// System includes
#include <stdio.h>
#include <stdlib.h>

// Cons... Konstructor :-)
KCC::KCC(QWidget *parent, const char *name)
: QWidget(parent, name, WStyle_Customize | WRepaintNoErase)
{
	setErasePixmap(QPixmap(QString("%1/kcc/bg.png").arg(GGZDATADIR)));
//?	setMask(QBitmap(QString("%1/kcc/mask.png").arg(GGZDATADIR)));

	m_fx = -1;
	m_turn = 0;

	m_score_opp = 0;
	m_score_you = 0;

	proto = new KCCProto(this);
}

// Destructor
KCC::~KCC()
{
	proto->shutdown();
	delete proto;
}

// Evaluate your turn (after click)
void KCC::slotSelected(QWidget *widget)
{
}

// Prepare your turn
void KCC::yourTurn()
{
	if(proto->state == proto->statemove) emit signalStatus(i18n("Your turn"));
	proto->state = proto->statemove;
}

// Handle the opponent's turn
void KCC::opponentTurn()
{
	emit signalStatus(i18n("Opponent's turn"));

	if(gameOver()) return;
}

// Get the player who occupies the seat
int KCC::getPlayer(int seat)
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
void KCC::getNextTurn()
{
	m_turn = (++m_turn) % 2;

	if(m_turn == proto->num) yourTurn();
	else opponentTurn();

	drawBoard();
}

// Check for game over, and show dialogs
int KCC::gameOver()
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

	// evaluate if game is still in progress
	if(m_x != -1)
	{
		//getAI();
		if(m_winner)
		{
			emit signalStatus(i18n("Game Over!"));

			conf = kapp->config();
			conf->setGroup("Score");
			if((m_opponent == PLAYER_NETWORK) && (proto->num < 0))
			{
				announce(i18n("The game is over."));
			}
			else
			{
				if(m_winner == proto->opponent)
				{
					m_score_opp++;
					conf->writeEntry("humanwon", conf->readNumEntry("humanwon") + 1);
					announce(i18n("You lost the game."));
				}
				else
				{
					m_score_you++;
					conf->writeEntry("humanlost", conf->readNumEntry("humanlost") + 1);
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
		announce(i18n("The game is over. There is no winner."));
		emit signalGameOver();
		return 1;
	}

	return 0;
}

// Ask for yet another game (some people can't get enough)
void KCC::announce(QString str)
{
	int ret;

	return; // !!!!

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
void KCC::init()
{
	QSocketNotifier *sn;

	proto->init();

	proto->connect();
	sn = new QSocketNotifier(proto->fdcontrol, QSocketNotifier::Read, this);
	connect(sn, SIGNAL(activated(int)), SLOT(slotDispatch()));
}

// Specify the opponent type (network or AI)
void KCC::setOpponent(int type)
{
	m_opponent = type;
	emit signalScore(i18n("Network game"));
	emit signalStatus(i18n("Waiting for opponent!"));
}

// Synchronization
void KCC::sync()
{
	proto->sendSync();
}

// Statistics
void KCC::statistics()
{
	proto->sendStatistics();
}

// Handle network input
void KCC::network()
{
	QSocketNotifier *sn;

	sn = new QSocketNotifier(proto->fd, QSocketNotifier::Read, this);
	connect(sn, SIGNAL(activated(int)), SLOT(slotNetwork()));
}

// Network data
void KCC::slotNetwork()
{
	int op;

	op = proto->getOp();

	switch(op)
	{
		case proto->cc_msg_seat:
			proto->getSeat();
			kdDebug() << "*proto* got my seat: " << proto->num << endl;
			break;
		case proto->cc_msg_players:
			proto->getPlayers();
			proto->state = proto->statewait;
			emit signalScore(i18n("Network game"));
			kdDebug() << "*proto* got players " << endl;
			break;
		case proto->cc_req_move:
			proto->state = proto->statemove;
			m_turn = proto->num;
			emit signalStatus(i18n("Your move"));
			kdDebug() << "*proto* move requested" << endl;
			break;
		case proto->cc_rsp_move:
			switch(proto->getMoveStatus())
			{
				case proto->errnone:
					emit signalStatus(i18n("Move accepted"));
					break;
				case proto->errother:
					emit signalStatus(i18n("Move invalid"));
					break;
				default:
					emit signalStatus(i18n("Move invalid"));
			}
			//getNextTurn();
			kdDebug() << "*proto* rsp_move" << endl;
			break;
		/*case proto->cc_msg_move:
			proto->getOpponentMove();
			if(proto->num < 0) emit signalStatus(i18n("Watching the game"));
			break;*/
		case proto->cc_msg_sync:
			proto->getSync();
			kdDebug() << "*proto* sync" << endl;
			break;
		case proto->cc_msg_gameover:
			proto->getGameOver();
			kdDebug() << "*proto* gameover" << endl;
			proto->state = proto->statedone;
			gameOver();
			break;
	}
	drawBoard();
}

// Draw the game board
void KCC::drawBoard()
{
	QPainter p;
	QString s;

	p.begin(this);

	for(int j = 0; j < 17; j++)
		for(int i = 0; i < 17; i++)
		{
			if(proto->board[i][j])
				p.drawPixmap(i * 18 + 50 + (j % 2) * 9, j * 18 + 50,
					QPixmap(QString("%1/kcc/point%2.png").arg(GGZDATADIR).arg(proto->board[i][j] - 1)));
		}

	p.end();
}

// Evaluate network control input
void KCC::slotDispatch()
{
	proto->dispatch();
}

void KCC::mousePressEvent(QMouseEvent *e)
{
	int x, y;

	kdDebug() << "mouse!" << endl;
	if(m_turn != proto->num) return;

	y = (e->y() - 50) / 18;
	x = ((e->x() - 50) - (y % 2) * 9) / 18;

	kdDebug() << "* " << e->x() << ", " << e->y() << endl;
	kdDebug() << x << y << endl;

	if(m_fx == -1)
	{
		m_fx = e->x();
		m_fy = e->y();
	}
	else
	{
		proto->sendMyMove(m_fx, m_fy, e->x(), e->y());
		m_fx = -1;
	}
}

void KCC::paintEvent(QPaintEvent *e)
{
	drawBoard();
}

