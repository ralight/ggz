///////////////////////////////////////////////////////////////////
// KCC
// Copyright (C) 2003, 2004 Josef Spillner, josef@ggzgamingzone.org
// Published under GNU GPL conditions
///////////////////////////////////////////////////////////////////

// KCC includes
#include "kccwin.h"

// KDE includes
#include <kpopupmenu.h>
#include <kmenubar.h>
#include <klocale.h>
#include <kstatusbar.h>
#include <kconfig.h>
#include <ksimpleconfig.h>
#include <kapplication.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kdebug.h>

// Qt includes
#include <qdir.h>
#include <qstringlist.h>

// Constructor
KCCWin::KCCWin(QWidget *parent, const char *name)
: KMainWindow(parent, name)
{
	KStandardDirs d;
	QString icontheme;

	m_kcc = new KCC(this);
	setCentralWidget(m_kcc);

	m_networked = false;

#if (((KDE_VERSION_MAJOR == 3) && (KDE_VERSION_MINOR >= 1)) || (KDE_VERSION_MAJOR > 3))
	icontheme = "crystalsvg";
#else
	icontheme = "hicolor";
#endif

	QString pixexit = d.findResource("icon", icontheme + "/16x16/actions/exit.png");
	QString pixsync = d.findResource("icon", icontheme + "/16x16/actions/reload.png");
	QString pixscore = d.findResource("icon", icontheme + "/16x16/actions/history.png");

	mgame = new KPopupMenu(this);
	mgame->insertItem(QIconSet(QPixmap(pixsync)), i18n("Synchronize"), menusync);
	mgame->insertItem(QIconSet(QPixmap(pixscore)), i18n("View score"), menuscore);
	mgame->insertSeparator();
	mgame->insertItem(QIconSet(QPixmap(pixexit)), i18n("Quit"), menuquit);

	//mtheme = new KPopupMenu(this);
	//mtheme->insertItem(i18n("KDE/Gnome"), menuthemenew);
	//mtheme->insertItem(i18n("Tux/Kandalf (classic)"), menuthemeclassic);
	//mtheme->insertItem(i18n("Symbols"), menuthemesymbols);

	menuBar()->insertItem(i18n("Game"), mgame);
	//menuBar()->insertItem(i18n("Theme"), mtheme);
	menuBar()->insertItem(i18n("Help"), helpMenu());

	statusBar()->insertItem(i18n("Status"), 1, 1);
	statusBar()->insertItem(i18n("Game with the AI"), 2, 1);

	connect(m_kcc, SIGNAL(signalStatus(const QString &)), SLOT(slotStatus(const QString &)));
	connect(m_kcc, SIGNAL(signalScore(const QString &)), SLOT(slotScore(const QString &)));
	connect(m_kcc, SIGNAL(signalNetworkScore(int, int)), SLOT(slotNetworkScore(int, int)));
	connect(m_kcc, SIGNAL(signalGameOver()), SLOT(slotGameOver()));
	connect(mgame, SIGNAL(activated(int)), SLOT(slotMenu(int)));
	//connect(mtheme, SIGNAL(activated(int)), SLOT(slotMenu(int)));

	enableNetwork(false);

	setCaption("Chinese Checkers");
	resize(400, 450);
	show();
}

// Destructor
KCCWin::~KCCWin()
{
}

// Display the game status
void KCCWin::slotStatus(const QString &status)
{
	statusBar()->changeItem(status, 1);
}

// Display the game score
void KCCWin::slotScore(const QString &score)
{
	statusBar()->changeItem(score, 2);
}

// Return the game object
KCC *KCCWin::kcc()
{
	return m_kcc;
}

// Handle menu stuff
void KCCWin::slotMenu(int id)
{
	KConfig *conf;

	// Standard menu entries
	switch(id)
	{
		case menusync:
			m_kcc->sync();
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
void KCCWin::enableNetwork(bool enabled)
{
	mgame->setItemEnabled(menusync, enabled);
	m_networked = enabled;
}

// Display scores
void KCCWin::score()
{
	if(m_networked)
	{
		m_kcc->statistics();
		return;
	}

	KConfig *conf = kapp->config();
	conf->setGroup("Score");
	int ailost = conf->readNumEntry("ailost");
	int aiwon = conf->readNumEntry("aiwon");

	KMessageBox::information(this,
		i18n("You won %1 times and lost %2 times against the AI.").arg(ailost).arg(aiwon),
		i18n("KCC score"));
}

// Display network score
void KCCWin::slotNetworkScore(int wins, int losses)
{
	KMessageBox::information(this,
		i18n("Human players have been beaten %1 times by you, you lost %2 times."
			).arg(wins).arg(losses),
		i18n("KCC network score"));
}

// Game is over
void KCCWin::slotGameOver()
{
	mgame->setItemEnabled(menusync, false);
	if(m_networked) mgame->setItemEnabled(menuscore, false);
}

