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
#include "kdots_about.h"
#include "kdots_help.h"

#include <qmenubar.h>
#include <qlayout.h>
#include <qsocketnotifier.h>

#include <iostream>
#include <stdlib.h>

#include "qdots.h"

#include <stdio.h>

KDotsAbout *kdots_about;
KDotsOptions *kdots_options;
KDotsHelp *kdots_help;
QDots *dots;

KDots::KDots(QWidget *parent, char *name)
: QWidget(parent, name)
{
	QMenuBar *menubar;
	QPopupMenu *menu_game, *menu_help;
	QVBoxLayout *vbox;
	QSocketNotifier *sn;

cout << "fire up kdots" << endl;

	menu_game = new QPopupMenu();
	menu_game->insertItem("Quit", this, SLOT(slotQuit()));

	menu_help = new QPopupMenu();
	menu_help->insertItem("About", this, SLOT(slotAbout()));
	menu_help->insertSeparator();
	menu_help->insertItem("Help", this, SLOT(slotHelp()));

	menubar = new QMenuBar(this);
	menubar->insertItem("Game", menu_game);
	menubar->insertSeparator();
	menubar->insertItem("Help", menu_help);

cout << "create qdots" << endl;

	dots = new QDots(this, "qdots");

cout << "ready" << endl;

	vbox = new QVBoxLayout(this, 5);
	vbox->add(dots);

	m_running = 0;
	kdots_help = NULL;
	kdots_about = NULL;
	kdots_options = NULL;

	connect(dots, SIGNAL(signalTurn(int, int, int)), SLOT(slotTurn(int, int, int)));

cout << "kdots ready" << endl;

	setCaption("KDE Dots");
	dots->resize(390, 390);
	resize(400, 400);
	show();
	//slotOptions();

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

void KDots::slotAbout()
{
	if(!kdots_about) kdots_about = new KDotsAbout(NULL, "kdots_about");
}

void KDots::slotOptions()
{
	if(!kdots_options)
	{
		kdots_options = new KDotsOptions(NULL, "kdots_options");
		connect(kdots_options, SIGNAL(signalAccepted(int, int)), this, SLOT(slotStart(int, int)));
	}
}

void KDots::slotHelp()
{
	if(!kdots_help) kdots_help = new KDotsHelp(NULL, "kdots_help");
}

void KDots::slotQuit()
{
	close();
}

void KDots::slotStart(int horizontal, int vertical)
{
	/*
	dots->resizeBoard(horizontal, vertical);
	dots->refreshBoard();
	*/

	if(kdots_options) proto->sendOptions(horizontal, vertical);

	m_running = 1;
}

void KDots::slotTurn(int x, int y, int direction)
{
	int sdotx, sdoty;

	if(proto->turn != proto->num)
	{
		cout << "not your turn!" << endl;
		return;
	}

	sdotx = x;
	sdoty = y;

	if(direction == QDots::up) sdoty--;
	if(direction == QDots::left) sdotx--;

	cout << "*** SLOT TURN ***" << endl;
	cout << "Send to server: " << endl;
	if((direction == QDots::up) || (direction == QDots::down))
	{
		cout << "VERTICAL " << sdotx << ", " << sdoty << endl;
		proto->sendMove(sdotx, sdoty, proto->sndmovev);
	}
	else
	{
		cout << "HORIZONTAL " << sdotx << ", " << sdoty << endl;
		proto->sendMove(sdotx, sdoty, proto->sndmoveh);
	}
}

void KDots::slotInput()
{
	int op;

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
			dots->resizeBoard(proto->width, proto->height);
			dots->refreshBoard();
			break;
		case proto->reqmove:
printf("##### reqmove\n");
			proto->state = proto->statemove;
			proto->turn = proto->num;
			break;
		case proto->msgmoveh:
printf("##### msgmoveh\n");
			proto->getOppMove(proto->sndmoveh);
			dots->setBorderValue(proto->movex, proto->movey, proto->sndmoveh, proto->turn);
			break;
		case proto->msgmovev:
printf("##### msgmovev\n");
			proto->getOppMove(proto->sndmoveh);
			dots->setBorderValue(proto->movex, proto->movey, proto->sndmovev, proto->turn);
			break;
		case proto->rspmove:
printf("##### rspmove\n");
			proto->getMove();
			if(proto->m_lastx != -1)
			{
				printf("setBorderValue(%i, %i, %i, %i); ", proto->m_lasty, proto->m_lasty, proto->m_lastdir, proto->turn);
				dots->setBorderValue(proto->m_lastx, proto->m_lasty, proto->m_lastdir, proto->turn);
				proto->turn = (proto->num + 1) % 2;
			}
			else
			{
				printf("INVALID MOVE!!! %i/%i\n", proto->movex, proto->movey);
			}
			break;
		case proto->msggameover:
printf("##### msggameover\n");
			break;
		case proto->sndsync:
printf("##### sndsync\n");
			break;
		case proto->reqoptions:
printf("##### reqoptions\n");
			slotOptions();
			break;
	}
}

