///////////////////////////////////////////////////////////////////
// KCC
// Copyright (C) 2003, 2004 Josef Spillner, josef@ggzgamingzone.org
// Published under GNU GPL conditions
///////////////////////////////////////////////////////////////////

// KCC includes
#include "kccwin.h"

#include "kggzseatsdialog.h"

// Configuration
#include "config.h"

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
#include <kiconloader.h>
#include <kprocess.h>

#ifdef HAVE_KNEWSTUFF
#include <knewstuff/downloaddialog.h>
#endif

// Qt includes
#include <qdir.h>
#include <qstringlist.h>

// Constructor
KCCWin::KCCWin(QWidget *parent, const char *name)
: KMainWindow(parent, name)
{
	m_networked = false;

	mgame = new KPopupMenu(this);
	mgame->insertItem(KGlobal::iconLoader()->loadIcon("reload", KIcon::Small), i18n("Synchronize"), menusync);
	mgame->insertItem(KGlobal::iconLoader()->loadIcon("history", KIcon::Small), i18n("View score"), menuscore);
	mgame->insertItem(KGlobal::iconLoader()->loadIcon("thumbnail", KIcon::Small), i18n("Seats..."), menuseats);
	mgame->insertSeparator();
#ifdef HAVE_KNEWSTUFF
	mgame->insertItem(KGlobal::iconLoader()->loadIcon("knewstuff", KIcon::Small), i18n("Get themes"), menutheme);
	mgame->insertSeparator();
#endif
	mgame->insertItem(KGlobal::iconLoader()->loadIcon("exit", KIcon::Small), i18n("Quit"), menuquit);

	mtheme = new KPopupMenu(this);

	menuBar()->insertItem(i18n("Game"), mgame);
	menuBar()->insertItem(i18n("Theme"), mtheme);
	menuBar()->insertItem(i18n("Help"), helpMenu());

	statusBar()->insertItem(i18n("Status"), 1, 1);
	statusBar()->insertItem(i18n("Game with the AI"), 2, 1);

	loadThemes();

	m_kcc = new KCC(this);

	connect(m_kcc, SIGNAL(signalStatus(const QString &)), SLOT(slotStatus(const QString &)));
	connect(m_kcc, SIGNAL(signalScore(const QString &)), SLOT(slotScore(const QString &)));
	connect(m_kcc, SIGNAL(signalNetworkScore(int, int)), SLOT(slotNetworkScore(int, int)));
	connect(m_kcc, SIGNAL(signalGameOver()), SLOT(slotGameOver()));
	connect(mgame, SIGNAL(activated(int)), SLOT(slotMenu(int)));
	connect(mtheme, SIGNAL(activated(int)), SLOT(slotMenu(int)));

	enableNetwork(false);

	setCaption(i18n("Chinese Checkers"));
	show();
}

// Destructor
KCCWin::~KCCWin()
{
}

// install new theme
void KCCWin::newTheme(QString theme)
{
	KProcess proc;
	proc << "tar";
	proc << "-C" << QString(QDir::home().path() + "%1").arg("/.ggz/games/kcc").latin1();
	proc << "-xvzf" << theme.latin1();
	proc.start(KProcess::Block);
}

void KCCWin::scanNewThemes()
{
	KStandardDirs d;

	d.addResourceDir("data", GGZDATADIR);
	d.addResourceDir("data", QDir::home().path() + "/.ggz/games");

	QString basedir = d.findResource("data", "kcc/");
	QDir dir(basedir);
	QStringList s = dir.entryList(QDir::Files);
	for(QStringList::iterator it = s.begin(); it != s.end(); it++)
	{
		if(((*it) == ".") || ((*it) == "..")) continue;
		newTheme(basedir + "/" + (*it));
	}

	mtheme->clear();
	loadThemes();
}

// Read in all themes
void KCCWin::loadThemes()
{
	KStandardDirs d;
	QString type;
	int index = menuthemes;
	int valid;

	d.addResourceDir("data", GGZDATADIR);
	d.addResourceDir("data", QDir::home().path() + "/.ggz/games");

	// Recursively scan all data directories
	kdDebug() << "loadThemes" << endl;
	QStringList list(d.findDirs("data", "kcc/"));
	for(QStringList::iterator it = list.begin(); it != list.end(); it++)
	{
		kdDebug() << "Scan dir: " << (*it) << endl;
		valid = 0;
		type = QString::null;
		QDir dir((*it));
		QStringList dirs = dir.entryList(QDir::Dirs);
		for(QStringList::iterator it2 = dirs.begin(); it2 != dirs.end(); it2++)
		{
			if ((*it2) == ".") continue;
			if ((*it2) == "..") continue;
			kdDebug() << "Check theme dir: " << (*it2) << endl;
			QDir dir2((*it) + (*it2));
			QStringList entries = dir2.entryList(QDir::Files);
			for(QStringList::iterator it3 = entries.begin(); it3 != entries.end(); it3++)
			{
				if((*it3) == "board.xpm")
				{
					valid++;
					type = "xpm";
				}
				if((*it3) == "board.png")
				{
					valid++;
					type = "png";
				}
			}
			if(valid)
			{
				kdDebug() << "Insert " << (*it) + (*it2) << " type " << type << endl;
				mtheme->insertItem(dir2.dirName(), index);
				m_themenames[index] = (*it) + (*it2);
				m_themetypes[index] = type;
				index++;
			}
		}
	}
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
	QDir d;
	KGGZSeatsDialog *seats;

	// Standard menu entries
	switch(id)
	{
		case menusync:
			m_kcc->sync();
			break;
		case menuscore:
			score();
			break;
		case menuseats:
			/*seats = new KGGZSeatsDialog();
			seats->setMod(m_kcc->getProto()->mod);*/
			// FIXME: disabled until ported to kggzmod
			break;
#ifdef HAVE_KNEWSTUFF
		case menutheme:
			d.mkdir(QDir::home().path() + "/.ggz");
			d.mkdir(QDir::home().path() + "/.ggz/games");
			d.mkdir(QDir::home().path() + "/.ggz/games/kcc");
			KNS::DownloadDialog::open("ccheckers/theme");
			scanNewThemes();
			break;
#endif
		case menuquit:
			close();
			break;
	}

	if(id >= menuthemes)
	{
		m_kcc->setTheme(m_themenames[id], m_themetypes[id]);
	}
}

/// Enable network functionality
void KCCWin::enableNetwork(bool enabled)
{
	mgame->setItemEnabled(menusync, enabled);
	mgame->setItemEnabled(menuseats, enabled);
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
	mgame->setItemEnabled(menuseats, false);
	if(m_networked) mgame->setItemEnabled(menuscore, false);
}

