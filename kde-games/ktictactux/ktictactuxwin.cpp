//////////////////////////////////////////////////////////////////////
// KTicTacTux
// Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
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
	m_tux = new KTicTacTux(this);
	setCentralWidget(m_tux);

	mgame = new KPopupMenu(this);
	mgame->insertItem(i18n("Synchronize"), menusync);
	mgame->insertItem(i18n("Quit"), menuquit);

	mtheme = new KPopupMenu(this);
	mtheme->insertItem(i18n("KDE/Gnome"), menuthemenew);
	mtheme->insertItem(i18n("Tux/Kandalf (classic)"), menuthemeclassic);
	mtheme->insertItem(i18n("Symbols"), menuthemesymbols);

	menuBar()->insertItem(i18n("Game"), mgame);
	menuBar()->insertItem(i18n("Theme"), mtheme);
	menuBar()->insertItem(i18n("Help"), helpMenu());

	statusBar()->insertItem(i18n("Status"), 1, 1);
	statusBar()->insertItem(i18n("Game with the AI"), 2, 1);

	connect(m_tux, SIGNAL(signalStatus(const QString &)), SLOT(slotStatus(const QString &)));
	connect(m_tux, SIGNAL(signalScore(const QString &)), SLOT(slotScore(const QString &)));
	connect(mgame, SIGNAL(activated(int)), SLOT(slotMenu(int)));
	connect(mtheme, SIGNAL(activated(int)), SLOT(slotMenu(int)));

	enableNetwork(false);
	slotMenu(menuthemenew);

	setCaption("KTicTacTux");
	resize(250, 250);
	show();
}

// Destructor
KTicTacTuxWin::~KTicTacTuxWin()
{
}

// Display the game status
void KTicTacTuxWin::slotStatus(const QString &status)
{
	statusBar()->changeItem(status, 1);
}

// Display the game score
void KTicTacTuxWin::slotScore(const QString &score)
{
	statusBar()->changeItem(score, 2);
}

// Return the game object
KTicTacTux *KTicTacTuxWin::tux()
{
	return m_tux;
}

// Handle menu stuff
void KTicTacTuxWin::slotMenu(int id)
{
	switch(id)
	{
		case menuthemenew:
			mtheme->setItemChecked(menuthemenew, true);
			mtheme->setItemChecked(menuthemeclassic, false);
			mtheme->setItemChecked(menuthemesymbols, false);
			m_tux->setTheme("kde.png", "gnome.png");
			break;
		case menuthemeclassic:
			mtheme->setItemChecked(menuthemenew, false);
			mtheme->setItemChecked(menuthemeclassic, true);
			mtheme->setItemChecked(menuthemesymbols, false);
			m_tux->setTheme("tux.png", "merlin.png");
			break;
		case menuthemesymbols:
			mtheme->setItemChecked(menuthemenew, false);
			mtheme->setItemChecked(menuthemeclassic, false);
			mtheme->setItemChecked(menuthemesymbols, true);
			m_tux->setTheme("cross.png", "circle.png");
			break;
		case menusync:
			m_tux->sync();
			break;
		case menuquit:
			close();
			break;
	}
}

/// Enable network functionality
void KTicTacTuxWin::enableNetwork(bool enabled)
{
	mgame->setItemEnabled(menusync, enabled);
}

