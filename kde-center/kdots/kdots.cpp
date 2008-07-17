/////////////////////////////////////////////////////////////////////
//
// KDots: Connect the Dots game for KDE 4
// http://www.ggzgamingzone.org/gameclients/kdots/
//
// Copyright (C) 2001 - 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under the conditions of the GNU GPL, see COPYING
//
/////////////////////////////////////////////////////////////////////

#include "kdots.h"
#include "kdots_options.h"
#include "kdots_proto.h"

#include "qdots.h"

#include <kmessagebox.h>
#include <klocale.h>
#include <kstandarddirs.h>

#include <qmenubar.h>
#include <QLayout>
#include <QSocketNotifier>
#include <QDir>
#include <QDateTime>

#include <iostream>
#include <cstdlib>

KDots::KDots(bool ggzmode)
: QWidget()
{
	QVBoxLayout *vbox;
	KStandardDirs d;
	//QSocketNotifier *sn;

	dots = new QDots();
	QPixmap pix(d.findResource("data", "kdots/dragon.png"));
	dots->setBackgroundImage(pix);

	vbox = new QVBoxLayout();
	setLayout(vbox);
	vbox->addWidget(dots);

	kdots_options = NULL;

	connect(dots, SIGNAL(signalTurn(int, int, int)), SLOT(slotTurn(int, int, int)));

	// FIXME: background colour
	//setEraseColor(QColor(100, 100, 100));

	//setCaption("KDE Dots");
	setFixedSize(400, 400);
	show();

	if(ggzmode)
	{
		proto = new KDotsProto(this);
		proto->connect();
		proto->turn = -1;
		proto->num = -2;

		//sn = new QSocketNotifier(proto->fdcontrol, QSocketNotifier::Read, this);
		//connect(sn, SIGNAL(activated(int)), SLOT(slotDispatch()));
	}
	else proto = NULL;
}

KDots::~KDots()
{
	if(proto)
	{
		proto->disconnect();
		delete proto;
	}
}

KDotsProto *KDots::getProto()
{
	return proto;
}

void KDots::slotOptions()
{
	emit signalStatus(i18n("Requesting game options"));

	if(!kdots_options)
	{
		kdots_options = new KDotsOptions();
		connect(kdots_options, SIGNAL(signalAccepted(int, int)), this, SLOT(slotStart(int, int)));
	}
	kdots_options->show();
}

void KDots::slotStart(int horizontal, int vertical)
{
	emit signalStatus(i18n("Sending game options"));

	if(kdots_options) proto->sendOptions(horizontal + 1, vertical + 1);
}

void KDots::slotTurn(int x, int y, int direction)
{
	int sdotx, sdoty;

	if(proto->num < 0)
	{
		emit signalStatus(i18n("You're only watching"));
		return;
	}

	if(proto->turn != proto->num)
	{
		emit signalStatus(i18n("Not your turn!"));
		return;
	}

	sdotx = x;
	sdoty = y;

	proto->m_lastdir = direction;

	if(direction == QDots::up)
	{
		proto->m_lastdir = QDots::down;
		sdoty--;
	}
	if(direction == QDots::left)
	{
		proto->m_lastdir = QDots::right;
		sdotx--;
	}

	emit signalStatus(i18n("Sending move"));

	if((direction == QDots::up) || (direction == QDots::down))
	{
		proto->sendMove(sdotx, sdoty, proto->sndmovev);
	}
	else
	{
		proto->sendMove(sdotx, sdoty, proto->sndmoveh);
	}
}

void KDots::slotSync()
{
	if(!proto) return;
	if(proto->turn == -1)
	{
		KMessageBox::sorry(this, i18n("No game running yet!"), i18n("Synchronization"));
		return;
	}
	proto->sync();
}

void KDots::slotInput()
{
	int op;
	int msgret;
	QString str;
	QString savegame, savepath;
	char status;
	QDir dir;
	bool ret;

	if(proto->state == proto->statedone) return;

	emit signalStatus(i18n("Receiving data"));

	op = proto->getOpcode();

	switch(op)
	{
		case KDotsProto::msgseat:
			proto->getSeat();
			if(proto->num == 1) emit signalColor(QColor(0, 0, 250));
			else if(proto->num == 0) emit signalColor(QColor(0, 0, 50));
			else emit signalColor(QColor(255, 255, 255));
			break;
		case KDotsProto::msgplayers:
			proto->getPlayers();
			if(proto->state != proto->statechoose) proto->state = proto->statewait;
			break;
		case KDotsProto::msgoptions:
			proto->getOptions();
			dots->resizeBoard(proto->width - 1, proto->height - 1);
			dots->refreshBoard();
			break;
		case KDotsProto::reqmove:
			emit signalStatus(i18n("Your turn."));
			proto->state = proto->statemove;
			proto->turn = proto->num;
			break;
		case KDotsProto::msgmoveh:
			proto->getOppMove(proto->sndmoveh);
			dots->setBorderValue(proto->movex, proto->movey, QDots::right, proto->turn, Dots::move);
			dots->repaint();
			if(proto->num < 0) proto->turn = !proto->turn;
			break;
		case KDotsProto::msgmovev:
			proto->getOppMove(proto->sndmovev);
			dots->setBorderValue(proto->movex, proto->movey, QDots::down, proto->turn, Dots::move);
			dots->repaint();
			if(proto->num < 0) proto->turn = !proto->turn;
			break;
		case KDotsProto::rspmove:
			if(proto->getMove() != -1)
			{
				dots->setBorderValue(proto->m_lastx, proto->m_lasty, proto->m_lastdir, proto->turn, Dots::move);
				dots->repaint();
				proto->turn = (proto->num + 1) % 2;
			}
			else
			{
				emit signalStatus(i18n("Invalid move, please try again!"));
			}
			break;
		case KDotsProto::msggameover:
			savepath = QDir::home().path() + "/.ggz/games/kdots";
			dir.mkpath(savepath);
			savegame = QString("%1-%2-%3").arg(
				QDateTime::currentDateTime().toString()).arg(proto->players[0]).arg(proto->players[1]);
			ret = dots->save(savepath + "/" + savegame);
			if(!ret)
			{
				KMessageBox::error(this,
					i18n("The savegame could not be saved"),
					i18n("Savegame error"));
				return;
			}
			status = proto->getStatus();
			if(status == -1) exit(-1);
			proto->state = proto->statechoose;
			proto->turn = -1;
			emit signalStatus(i18n("Game over!"));
			str = i18n("The game is over.\nYou reached %1 points, your opponent %2.\n\nPlay another game?",
				dots->count(proto->num), dots->count((proto->num + 1) % 2));
			msgret = KMessageBox::questionYesNo(this, str, i18n("Game over"));
			if(msgret == KMessageBox::Yes)
			{
				gameinit();
			}
			else
			{
				proto->state = proto->statedone;
				exit(-1);
			}
			break;
		case KDotsProto::sndsync:
			gamesync();
			break;
		case KDotsProto::reqoptions:
			slotOptions();
			break;
		default:
			proto->state = proto->statedone;
			KMessageBox::error(this, i18n("A protocol error has been detected. Aborting the game."), i18n("Protocol error"));
			exit(-1);
	}
}

void KDots::gameinit()
{
	proto->init();
}

void KDots::gamesync()
{
	char move;
	int score;
	char dot;

	move = proto->getSyncMove();
	proto->turn = move;
	score = proto->getSyncScore();
	score = proto->getSyncScore();

	for(int i = 0; i < proto->width; i++)
		for(int j = 0; j < proto->height - 1; j++)
		{
			dot = proto->getSyncMove();
			dots->setBorderValue(i, j, QDots::down, 0, dot); 
		}

	for(int i = 0; i < proto->width - 1; i++)
		for(int j = 0; j < proto->height; j++)
		{
			dot = proto->getSyncMove();
			dots->setBorderValue(i, j, QDots::right, 0, dot);
		}

	for(int i = 0; i < proto->width - 1; i++)
		for(int j = 0; j < proto->height - 1; j++)
		{
			dot = proto->getSyncMove();
			dots->setOwnership(i, j, dot);
		}

	dots->repaint();

	emit signalStatus(i18n("Syncing successful"));
}

void KDots::input()
{
/*
	QSocketNotifier *sn;

	sn = new QSocketNotifier(proto->fd, QSocketNotifier::Read, this);
	connect(sn, SIGNAL(activated(int)), SLOT(slotInput()));
*/
	// FIXME: setup kggzmod signals here
}

/*void KDots::slotDispatch()
{
	proto->dispatch();
}*/

