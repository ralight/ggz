//////////////////////////////////////////////////////////////////////
// KTicTacTux
// Copyright (C) 2001 Josef Spillner, dr_maux@users.sourceforge.net
// Published under GNU GPL conditions
//////////////////////////////////////////////////////////////////////

// KTicTacTux includes
#include "ktictactuxwin.h"

// KDE includes
#include <kpopupmenu.h>
#include <kmenubar.h>
#include <klocale.h>

// Constructor
KTicTacTuxWin::KTicTacTuxWin(QWidget *parent, const char *name)
: KMainWindow(parent, name)
{
	KPopupMenu *mgame;

	m_tux = new KTicTacTux(this);
	setCentralWidget(m_tux);

	mgame = new KPopupMenu(this);
	mgame->insertItem(i18n("Synchronize"), menusync);
	mgame->insertItem(i18n("Quit"), menuquit);

	menuBar()->insertItem(i18n("Game"), mgame);
	menuBar()->insertItem(i18n("Help"), helpMenu());

	statusBar()->insertItem("Status", 1);

	connect(m_tux, SIGNAL(signalStatus(QString)), SLOT(slotStatus(QString)));
	connect(mgame, SIGNAL(activated(int)), SLOT(slotMenu(int)));

	setCaption("KTicTacTux");
	show();
}

// Destructor
KTicTacTuxWin::~KTicTacTuxWin()
{
}

// Display the game status
void KTicTacTuxWin::slotStatus(QString status)
{
	statusBar()->changeItem(status, 1);
}

KTicTacTux *KTicTacTuxWin::tux()
{
	return m_tux;
}

void KTicTacTuxWin::slotMenu(int id)
{
	switch(id)
	{
		case menusync:
			break;
		case menuquit:
			close();
			break;
	}
}

