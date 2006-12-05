//////////////////////////////////////////////////////////////////////
// KConnectX
// Copyright (C) 2006 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions
//////////////////////////////////////////////////////////////////////

// KConnectX includes
#include "win.h"

// GGZ-KDE-Games includes
#include "kggzseatsdialog.h"

// KDE includes
#include <kpopupmenu.h>
#include <kmenubar.h>
#include <klocale.h>
#include <kstatusbar.h>
#include <kapplication.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kiconloader.h>

// Qt includes
#include <qdir.h>
#include <qstringlist.h>

// Constructor
Win::Win(QWidget *parent, const char *name)
: KMainWindow(parent, name)
{
	m_game = new KConnectX(this);
	setCentralWidget(m_game);

	m_menugame = new KPopupMenu(this);
	m_menugame->insertItem(KGlobal::iconLoader()->loadIcon("exit", KIcon::Small),
		i18n("Quit"),
		menuquit);

	m_menuggz = new KPopupMenu(this);
	m_menuggz->insertItem(KGlobal::iconLoader()->loadIcon("ggz", KIcon::Small),
		i18n("Seats && Spectators"),
		menuplayers);
	m_menuggz->insertItem(KGlobal::iconLoader()->loadIcon("history", KIcon::Small),
		i18n("View score"),
		menuscore);

	menuBar()->insertItem(i18n("Game"), m_menugame);
	menuBar()->insertItem(i18n("GGZ"), m_menuggz);
	menuBar()->insertItem(i18n("Help"), helpMenu());

	statusBar()->insertItem(i18n("Status"), statusgame);

	connect(m_game,
		SIGNAL(signalStatus(const QString &)),
		SLOT(slotStatus(const QString &)));
	connect(m_game,
		SIGNAL(signalNetworkScore(int, int, int)),
		SLOT(slotNetworkScore(int, int, int)));
	connect(m_game,
		SIGNAL(signalGameOver()),
		SLOT(slotGameOver()));

	connect(m_menugame,
		SIGNAL(activated(int)),
		SLOT(slotMenu(int)));
	connect(m_menuggz,
		SIGNAL(activated(int)),
		SLOT(slotMenu(int)));

	setCaption(i18n("KConnectX Test Client"));
	resize(250, 250);
	show();
}

// Destructor
Win::~Win()
{
}

// Display the game status
void Win::slotStatus(const QString &status)
{
	statusBar()->changeItem(status, statusgame);
}

// Return the game object
KConnectX *Win::game() const
{
	return m_game;
}

// Handle menu stuff
void Win::slotMenu(int id)
{
	QDir d;
	KGGZSeatsDialog *seats;

	// Standard menu entries
	switch(id)
	{
		case menuscore:
			score();
			break;
		case menuplayers:
			seats = new KGGZSeatsDialog();
			seats->setMod(m_game->proto()->mod());
			break;
		case menuquit:
			close();
			break;
	}
}

// Display scores
void Win::score()
{
	//m_game->statistics();
}

// Display network score
void Win::slotNetworkScore(int wins, int losses, int ties)
{
	QString comment = "";
	if(!(wins + losses + ties))
		comment = i18n("Of course, because you didn't play yet.");
	else if(losses > wins * 2)
		comment = i18n("You are so bad.");
	else if(wins > losses * 2)
		comment = i18n("You're a TicTacTux expert!");

	KMessageBox::information(this,
		i18n("Human players have been beaten %1 times by you, you lost %2 times. "
			"%3 ties were achieved. "
			"%4").arg(wins).arg(losses).arg(ties).arg(comment),
		i18n("KTicTacTux network score"));
}

// Game is over
void Win::slotGameOver()
{
	m_menugame->setItemEnabled(menuscore, false);
	m_menugame->setItemEnabled(menuplayers, false);
}

