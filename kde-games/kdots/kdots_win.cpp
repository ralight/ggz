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

#include "kdots_win.h"

#include <klocale.h>
#include <kpopupmenu.h>

KDotsWin::KDotsWin(char *name = NULL)
: KTMainWindow(name)
{
	KMenuBar *menubar;
	KPopupMenu *menu_game, *menu_help;

	kdots_help = NULL;
	kdots_about = NULL;

	enableStatusBar();
	statusBar()->insertItem(i18n("Launching KDots..."), 1);

	menubar = new KMenuBar(this);

	menu_game = new KPopupMenu(this);
	menu_game->insertItem(i18n("Synchronize"), menusync);
	menu_game->insertSeparator();
	menu_game->insertItem(i18n("Quit"), menuquit);

	menu_help = new KPopupMenu(this);
	menu_help->insertItem(i18n("Game Help"), menuhelp);
	menu_help->insertSeparator();
	menu_help->insertItem(i18n("About"), menuabout);
	
	menubar->insertItem(i18n("Game"), menu_game);
	menubar->insertItem(i18n("Help"), menu_help);

	connect(menu_game, SIGNAL(activated(int)), SLOT(slotMenu(int)));
	connect(menu_help, SIGNAL(activated(int)), SLOT(slotMenu(int)));

	m_dots = new KDots(this, "KDots"); 
	setView(m_dots);

	connect(m_dots, SIGNAL(signalStatus(const char*)), SLOT(slotStatus(const char*)));

	slotStatus(i18n("Waiting for opponent..."));

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
	}
}

void KDotsWin::slotStatus(const char *message)
{
	statusBar()->changeItem(message, 1);
}

