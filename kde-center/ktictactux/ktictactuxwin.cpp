/////////////////////////////////////////////////////////////////////
//
// KTicTacTux: Tic-Tac-Toe game for KDE 4
// http://www.ggzgamingzone.org/gameclients/ktictactux/
//
// Copyright (C) 2001 - 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under the conditions of the GNU GPL, see COPYING
//
/////////////////////////////////////////////////////////////////////

// KTicTacTux includes
#include "ktictactuxwin.h"

// KDE includes
#include <kmenu.h>
#include <kmenubar.h>
#include <klocale.h>
#include <kstatusbar.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <kapplication.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <kiconloader.h>

#include <knewstuff2/engine.h>

#include <kggzlib/connectiondialog.h>
#include <kggzlib/kggzcorelayer.h>
#include <kggzlib/gamecoreclient.h>

#include <kggzcore/player.h>

// Qt includes
#include <qdir.h>
#include <qstringlist.h>
#include <kglobal.h>

// Constructor
KTicTacTuxWin::KTicTacTuxWin()
: KMainWindow()
{
	m_tux = new KTicTacTux();
	setCentralWidget(m_tux);

	m_core = NULL;
	m_gcc = NULL;
	m_networked = false;

	mgame = new KMenu(this);
	mgame->setTitle(i18n("Game"));
	action_connect = mgame->addAction(KIconLoader::global()->loadIcon("network-connect", KIconLoader::Small), i18n("Connect to GGZ Gaming Zone"));
	action_sync = mgame->addAction(KIconLoader::global()->loadIcon("view-refresh", KIconLoader::Small), i18n("Synchronize"));
	action_highscores = mgame->addAction(KIconLoader::global()->loadIcon("view-history", KIconLoader::Small), i18n("View highscores"));
	action_score = mgame->addAction(KIconLoader::global()->loadIcon("view-history", KIconLoader::Small), i18n("View score"));
	mgame->addSeparator();
	action_theme = mgame->addAction(KIconLoader::global()->loadIcon("get-hot-new-stuff", KIconLoader::Small), i18n("Get themes"));
	mgame->addSeparator();
	action_quit = mgame->addAction(KIconLoader::global()->loadIcon("application-exit", KIconLoader::Small), i18n("Quit"));

	mggz = new KMenu(this);
	mggz->setTitle(i18n("GGZ"));
	action_ggzplayers = mggz->addAction(KIconLoader::global()->loadIcon("ggz", KIconLoader::Small), i18n("Seats && Spectators"));
	action_ggzcontrol = mggz->addAction(KIconLoader::global()->loadIcon("ggz", KIconLoader::Small), i18n("Control panel"));

	action_ggzplayers->setCheckable(true);
	action_ggzcontrol->setCheckable(true);

	mtheme = new KMenu(this);
	mtheme->setTitle(i18n("Theme"));

	menuBar()->addMenu(mgame);
	menuBar()->addMenu(mggz);
	menuBar()->addMenu(mtheme);
	menuBar()->addMenu(helpMenu());

	statusBar()->insertItem(i18n("Status"), 1, 1);
	statusBar()->insertItem(i18n("Game with the AI"), 2, 1);

	connect(m_tux, SIGNAL(signalStatus(const QString &)), SLOT(slotStatus(const QString &)));
	connect(m_tux, SIGNAL(signalScore(const QString &)), SLOT(slotScore(const QString &)));
	connect(m_tux, SIGNAL(signalNetworkScore(int, int, int)), SLOT(slotNetworkScore(int, int, int)));
	connect(m_tux, SIGNAL(signalGameOver()), SLOT(slotGameOver()));
	connect(m_tux, SIGNAL(signalError()), SLOT(slotError()));

	connect(mgame, SIGNAL(triggered(QAction*)), SLOT(slotMenu(QAction*)));
	connect(mggz, SIGNAL(triggered(QAction*)), SLOT(slotMenu(QAction*)));
	connect(mtheme, SIGNAL(triggered(QAction*)), SLOT(slotMenu(QAction*)));

	loadThemes();

	KSharedConfig::Ptr conf = KGlobal::config();
	KConfigGroup cg = KConfigGroup(conf, "Settings");
	QString theme = cg.readEntry("theme");
	if(!theme.isEmpty())
		changeTheme(theme);
	else
		changeTheme(QString());

	setCaption(i18n("KTicTacTux"));
	resize(400, 350);
	show();
}

// Destructor
KTicTacTuxWin::~KTicTacTuxWin()
{
	if(action_ggzcontrol->isChecked())
		delete m_gcc;
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

// Change the theme, theme parameter points to file
void KTicTacTuxWin::changeTheme(QString theme)
{
	KStandardDirs *d = KGlobal::dirs();

	// fall back to default if no theme is set or theme is no more installed
	if((theme.isEmpty()) || m_player1[theme].isEmpty())
	{
		kDebug() << "Theme fallback triggered!";
		theme = d->findResource("data", "ktictactux/classic");
	}

	// try to enable the corresponding menu item
	for(int i = 0; i < m_themeactions.count(); i++)
	{
		QAction *action = m_themeactions.at(i);
		if(m_themes[m_themenames[i]] == theme)
			action->setChecked(true);
		else
			action->setChecked(false);
	}

	// Update pixmaps
	kDebug() << "Player 1 has " << m_player1[theme] << ", theme " << theme << endl;
	m_tux->setTheme(m_player1[theme], m_player2[theme]);
}

// Handle menu stuff
void KTicTacTuxWin::slotMenu(QAction *action)
{
	QDir d;

	// Standard menu entries
	if(action == action_connect)
	{
		connectcore();
	}
	else if(action == action_sync)
	{
		m_tux->sync();
	}
	else if(action == action_score)
	{
		score();
	}
	else if(action == action_highscores)
	{
		m_tux->highscores();
	}
	else if(action == action_theme)
	{
		d.mkpath(QDir::home().path() + "/.ggz/games/ktictactux");
		KNS::Entry::List entries = KNS::Engine::download();
		for(int i = 0; i < entries.size(); i++)
		{
			KNS::Entry *entry = entries.at(i);
			QStringList files = entry->installedFiles();
			for(int j = 0; j < files.size(); j++)
			{
				QString file = files.at(j);
				QDir dir(file);
				if(dir.exists())
				{
					kDebug() << "KNS theme addition:" << file;
					scanDir(dir, false);
				}
			}
		}
	}
	else if(action == action_ggzplayers)
	{
		m_tux->seats();
		// FIXME: not yet well integrated with checkable actions
	}
	else if(action == action_ggzcontrol)
	{
		if(action_ggzcontrol->isChecked())
		{
			m_gcc = new GameCoreClient(m_core, NULL);
			m_gcc->setAttribute(Qt::WA_DeleteOnClose);
			connect(m_gcc, SIGNAL(destroyed()), action, SLOT(toggle()));
		}
		else
		{
			// FIXME: this is done because the destruction of m_gcc reverts it again
			action_ggzcontrol->setChecked(true);
			delete m_gcc;
			m_gcc = NULL;
		}
	}
	else if(action == action_quit)
	{
		close();
	}
	else
	{
		for(int i = 0; i < m_themeactions.count(); i++)
		{
			QAction *action2 = m_themeactions.at(i);
			if(action == action2)
			{
				QString theme = m_themes[m_themenames[i]];
				changeTheme(theme);
				KSharedConfig::Ptr conf = KGlobal::config();
				KConfigGroup cg = KConfigGroup(conf, "Settings");
				cg.writeEntry("theme", theme);
				conf->sync();
			}
		}
	}
}

/// Enable network functionality
void KTicTacTuxWin::enableNetwork(bool enabled)
{
	m_networked = enabled;

	action_sync->setEnabled(enabled);
	action_highscores->setEnabled(enabled);

	action_connect->setEnabled(!enabled);

	mggz->setEnabled(enabled);

	if(enabled)
	{
		m_tux->setOpponent(PLAYER_NETWORK);
	}

	m_tux->init();
}

// Display scores
void KTicTacTuxWin::score()
{
	if(m_networked)
	{
		m_tux->statistics();
		return;
	}

	KSharedConfig::Ptr conf = KGlobal::config();
	KConfigGroup cg = KConfigGroup(conf, "Score");
	int ailost = cg.readEntry("ailost").toInt();
	int aiwon = cg.readEntry("aiwon").toInt();
	int aitied = cg.readEntry("aitied").toInt();

	QString comment = "";
	if(!(ailost + aiwon))
		comment = i18n("Of course, because you didn't play yet.");
	else if(aiwon > ailost * 2)
		comment = i18n("You are so bad.");
	else if(aiwon * 2 < ailost)
		comment = i18n("You're a TicTacTux expert!");

	KMessageBox::information(this,
		i18n("You won %1 times, lost %2 times and tied %3 times against the AI. "
			"%4", ailost, aiwon, aitied, comment),
		i18n("KTicTacTux score"));
}

// Display network score
void KTicTacTuxWin::slotNetworkScore(int wins, int losses, int ties)
{
	QString comment;

	if(wins == -1)
	{
		comment = i18n("Playing as a guest doesn't allow for scores.");

		KMessageBox::sorry(this,
			comment,
			i18n("KTicTacTux network score"));
	}
	else
	{
		if(!(wins + losses + ties))
			comment = i18n("Of course, because you didn't play yet.");
		else if(losses > wins * 2)
			comment = i18n("You are so bad.");
		else if(wins > losses * 2)
			comment = i18n("You're a TicTacTux expert!");

		KMessageBox::information(this,
			i18n("Human players have been beaten %1 times by you; you lost %2 times. "
				"%3 ties were achieved. "
				"%4", wins, losses, ties, comment),
			i18n("KTicTacTux network score"));
	}
}

// Game is over
void KTicTacTuxWin::slotGameOver()
{
	action_sync->setEnabled(false);
	//action_connect->setEnabled(false);
	action_highscores->setEnabled(false);
	action_ggzplayers->setEnabled(false);
	action_ggzcontrol->setEnabled(false);

	if(m_networked)
		action_score->setEnabled(false);
}

// Scan a theme directory
void KTicTacTuxWin::scanDir(QDir dir, bool scanmore)
{
	QString name, player1, player2;
	QString file;

	QStringList entries = dir.entryList(QDir::Files);
	for(QStringList::iterator it = entries.begin(); it != entries.end(); it++)
	{
		if((*it).right(4) == ".png")
			continue;
		file = dir.path() + "/" + (*it);
		kDebug() << "Check file:" << file << endl;
		KConfig conf(file, KConfig::SimpleConfig);
		if(conf.hasGroup("Description"))
		{
			KConfigGroup cg = KConfigGroup(&conf, "Description");
			name = cg.readEntry("name");
			cg = KConfigGroup(&conf, "Pixmaps");
			player1 = cg.readEntry("player1");
			player2 = cg.readEntry("player2");
			kDebug() << "Found themes:" << player1 << player2;

			m_themes[name] = file;
			m_player1[file] = dir.path() + "/" + player1;
			m_player2[file] = dir.path() + "/" + player2;

			m_themenames[m_themeactions.count()] = name;
			QAction *action_theme = mtheme->addAction(KIconLoader::global()->loadIcon("games-config-theme", KIconLoader::Small), name);
			action_theme->setCheckable(true);
			m_themeactions << action_theme;
		}
	}

	if(scanmore)
	{
		QStringList entries = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		for(QStringList::iterator it = entries.begin(); it != entries.end(); it++)
		{
			QDir subdir(dir.path() + "/" + (*it));
			kDebug() << "Scan sub dir:" << (*it) << endl;
			scanDir(subdir, false);
		}
	}
}

// Read in all themes
void KTicTacTuxWin::loadThemes()
{
	KStandardDirs *d = KGlobal::dirs();

	// Recursively scan all data directories
	kDebug() << "loadThemes" << endl;
	QStringList list(d->findDirs("data", "ktictactux"));
	for(QStringList::iterator it = list.begin(); it != list.end(); it++)
	{
		QDir dir((*it));
		kDebug() << "Scan dir:" << (*it) << endl;
		scanDir(dir, true);
	}

	if(m_themes.size() == 0)
	{
		KMessageBox::error(this,
			i18n("No pixmap themes could be found"),
			i18n("Themes error"));
	}
}

void KTicTacTuxWin::slotError()
{
	enableNetwork(false);
}

void KTicTacTuxWin::connectcore()
{
	ConnectionDialog dialog(this);
	dialog.setGame("TicTacToe", "dio/5+dev");
	if(dialog.exec() == QDialog::Accepted)
	{
		enableNetwork(true);

		// The three lines below are completely optional and required only for the menu integration
		// with the ggz control panel action
		m_core = dialog.core();
		m_gcc = dialog.gamecoreclient();
		action_ggzcontrol->setChecked(true);
		m_gcc->setAttribute(Qt::WA_DeleteOnClose);
		connect(m_gcc, SIGNAL(destroyed()), action_ggzcontrol, SLOT(toggle()));
	}
}

