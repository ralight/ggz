///////////////////////////////////////////////////////////////////
// KCC
// Copyright (C) 2003, 2004 Josef Spillner, josef@ggzgamingzone.org
// Published under GNU GPL conditions
///////////////////////////////////////////////////////////////////

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
#include <qtimer.h>

// System includes
#include <stdio.h>
#include <stdlib.h>

// Cons... Konstructor :-)
KCC::KCC(QWidget *parent, const char *name)
: QWidget(parent, name, WStyle_Customize | WRepaintNoErase)
{
	m_fx = -1;
	m_turn = 0;
	m_opponent = PLAYER_NONE;
	m_winner = false;

	m_score_opp = 0;
	m_score_you = 0;

	proto = new KCCProto(this);

	drawBoard();
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
	emit signalStatus(i18n("Your turn"));
	proto->state = KCCProto::statemove;
}

// Handle the opponent's turn
void KCC::opponentTurn()
{
	bool result;
	QPoint p;
	int tmp;

	if(m_opponent == PLAYER_AI)
	{
		kdDebug() << "check ai player " << m_turn << endl;
		result = false;
		for(int j = 0; j < 17; j++)
		{
			for(int i = 0; i < 15; i++)
			{
				if(proto->board[i][j] == m_turn + 2)
				{
					result = findTarget(QPoint(i, j), QPoint(), false);
					if(result)
					{
						kdDebug() << "ai moved from " << i << "/" << j <<
							" to " << m_ax << "/" << m_ay << endl;
						/*tmp = proto->board[m_fx][m_fy];
						proto->board[m_fx][m_fy] = m_turn + 2;
						proto->board[i][j] = tmp;*/
						tmp = proto->board[i][j];
						proto->board[m_ax][m_ay] = tmp;
						proto->board[i][j] = 1;
						break;
					}
				}
			}
			if(result) break;
		}

		getNextTurn();
	}

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

// Turn switch
void KCC::getNextTurn()
{
	if(proto->max)
		m_turn = (++m_turn) % proto->max;

	if(m_turn == proto->num) yourTurn();
	else
	{
		emit signalStatus(i18n("Opponent's turn"));
		proto->state = KCCProto::statewait;

		QTimer::singleShot(2000, this, SLOT(slotOpponentMove()));
	}

	drawBoard();
}

// Delayed opponent move
void KCC::slotOpponentMove()
{
	opponentTurn();
}

// Check for game over, and show dialogs
int KCC::gameOver()
{
	m_x = -1;
	KConfig *conf;

	// Check for draw (no empty fields left)
	for(int j = 0; j < 3; j++)
		for(int i = 0; i < 3; i++)
			if(proto->board[i][j] == KCCProto::none)
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
				if(m_winner == KCCProto::opponent)
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
	drawBoard();

	if(m_opponent == PLAYER_NETWORK)
	{
		proto->connect();
		sn = new QSocketNotifier(proto->fdcontrol, QSocketNotifier::Read, this);
		connect(sn, SIGNAL(activated(int)), SLOT(slotDispatch()));
	}
	else if(m_opponent == PLAYER_AI)
	{
		if(!proto->max) proto->max = 6;
		proto->num = 0;
		m_turn = proto->num;
		yourTurn();
	}
}

// Specify the opponent type (network or AI)
void KCC::setOpponent(int type)
{
	m_opponent = type;
	if(m_opponent == PLAYER_NETWORK)
	{
		emit signalScore(i18n("Network game"));
		emit signalStatus(i18n("Waiting for opponent!"));
	}
}

void KCC::setPlayers(int players)
{
	kdDebug() << "set number of players to " << players << endl;
	proto->max = players;
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
	char tmp;
	QString status;

	op = proto->getOp();

	switch(op)
	{
		case proto->cc_msg_seat:
			proto->getSeat();
			kdDebug() << "*proto* got my seat: " << proto->num << endl;
			break;
		case proto->cc_msg_players:
			proto->getPlayers();
			proto->state = KCCProto::statewait;
			emit signalScore(i18n("Network game"));
			kdDebug() << "*proto* got players " << endl;
			break;
		case proto->cc_req_move:
			proto->state = KCCProto::statemove;
			m_turn = proto->num;
			emit signalStatus(i18n("Your move"));
			kdDebug() << "*proto* move requested" << endl;
			break;
		case proto->cc_rsp_move:
			proto->getMoveStatus();
			switch(proto->status)
			{
				case proto->errnone:
					status = i18n("Move accepted");
					break;
				case proto->errstate:
					status = i18n("Table not yet full");
					break;
				default:
					status = i18n("Move invalid");
					break;
			}
			emit signalStatus(status);
			//getNextTurn();
			kdDebug() << "*proto* rsp_move " << status << ": " << (int)proto->status << endl;

			if(proto->status == KCCProto::errnone)
			{
				findTarget(QPoint(m_fx, m_fy), QPoint(m_tx, m_ty), false);
				tmp = proto->board[m_fx][m_fy];
				proto->board[m_fx][m_fy] = 1;
				proto->board[m_tx][m_ty] = tmp;
				//drawBoard();
			}
			m_fx = -1;
			break;
		case proto->cc_msg_move:
			kdDebug() << "*proto* msg_move" << endl;
			proto->getOpponentMove();
			//if(proto->num < 0) emit signalStatus(i18n("Watching the game"));
			findTarget(QPoint(proto->m_ox1, proto->m_oy1),
				QPoint(proto->m_ox2, proto->m_oy2), false);
			tmp = proto->board[proto->m_ox1][proto->m_oy1];
			proto->board[proto->m_ox1][proto->m_oy1] = 1;
			proto->board[proto->m_ox2][proto->m_oy2] = tmp;
			break;
		case proto->cc_msg_sync:
			proto->getSync();
			kdDebug() << "*proto* sync" << endl;
			break;
		case proto->cc_msg_gameover:
			proto->getGameOver();
			kdDebug() << "*proto* gameover" << endl;
			proto->state = KCCProto::statedone;
			gameOver();
			break;
	}
	drawBoard();
}

// Draw the game board
void KCC::drawBoard()
{
	QPainter p;
	QPoint p1, p2;

	QPixmap b(QString("%1/kcc/bg.png").arg(GGZDATADIR));
	//setMask(QBitmap(QString("%1/kcc/mask.png").arg(GGZDATADIR)));
	p.begin(&b);

	if(proto->state != KCCProto::statenone)
		for(int j = 0; j < 17; j++)
			for(int i = 0; i < 15; i++)
			{
				if(proto->board[i][j])
					p.drawPixmap(i * 22 + 40 + (j % 2) * 11, j * 19 + 44,
						QPixmap(QString("%1/kcc/point%2.png").arg(
							GGZDATADIR).arg(proto->board[i][j] - 1)));
			}

	QValueList<QPoint>::iterator it;
	if(m_waypoints.count() > 0)
	{
		it = m_waypoints.begin();
		p1 = (*it);
		it++;
		p.setPen(QPen(Qt::yellow, 2));
		for(; it != m_waypoints.end(); it++)
		{
			p2 = (*it);
/*kdDebug() << "<line> "
	<< p2.x() * 22 + 40 + (p2.y() % 2) * 11 + 10 << " "
	<< p2.y() * 19 + 44 + 10 << " "
	<< p1.x() * 22 + 40 + (p1.y() % 2) * 11 + 10 << " "
	<< p1.y() * 19 + 44 + 10
	<< endl;*/
			p.drawLine(p2.x() * 22 + 40 + (p2.y() % 2) * 11 + 5,
				p2.y() * 19 + 44 + 5,
				p1.x() * 22 + 40 + (p1.y() % 2) * 11 + 5,
				p1.y() * 19 + 44 + 5);
			p1 = (*it);
		}
	}

	p.end();
	setErasePixmap(b);
	//setPaletteBackgroundPixmap(b);
	repaint();
}

// Evaluate network control input
void KCC::slotDispatch()
{
	proto->dispatch();
}

QPoint KCC::newPoint(const QPoint& current, int direction)
{
	QPoint p;
	int offset;

	offset = current.y() % 2;

	switch(direction)
	{
		case left:
			p = QPoint(current.x() - 1, current.y());
			break;
		case right:
			p = QPoint(current.x() + 1, current.y());
			break;
		case leftup:
			p = QPoint(current.x() + offset - 1, current.y() - 1);
			break;
		case leftdown:
			p = QPoint(current.x() + offset - 1, current.y() + 1);
			break;
		case rightup:
			p = QPoint(current.x() + offset, current.y() - 1);
			break;
		case rightdown:
			p = QPoint(current.x() + offset, current.y() + 1);
			break;
		default:
			p = QPoint(current.x(), current.y());
	}
	return p;
}

bool KCC::findTarget(const QPoint& current, const QPoint& target, bool jumps)
{
	QPoint p, safepoint;
	bool found = false;
	//int self = 2; // XXX todo!

	if((current.x() < 0) || (current.x() >= 17)) return false;
	if((current.y() < 0) || (current.y() >= 17)) return false;
	if((target.x() < 0) || (target.x() >= 17)) return false;
	if((target.y() < 0) || (target.y() >= 17)) return false;

	if(!jumps) m_waypoints.clear();
	if(m_waypoints.count() > 0)
		if(proto->board[current.x()][current.y()] != 1) return false;
	if(current == target)
	{
		m_waypoints.append(current);
		//kdDebug() << "pre-INSERT " << current.x() << "/" << current.y() << endl;
		return true;
	}

	m_waypoints.append(current);

//kdDebug() << "INSERT " << current.x() << "/" << current.y() << endl;
/*QValueList<QPoint>::iterator it;
for (it = m_waypoints.begin(); it != m_waypoints.end(); ++it)
kdDebug() << " " << (*it).x() << "/" << (*it).y() << endl;*/

	for(int i = left; i <= leftdown; i++)
	{
		p = newPoint(current, i);
		if((!jumps) && (p == target))
		{
			//kdDebug() << "nonjump-INSERT " << target.x() << "/" << target.y() << endl;
			m_waypoints.append(target);
			found = true;
			break;
		}
		/*kdDebug() << "<: "<< i << " :> " << p.x() << "/" << p.y() << " = " <<
			(int)proto->board[p.x()][p.y()] << endl;*/
		if(!m_waypoints.contains(newPoint(p, i)))
		{
			//if(proto->board[p.x()][p.y()] == 1)
			//	findTarget(p, target);
			if(proto->board[p.x()][p.y()] > 1)
				found = findTarget(newPoint(p, i), target, true);
		}
		if(target.isNull())
		{
			if(!jumps)
			{
				if(proto->board[p.x()][p.y()] == 1)
					safepoint = p;
			}
			else
			{
				if(proto->board[current.x()][current.y()] == 1)
					safepoint = current;
			}
		}
		if(found) break;
	}
	if(!found)
	{
		if(!safepoint.isNull())
		{
			m_ax = safepoint.x();
			m_ay = safepoint.y();
			//kdDebug() << "safepoint-INSERT " << m_ax << "/" << m_ay << endl;
			m_waypoints.append(QPoint(m_ax, m_ay));
			found = true; // auto-ai
		}
	}

	//if(found) kdDebug() << ":: found" << endl;
	if(!found)
	{
		m_waypoints.remove(m_waypoints.last());
		//kdDebug() << ":: not found" << endl;
	}
	return found;
}

void KCC::mousePressEvent(QMouseEvent *e)
{
	int x, y;
	char tmp;
	int error;

	if(m_turn != proto->num) return;

	y = (e->y() - 44) / 19;
	x = ((e->x() - 40) - (y % 2) * 11) / 22;

	//kdDebug() << "* " << e->x() << ", " << e->y() << endl;
	//kdDebug() << x << ":" << y << endl;

	if(m_fx == -1)
	{
		m_fx = x;
		m_fy = y;
	}
	else
	{
		if(m_opponent == PLAYER_NETWORK)
		{
			m_tx = x;
			m_ty = y;

			//kdDebug() << "send " << m_fy << "/" << (m_fx * 2) + (m_fy % 2) - 2
			//	<< " - " << y << "/" << (x * 2) + (y % 2) - 2 << endl;
			/*m_ty = 16 - m_ty;
			m_fy = 16 - m_fy;
			y = 16 - y;*/
			kdDebug() << "send " << m_fy << "/" << m_fx
				<< " - " << y << "/" << x << endl;
			proto->sendMyMove(m_fx, m_fy, x, y);
		}
		else
		{
			error = 0;
			if((m_fx < 0) || (m_fx >= 17)) error = 1;
			if((m_fy < 0) || (m_fy >= 17)) error = 1;
			if((x < 0) || (x >= 17)) error = 1;
			if((y < 0) || (y >= 17)) error = 1;
			if(!error)
			{
				if(proto->board[m_fx][m_fy] != 2) error = 1;
				if(proto->board[x][y] != 1) error = 1;
				if(!findTarget(QPoint(m_fx, m_fy), QPoint(x, y), false)) error = 1;
			}
			else
			{
				KMessageBox::information(this,
					i18n("Move was out of bounds."),
					i18n("Move invalid"));
			}
			if(!error)
			{
				kdDebug() << "move from " << m_fx << "/" << m_fy <<
					" to " << x << "/" << y << endl;
				//findTarget(QPoint(m_fx, m_fy), QPoint(x, y));
				tmp = proto->board[m_fx][m_fy];
				proto->board[m_fx][m_fy] = 1;
				proto->board[x][y] = tmp;
				drawBoard();

				kdDebug() << "internal ai" << endl;
				getNextTurn();
			}
			else
			{
				if((m_fx != x) || (m_fy != y))
				{
					KMessageBox::information(this,
						i18n("Source or target invalid."),
						i18n("Move invalid"));
				}
			}
		}
		if(m_opponent == PLAYER_NETWORK)
			m_turn = -1;
		else
			m_fx = -1;
	}
}

//void KCC::paintEvent(QPaintEvent *e)
//{
//	drawBoard();
//}

