//////////////////////////////////////////////////////////////////////
// KConnectX
// Copyright (C) 2006 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions
//////////////////////////////////////////////////////////////////////

// KConnectX includes
#include "win.h"

// GGZ-KDE-Games includes
#warning commented kggzseatsdialog until it will be ported
//#include "kggzseatsdialog.h"

// KDE includes
#include <kmenu.h>
#include <kmenubar.h>
#include <klocale.h>
#include <kstatusbar.h>
#include <kmessagebox.h>
#include <kicon.h>
#include <kaction.h>

// Constructor
Win::Win(QWidget *parent)
: KMainWindow(parent)
{
	m_game = new KConnectX(this);
	setCentralWidget(m_game);

	m_menugame = new KMenu(i18n("Game"), this);
	m_actexit = new KAction(KIcon("exit"), i18n("Quit"), actionCollection(), "act_exit");
	m_menugame->addAction( m_actexit );

	m_menuggz = new KMenu(i18n("GGZ"), this);
	m_actggz = new KAction(KIcon("ggz"),i18n("Seats && Spectators"), actionCollection(), "act_ggz");
	m_menuggz->addAction( m_actggz );

	m_acthistory = new KAction(KIcon("history"),i18n("View score"), actionCollection(), "act_history");
	m_menuggz->addAction( m_acthistory );

	menuBar()->addMenu(m_menugame);
	menuBar()->addMenu(m_menuggz);
	menuBar()->addMenu(helpMenu());

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
		SIGNAL(triggered(QAction*)),
		SLOT(slotMenu(QAction*)));
	connect(m_menuggz,
		SIGNAL(triggered(QAction*)),
		SLOT(slotMenu(QAction*)));

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
void Win::slotMenu(QAction* act)
{
#warning commented kggzseatsdialog until it will be ported
	//also commented if(act == m_actggz) below
        //uncomment it too after porting is done
        //
	//KGGZSeatsDialog *seats;

	// Standard menu entries
	if(act == m_acthistory)
	{
		score();
	}
	else if(act == m_actggz)
	{
		/* 
		seats = new KGGZSeatsDialog();
		seats->setMod(m_game->proto()->mod());
		 */
	}
	else if(act == m_actexit)
	{
		close();
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
	m_acthistory->setEnabled(false);
	m_actggz->setEnabled(false);
}

