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
#include <ksimpleconfig.h>
#include <kapplication.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kdebug.h>

// Qt includes
#include <qdir.h>
#include <qstringlist.h>

// Constructor
KTicTacTuxWin::KTicTacTuxWin(QWidget *parent, const char *name)
: KMainWindow(parent, name)
{
	KStandardDirs d;
	QString icontheme;

	m_tux = new KTicTacTux(this);
	setCentralWidget(m_tux);

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

	mtheme = new KPopupMenu(this);
	//mtheme->insertItem(i18n("KDE/Gnome"), menuthemenew);
	//mtheme->insertItem(i18n("Tux/Kandalf (classic)"), menuthemeclassic);
	//mtheme->insertItem(i18n("Symbols"), menuthemesymbols);

	menuBar()->insertItem(i18n("Game"), mgame);
	menuBar()->insertItem(i18n("Theme"), mtheme);
	menuBar()->insertItem(i18n("Help"), helpMenu());

	statusBar()->insertItem(i18n("Status"), 1, 1);
	statusBar()->insertItem(i18n("Game with the AI"), 2, 1);

	connect(m_tux, SIGNAL(signalStatus(const QString &)), SLOT(slotStatus(const QString &)));
	connect(m_tux, SIGNAL(signalScore(const QString &)), SLOT(slotScore(const QString &)));
	connect(m_tux, SIGNAL(signalNetworkScore(int, int, int)), SLOT(slotNetworkScore(int, int, int)));
	connect(m_tux, SIGNAL(signalGameOver()), SLOT(slotGameOver()));
	connect(mgame, SIGNAL(activated(int)), SLOT(slotMenu(int)));
	connect(mtheme, SIGNAL(activated(int)), SLOT(slotMenu(int)));

	enableNetwork(false);

	loadThemes();

	KConfig *conf = kapp->config();
	conf->setGroup("Settings");
	QString theme = conf->readEntry("theme");
	if(theme) changeTheme(theme);
	else changeTheme(QString::null);

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
void KTicTacTuxWin::changeTheme(QString theme)
{
	KStandardDirs d;
	int id;

	// fall back to default if no theme is set or theme is no more installed
	if((theme == QString::null) || m_player1[theme] == QString::null)
	{
		theme = d.findResource("data", "ktictactux/classic");
	}

	// try to enable the corresponding menu item
	for(unsigned int i = 0; i < mtheme->count(); i++)
	{
		id = mtheme->idAt(i);
		if(m_themes[mtheme->text(id)] == theme) mtheme->setItemChecked(id, true);
		else mtheme->setItemChecked(id, false);
	}

	// Update pixmaps
	kdDebug() << "Player 1 has " << m_player1[theme] << ", theme " << theme << endl;
	m_tux->setTheme(m_player1[theme], m_player2[theme]);
}

// Handle menu stuff
void KTicTacTuxWin::slotMenu(int id)
{
	KConfig *conf;

	// Standard menu entries
	switch(id)
	{
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

	// Dynamic theme menu entries
	if(id >= menuthemes)
	{
		changeTheme(m_themes[mtheme->text(id)]);
		conf = kapp->config();
		conf->setGroup("Settings");
		conf->writeEntry("theme", m_themes[mtheme->text(id)]);
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
	int aitied = conf->readNumEntry("aitied");

	QString comment = "";
	if(!(ailost + aiwon))
		comment = i18n("Of course, because you didn't play yet.");
	else if(aiwon > ailost * 2)
		comment = i18n("You are so bad.");
	else if(aiwon * 2 < ailost)
		comment = i18n("You're a TicTacTux expert!");

	KMessageBox::information(this,
		i18n("You won %1 times, lost %2 times and tied %3 times against the AI. "
			"%4").arg(ailost).arg(aiwon).arg(aitied).arg(comment),
		i18n("KTicTacTux score"));
}

// Display network score
void KTicTacTuxWin::slotNetworkScore(int wins, int losses, int ties)
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
void KTicTacTuxWin::slotGameOver()
{
	mgame->setItemEnabled(menusync, false);
	if(m_networked) mgame->setItemEnabled(menuscore, false);
}

// Read in all themes
void KTicTacTuxWin::loadThemes()
{
	KStandardDirs d;
	QString name, player1, player2;
	QString file;
	int index = menuthemes;
	QString icontheme;

#if (((KDE_VERSION_MAJOR == 3) && (KDE_VERSION_MINOR >= 1)) || (KDE_VERSION_MAJOR > 3))
	icontheme = "crystalsvg";
#else
	icontheme = "hicolor";
#endif

	// Recursively scan all data directories
	kdDebug() << "loadThemes" << endl;
	QStringList list(d.findDirs("data", "ktictactux"));
	for(QStringList::iterator it = list.begin(); it != list.end(); it++)
	{
		QDir dir((*it));
		kdDebug() << "Scan dir: " << (*it) << endl;
		QStringList entries = dir.entryList(QDir::Files);
		for(QStringList::iterator it2 = entries.begin(); it2 != entries.end(); it2++)
		{
			if((*it2).right(4) == ".png") continue;
			file = (*it) + (*it2);
			kdDebug() << "Check file: " << file << endl;
			KSimpleConfig conf(file);
			if(conf.hasGroup("Description"))
			{
				conf.setGroup("Description");
				name = conf.readEntry("name");
				conf.setGroup("Pixmaps");
				player1 = conf.readEntry("player1");
				player2 = conf.readEntry("player2");

				m_themes[name] = file;
				m_player1[file] = (*it) + player1;
				m_player2[file] = (*it) + player2;

				QString pixtheme = d.findResource("icon", icontheme + "/16x16/actions/imagegallery.png");
				mtheme->insertItem(QIconSet(QPixmap(pixtheme)), name, index++);
			}
		}
	}

	if(index == menuthemes)
	{
		KMessageBox::error(this,
			i18n("No pixmap themes could be found"),
			i18n("Themes error"));
	}
}

