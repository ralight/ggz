///////////////////////////////////////////////////////////////
//
// KDots
// Connect the Dots game for KDE, using the Dots classes
// Copyright (C) 2001 Josef Spillner
// dr_maux@users.sourceforge.net
// The MindX Open Source Project
// http://mindx.sourceforge.net/games/kdots/
//
///////////////////////////////////////////////////////////////

#include "kdots.h"
#include "kdots_options.h"

#include <kmessagebox.h>
#include <klocale.h>

#include <qmenubar.h>
#include <qlayout.h>
#include <qsocketnotifier.h>

#include <iostream>
#include <stdlib.h>

#include "qdots.h"

#include <stdio.h>

KDotsOptions *kdots_options;
QDots *dots;

KDots::KDots(QWidget *parent, char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox;
	QSocketNotifier *sn;

	dots = new QDots(this, "qdots");

	vbox = new QVBoxLayout(this, 5);
	vbox->add(dots);

	kdots_options = NULL;

	connect(dots, SIGNAL(signalTurn(int, int, int)), SLOT(slotTurn(int, int, int)));

	setCaption("KDE Dots");
	//dots->setFixedSize(400, 400);
	setFixedSize(400, 400);
	show();

	proto = new KDotsProto();
	proto->connect();
	proto->turn = -1;
	proto->num = -2;

	sn = new QSocketNotifier(proto->fd, QSocketNotifier::Read, this);
	connect(sn, SIGNAL(activated(int)), SLOT(slotInput()));
}

KDots::~KDots()
{
	delete proto;
}

void KDots::slotOptions()
{
	emit signalStatus(i18n("Requesting game options"));

	if(!kdots_options)
	{
		kdots_options = new KDotsOptions(NULL, "kdots_options");
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
	if(proto->turn == -1)
	{
		KMessageBox::information(this, i18n("No game running yet!"), i18n("Error"));
		return;
	}
	es_write_int(proto->fd, proto->reqsync);
}

void KDots::slotInput()
{
	int op;
	int ret;
	QString str;
	char status;

	if(proto->state == proto->statedone) return;

	emit signalStatus(i18n("Receiving data"));

	es_read_int(proto->fd, &op);

	switch(op)
	{
		case proto->msgseat:
printf("##### msgseat\n");
			proto->getSeat();
			break;
		case proto->msgplayers:
printf("##### msgplayers\n");
			proto->getPlayers();
			if(proto->state != proto->statechoose) proto->state = proto->statewait;
			break;
		case proto->msgoptions:
printf("##### msgoptions\n");
			proto->getOptions();
			//slotStart(proto->width, proto->height);
			dots->resizeBoard(proto->width - 1, proto->height - 1);
			dots->refreshBoard();
			break;
		case proto->reqmove:
printf("##### reqmove\n");
			emit signalStatus(i18n("Your turn."));
			proto->state = proto->statemove;
			proto->turn = proto->num;
			break;
		case proto->msgmoveh:
printf("##### msgmoveh\n");
			proto->getOppMove(proto->sndmoveh);
			dots->setBorderValue(proto->movex, proto->movey, QDots::right, proto->turn, 1);
			dots->repaint();
			break;
		case proto->msgmovev:
printf("##### msgmovev\n");
			proto->getOppMove(proto->sndmovev);
			dots->setBorderValue(proto->movex, proto->movey, QDots::down, proto->turn, 1);
			dots->repaint();
			break;
		case proto->rspmove:
printf("##### rspmove\n");
			if(proto->getMove() != -1)
			{
				dots->setBorderValue(proto->m_lastx, proto->m_lasty, proto->m_lastdir, proto->turn, 1);
				dots->repaint();
				proto->turn = (proto->num + 1) % 2;
			}
			else
			{
				emit signalStatus(i18n("Invalid move, please try again!"));
			}
			break;
		case proto->msggameover:
printf("##### msggameover\n");
			es_read_char(proto->fd, &status);
			if(status == -1) exit(-1);
			proto->state = proto->statechoose;
			proto->turn = -1;
			emit signalStatus(i18n("Game over!"));
			str.sprintf(i18n("The game is over.\nYou reached %i points, your opponent %i.\n\nPlay another game?"),
				dots->count(proto->num), dots->count((proto->num + 1) % 2));
			ret = KMessageBox::questionYesNo(this, str, i18n("Game over"));
			if(ret == KMessageBox::Yes) gameinit();
			else
			{
				proto->state = proto->statedone;
				exit(-1);
			}
			break;
		case proto->sndsync:
printf("##### sndsync\n");
			gamesync();
			break;
		case proto->reqoptions:
printf("##### reqoptions\n");
			slotOptions();
			break;
		default:
printf("##### unknown opcode -> %i\n", op);
	}
}

void KDots::gameinit()
{
	proto->state = proto->stateinit;
	es_write_int(proto->fd, proto->reqnewgame);
}

void KDots::gamesync()
{
	char move;
	int score;
	char dot;

	es_read_char(proto->fd, &move);
	proto->turn = move;
	es_read_int(proto->fd, &score);
	es_read_int(proto->fd, &score);

	for(int i = 0; i < proto->width; i++)
		for(int j = 0; j < proto->height - 1; j++)
			es_read_char(proto->fd, &dot);

	for(int i = 0; i < proto->width - 1; i++)
		for(int j = 0; j < proto->height; j++)
			es_read_char(proto->fd, &dot);

	for(int i = 0; i < proto->width - 1; i++)
		for(int j = 0; j < proto->height - 1; j++)
			es_read_char(proto->fd, &dot);
}

