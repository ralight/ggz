///////////////////////////////////////////////////////////////
//
// KDots
// Connect the Dots game for KDE, using the Dots classes
// Copyright (C) 2001, 2002 Josef Spillner
// dr_maux@users.sourceforge.net
// The MindX Open Source Project
// http://mindx.sourceforge.net/games/kdots/
//
///////////////////////////////////////////////////////////////

#include "kdots_win.h"
#include "kdots.h"
#include "kdots_about.h"
#include "kdots_help.h"
#include "kdots_replay.h"

#include <klocale.h>
#include <kpopupmenu.h>
#include <kstatusbar.h>

KDotsWin::KDotsWin(bool ggzmode)
: KMainWindow()
{
	KPopupMenu *menu_game, *menu_help;

	kdots_help = NULL;
	kdots_about = NULL;
	kdots_replay = NULL;

	m_color = new QWidget(statusBar());
	statusBar()->insertItem(i18n("Launching KDots..."), 1, 1);
	statusBar()->addWidget(m_color, 1, true);

	menu_game = new KPopupMenu(this);
	menu_game->insertItem(i18n("Synchronize"), menusync);
	menu_game->insertSeparator();
	menu_game->insertItem(i18n("Replay games..."), menureplay);
	menu_game->insertSeparator();
	menu_game->insertItem(i18n("Quit"), menuquit);

	menu_help = new KPopupMenu(this);
	menu_help->insertItem(i18n("Game Help"), menuhelp);
	menu_help->insertSeparator();
	menu_help->insertItem(i18n("About"), menuabout);
	
	menuBar()->insertItem(i18n("Game"), menu_game);
	menuBar()->insertItem(i18n("Help"), menu_help);

	connect(menu_game, SIGNAL(activated(int)), SLOT(slotMenu(int)));
	connect(menu_help, SIGNAL(activated(int)), SLOT(slotMenu(int)));

	m_dots = new KDots(ggzmode, this, "KDots"); 
	setCentralWidget(m_dots);

	connect(m_dots, SIGNAL(signalStatus(const char*)), SLOT(slotStatus(const char*)));
	connect(m_dots, SIGNAL(signalColor(const QColor&)), SLOT(slotColor(const QColor&)));

	if(ggzmode)
		slotStatus(i18n("Waiting for opponent..."));
	else
	{
		slotStatus(i18n("Replay-only mode"));
		menu_game->setItemEnabled(menusync, false);
	}

	setFixedSize(400, 400);
	setCaption(i18n("Connect The Dots for KDE"));
	show();
}

KDotsWin::~KDotsWin()
{
}

void KDotsWin::slotMenu(int id)
{
	switch(id)
	{
		case menuabout:
			if(!kdots_about) kdots_about = new KDotsAbout(NULL, "KDotsAbout");
			kdots_about->show();
			break;
		case menuhelp:
			if(!kdots_help) kdots_help = new KDotsHelp(NULL, "KDotsHelp");
			kdots_help->show();
			break;
		case menuquit:
			close();
			break;
		case menusync:
			m_dots->slotSync();
			break;
		case menureplay:
			if(!kdots_replay) kdots_replay = new KDotsReplay(NULL, "KDotsReplay");
			kdots_replay->show();
			break;
	}
}

void KDotsWin::slotStatus(const char *message)
{
	statusBar()->changeItem(message, 1);
}

void KDotsWin::slotColor(const QColor& color)
{
	m_color->setEraseColor(QColor(color));
}

