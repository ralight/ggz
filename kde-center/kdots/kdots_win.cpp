/////////////////////////////////////////////////////////////////////
//
// KDots: Connect the Dots game for KDE 4
// http://www.ggzgamingzone.org/gameclients/kdots/
//
// Copyright (C) 2001 - 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under the conditions of the GNU GPL, see COPYING
//
/////////////////////////////////////////////////////////////////////

#include "kdots_win.h"
#include "kdots.h"
#include "kdots_about.h"
#include "kdots_help.h"
#include "kdots_replay.h"
#include "kdots_proto.h"

#include <kggzgames/kggzseatsdialog.h>

#include <klocale.h>
#include <kmenu.h>
#include <kstatusbar.h>

KDotsWin::KDotsWin(bool ggzmode)
: KMainWindow()
{
	KMenu *menu_game, *menu_help;

	kdots_help = NULL;
	kdots_about = NULL;
	kdots_replay = NULL;

	m_color = new QWidget(statusBar());
	statusBar()->insertItem(i18n("Launching KDots..."), 1, 1);
	statusBar()->addWidget(m_color, 1);

	menu_game = new KMenu(this);
	menu_game->setTitle(i18n("Game"));
	action_sync = menu_game->addAction(i18n("Synchronize"));
	action_seats = menu_game->addAction(i18n("Seats..."));
	menu_game->addSeparator();
	action_replay = menu_game->addAction(i18n("Replay games..."));
	menu_game->addSeparator();
	action_quit = menu_game->addAction(i18n("Quit"));

	menu_help = new KMenu(this);
	menu_help->setTitle(i18n("Help"));
	action_help = menu_help->addAction(i18n("Game Help"));
	menu_help->addSeparator();
	action_about = menu_help->addAction(i18n("About"));
	
	menuBar()->addMenu(menu_game);
	menuBar()->addMenu(menu_help);

	connect(menu_game, SIGNAL(triggered(QAction*)), SLOT(slotMenu(QAction*)));
	connect(menu_help, SIGNAL(triggered(QAction*)), SLOT(slotMenu(QAction*)));

	m_dots = new KDots(ggzmode); 
	setCentralWidget(m_dots);

	connect(m_dots, SIGNAL(signalStatus(QString)), SLOT(slotStatus(QString)));
	connect(m_dots, SIGNAL(signalColor(const QColor&)), SLOT(slotColor(const QColor&)));

	if(ggzmode)
	{
		slotStatus(i18n("Waiting for opponent..."));
	}
	else
	{
		slotStatus(i18n("Replay-only mode"));
		action_sync->setEnabled(false);
		action_seats->setEnabled(false);
	}

	setFixedSize(400, 400);
	setCaption(i18n("Connect The Dots for KDE"));
	show();
}

KDotsWin::~KDotsWin()
{
}

void KDotsWin::slotMenu(QAction *action)
{
	if(action == action_about)
	{
		if(!kdots_about) kdots_about = new KDotsAbout();
		kdots_about->show();
	}
	else if(action == action_help)
	{
		if(!kdots_help) kdots_help = new KDotsHelp();
		kdots_help->show();
	}
	else if(action == action_quit)
	{
		close();
	}
	else if(action == action_sync)
	{
		m_dots->slotSync();
	}
	else if(action == action_replay)
	{
		if(!kdots_replay) kdots_replay = new KDotsReplay();
		kdots_replay->show();
	}
	else if(action == action_seats)
	{
		/*KGGZSeatsDialog *seats = new KGGZSeatsDialog();
		seats->setMod(m_dots->getProto()->getMod());*/
		// FIXME: disabled until kdots uses kggzmod
	}
}

void KDotsWin::slotStatus(QString message)
{
	statusBar()->changeItem(message, 1);
}

void KDotsWin::slotColor(const QColor& color)
{
	QPalette palette = m_color->palette();
	palette.setBrush(QPalette::Window, color);
	m_color->setPalette(palette);
}

