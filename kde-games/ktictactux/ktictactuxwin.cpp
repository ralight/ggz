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
#include <kstatusbar.h>
#include <kconfig.h>
#include <kapplication.h>
#include <kmessagebox.h>

// Constructor
KTicTacTuxWin::KTicTacTuxWin(QWidget *parent, const char *name)
: KMainWindow(parent, name)
{
	m_tux = new KTicTacTux(this);
	setCentralWidget(m_tux);

	m_networked = false;

	mgame = new KPopupMenu(this);
	mgame->insertItem(i18n("Synchronize"), menusync);
	mgame->insertItem(i18n("View score"), menuscore);
	mgame->insertSeparator();
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
	connect(m_tux, SIGNAL(signalNetworkScore(int, int)), SLOT(slotNetworkScore(int, int)));
	connect(mgame, SIGNAL(activated(int)), SLOT(slotMenu(int)));
	connect(mtheme, SIGNAL(activated(int)), SLOT(slotMenu(int)));

	enableNetwork(false);

	KConfig *conf = kapp->config();
	conf->setGroup("Settings");
	int theme = conf->readNumEntry("theme");
	if(theme) changeTheme(theme);
	else changeTheme(menuthemenew);

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

// Change the theme
void KTicTacTuxWin::changeTheme(int theme)
{
	switch(theme)
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
	}
}

// Handle menu stuff
void KTicTacTuxWin::slotMenu(int id)
{
	KConfig *conf;

	switch(id)
	{
		case menuthemenew:
		case menuthemeclassic:
		case menuthemesymbols:
			changeTheme(id);
			conf = kapp->config();
			conf->setGroup("Settings");
			conf->writeEntry("theme", id);
			break;
		case menusync:
			m_tux->sync();
			break;
		case menuscore:
			score();
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
	m_networked = enabled;
}

// Display scores
void KTicTacTuxWin::score()
{
	if(m_networked)
	{
		m_tux->statistics();
		return;
	}

	KConfig *conf = kapp->config();
	conf->setGroup("Score");
	int ailost = conf->readNumEntry("ailost");
	int aiwon = conf->readNumEntry("aiwon");

	QString comment = "";
	if(!(ailost + aiwon))
		comment = i18n("Of course, because you didn't play yet.");
	else if(aiwon > ailost * 2)
		comment = i18n("You are so bad.");
	else if(aiwon * 2 < ailost)
		comment = i18n("You're a TicTacTux expert!");

	KMessageBox::information(this,
		i18n("You won %1 times and lost %2 times against the AI. "
			"%3").arg(ailost).arg(aiwon).arg(comment),
		i18n("KTicTacTux score"));
}

// Display network score
void KTicTacTuxWin::slotNetworkScore(int wins, int losses)
{
	QString comment = "";
	if(!(wins + losses))
		comment = i18n("Of course, because you didn't play yet.");
	else if(losses > wins * 2)
		comment = i18n("You are so bad.");
	else if(wins > losses * 2)
		comment = i18n("You're a TicTacTux expert!");

	KMessageBox::information(this,
		i18n("Human players have been beaten %1 times by you, you lost %2 times. "
			"%3").arg(wins).arg(losses).arg(comment),
		i18n("KTicTacTux network score"));
}

