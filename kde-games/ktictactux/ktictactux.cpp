// Header file
#include "ktictactux.h"

// KDE includes
#include <klocale.h>
#include <kmessagebox.h>

// Qt includes
#include <qlayout.h>
#include <qpixmap.h>
#include <qsocketnotifier.h>

// System includes
#include <stdio.h>
#include <stdlib.h>

KTicTacTux::KTicTacTux(QWidget *parent = NULL, char *name = NULL)
: KMainWindow(parent, name)
{
	QVBoxLayout *vbox;
	QHBoxLayout *hbox[3];

	vbox = new QVBoxLayout(this);
	for(int j = 0; j < 3; j++)
	{
		hbox[j] = new QHBoxLayout(vbox);
		for(int i = 0; i < 3; i++)
		{
			frame[i][j] = new QWhiteFrame(this);
			hbox[j]->add(frame[i][j]);
			connect(frame[i][j], SIGNAL(signalSelected(QWidget *)), SLOT(slotSelected(QWidget *)));
		}
	}

	label = new QLabel(i18n("KTicTacTux - Waiting for opponent!"), this);
	label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	label->setFixedHeight(20);
	vbox->add(label);

	setFixedSize(210, 210);
	setCaption("KTicTacTux");
	show();

	m_firstid = frame[0][0]->winId();
	m_turn = 0;

	m_opponent = PLAYER_AI;

	m_score_opp = 0;
	m_score_you = 0;

	proto = new KTicTacTuxProto();
	proto->num = 0;

	//init();
}

KTicTacTux::~KTicTacTux()
{
	delete proto;
}

void KTicTacTux::slotSelected(QWidget *widget)
{
	int id;

	//printf("quickdebug: %i/%i\n", proto->state, proto->statemove);
	if(proto->state != proto->statemove) return;
	if(m_turn != proto->num)
	{
		//printf("Hey, wrong turn!!! (%i should be %i)\n", m_turn, proto->num);
		return;
	}

	id = widget->winId() - m_firstid;

	//printf("selected %i... checking...\n", id);
	if(proto->board[id % 3][id / 3] == proto->player) return;
	if(proto->board[id % 3][id / 3] == proto->opponent) return;
	//printf("ok, let's go!\n");

	if(m_opponent == PLAYER_AI)
	{
		proto->board[id % 3][id / 3] = proto->player;
		getNextTurn();
	}
	else
	{
		proto->move = id;
		proto->sendMyMove();
	}

	//getNextTurn();
}

void KTicTacTux::yourTurn()
{
	if((m_opponent == PLAYER_AI) || (proto->state == proto->statemove)) status(i18n("Your turn"));
	proto->state = proto->statemove;

	if(gameOver()) return;
}

void KTicTacTux::opponentTurn()
{
	if(m_opponent == PLAYER_AI) status(i18n("AI's turn"));
	else status(i18n("Opp's turn"));

	if(gameOver()) return;

	if(m_opponent == PLAYER_AI)
	{
		proto->board[m_x][m_y] = proto->opponent;

		getNextTurn();
	}
}

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

void KTicTacTux::getNextTurn()
{
	m_turn = (++m_turn) % 2;

	//m_turn++;
	//if(m_turn > 2) m_turn = 1;

	//printf("-turn: %i- (%i)\n", m_turn, proto->num);
	if(m_turn == proto->num) yourTurn();
	else opponentTurn();

	drawBoard();
}

int KTicTacTux::gameOver()
{
	m_x = -1;

	// Check for draw
	for(int j = 0; j < 3; j++)
	{
		for(int i = 0; i < 3; i++)
		{
			if(proto->board[i][j] == proto->none)
			{
				m_x = i;
				m_y = j;
			}
		}
	}

	// evaluate
	if(m_x != -1)
	{
		getAI();
		if(m_winner)
		{
			status(i18n("Game Over!"));
			if(m_winner == proto->opponent)
			{
				m_score_opp++;
				announce(i18n("You lost the game."));
			}
			else
			{
				m_score_you++;
				announce(i18n("You are the winner!"));
			}
			return 1;
		}
		else return 0;
	}
	else
	{
		status(i18n("Game Over!"));
		announce(i18n("The game is over. There is no winner."));
		return 1;
	}

	return 0;
}

void KTicTacTux::announce(QString str)
{
	int ret;

	if(m_opponent == PLAYER_NETWORK) return;

	ret = KMessageBox::questionYesNo(this, str + "\n\n" + i18n("Play another game?"), "Information");

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

void KTicTacTux::init()
{
	QSocketNotifier *sn;

	proto->init();
	if(m_opponent == PLAYER_NETWORK)
	{
		proto->connect();
		sn = new QSocketNotifier(proto->fd, QSocketNotifier::Read, this);
		connect(sn, SIGNAL(activated(int)), SLOT(slotNetwork()));
	}
	else
	{
		proto->seats[0] = getPlayer(0);
    	proto->seats[1] = getPlayer(1);
	}

	for(int i = 0; i < 9; i++)
		frame[i % 3][i / 3]->setBackgroundPixmap(NULL);

	m_turn++;
	getNextTurn();
}

void KTicTacTux::status(QString str)
{
	str.sprintf(str + " (" + i18n("Score: you %i, opp %i") + ")", m_score_you, m_score_opp);
	label->setText(str);
}

void KTicTacTux::getAI()
{
	m_winner = proto->none;

	//printf("%i\n", m_winner);
	getAIAt(0, 0, 1, 1);
	getAIAt(2, 0, -1, 1);
	for(int i = 0; i < 3; i++)
	{
		getAIAt(i, 0, 0, 1);
		getAIAt(0, i, 1, 0);
	}
	//printf("%i\n", m_winner);
}

void KTicTacTux::getAIAt(int xo, int yo, int xp, int yp)
{
	int x, y;

	for(int i = 0; i < 3; i++)
	{
		x = xo + xp * i;
		y = yo + yp * i;
		//if(i) printf("  ");
		//printf("assign: start = %i/%i (%i/%i) (%i/%i)\n", x, y, trip(x + xp), trip(y + yp),
			//trip(x + xp * 2), trip(y + yp * 2));
		if((proto->board[trip(x)][trip(y)] != proto->none)
		&& (proto->board[trip(x + xp)][trip(y + yp)] == proto->board[trip(x)][trip(y)]))
		{
			if(proto->board[trip(x + xp * 2)][trip(y + yp * 2)] != proto->board[trip(x)][trip(y)])
			{
				if(proto->board[trip(x + xp * 2)][trip(y + yp * 2)] == proto->none)
				{
					//printf("** ASSIGN: opp = %i/%i\n", trip(x + xp * 2), trip(y + yp * 2));
					m_x = trip(x + xp * 2);
					m_y = trip(y + yp * 2);
				}
			}
			else
			{
				//printf("** ASSIGN: winner = %i/%i\n", trip(x + xp * 2), trip(y + yp * 2));
				m_winner = proto->board[trip(x + xp * 2)][trip(y + yp * 2)];
			}
		}
	}
}

int KTicTacTux::trip(int value)
{
	int ret;

	ret = value % 3;
	if(ret < 0) ret = 3 + ret;
	return ret;
}

void KTicTacTux::setOpponent(int type)
{
	m_opponent = type;
}

void KTicTacTux::slotNetwork()
{
	int op;

	es_read_int(proto->fd, &op);

	switch(op)
	{
		case proto->msgseat:
		//printf("=== Msg Seat!\n");
			proto->getSeat();
			break;
		case proto->msgplayers:
		//printf("=== Msg Players!\n");
			proto->getPlayers();
			proto->state = proto->statewait;
			break;
		case proto->reqmove:
		//printf("=== Req Move!\n");
			proto->state = proto->statemove;
			m_turn = proto->num;
			status(i18n("Your move"));
			break;
		case proto->rspmove:
		//printf("=== Rsp Move!\n");
			switch(proto->getMoveStatus())
			{
				case proto->errstate:
					status(i18n("*server*"));
					break;
				case proto->errturn:
					status(i18n("*turn*"));
					break;
				case proto->errbound:
					status(i18n("*bounds*"));
					break;
				case proto->errfull:
					status(i18n("*occupied*"));
					break;
				default:
					status(i18n("Move OK"));
			}
			getNextTurn();
			break;
		case proto->msgmove:
		//printf("=== Msg Move!\n");
			proto->getOpponentMove();
			//getNextTurn();
			break;
		case proto->sndsync:
		//printf("=== Send Sync!\n");
			proto->getSync();
			break;
		case proto->msggameover:
		//printf("=== Game Over!\n");
			proto->getGameOver();
			proto->state = proto->statedone;
			gameOver();
			break;
	}
	drawBoard();
}

void KTicTacTux::drawBoard()
{
	for(int i = 0; i < 9; i++)
	{
		switch(proto->board[i % 3][i / 3])
		{
			case proto->player:
				frame[i % 3][i / 3]->setBackgroundPixmap(QPixmap(GGZDATADIR "/ktictactux/tux.png"));
				break;
			case proto->opponent:
				frame[i % 3][i / 3]->setBackgroundPixmap(QPixmap(GGZDATADIR "/ktictactux/merlin.png"));
				break;
			default:
				//printf("Unkown: %i\n", proto->board[i % 3][i / 3]);
				frame[i % 3][i / 3]->setBackgroundPixmap(NULL);
		}
	}
	//printf("..board drawn..\n");
}
