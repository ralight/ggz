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

#include <iostream>
#include <stdlib.h>

#include "qdots.h"

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

	dots = new QDots(this, "qdots");

	vbox = new QVBoxLayout(this, 5);
	vbox->add(dots);

	m_running = 0;
	kdots_help = NULL;
	kdots_about = NULL;
	kdots_options = NULL;

	connect(dots, SIGNAL(signalTurn(int, int, int)), SLOT(slotTurn(int, int, int)));

	setCaption("KDE Dots");
	dots->resize(390, 390);
	resize(400, 400);
	show();
	slotOptions();
}

KDots::~KDots()
{
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
	dots->resizeBoard(horizontal, vertical);
	dots->refreshBoard();

	m_running = 1;
}

void KDots::slotTurn(int x, int y, int direction)
{
	cout << "*** SLOT TURN ***" << endl;
}
