// Muehle - KDE Muehle (Nine Men's Morris) game for GGZ
// Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

// Header file
#include "toplevel.h"

// Muehle includes
#include "board.h"

// KDE includes
#include <kpopupmenu.h>
#include <kmenubar.h>
#include <ksimpleconfig.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <klocale.h>
#include <kstatusbar.h>

// Qt includes
#include <qlayout.h>

// Configuration includes
#include "config.h"

// Constructor: create main Muehle window including menus
Toplevel::Toplevel()
: KMainWindow()
{
	KPopupMenu *menu_theme, *menu_player;
	int counter;
	KStandardDirs d;
	QString s;
	QWidget *root;
	QVBoxLayout *vbox;
	QString icontheme;

#if ((KDE_VERSION_MAJOR == 3) && (KDE_VERSION_MINOR >= 1) || (KDE_VERSION_MAJOR > 3))
	icontheme = "crystalsvg";
#else
	icontheme = "hicolor";
#endif

	QString pixnew = d.findResource("icon", icontheme + "/16x16/mimetypes/Mime.png");
	QString pixexit = d.findResource("icon", icontheme + "/16x16/actions/exit.png");
	QString pixvariant = d.findResource("icon", icontheme + "/16x16/actions/gear.png");
	QString pixtheme = d.findResource("icon", icontheme + "/16x16/actions/imagegallery.png");
	QString pixaction = d.findResource("icon", icontheme + "/16x16/actions/wizard.png");


	menu_game = new KPopupMenu(this);
	menu_game->insertItem(QIconSet(QPixmap(pixnew)), i18n("Start new game"), menugamenew);
	menu_game->insertSeparator();
	menu_game->insertItem(QIconSet(QPixmap(pixexit)), i18n("Quit"), menugamequit);

	counter = 0;
	menu_variants = new KPopupMenu(this);
	d.addResourceDir("data", GGZDATADIR);
	s = d.findResource("data", "muehle/rc");
	KSimpleConfig conf(s);
	conf.setGroup("Muehle");
	variantslist = conf.readListEntry("Variants");
	for(QStringList::Iterator it = variantslist.begin(); it != variantslist.end(); it++)
	{
		conf.setGroup((*it));
		QString file = conf.readEntry("file");
		QString description = conf.readEntry("description");
		int width = conf.readNumEntry("width");
		int height = conf.readNumEntry("height");
		menu_variants->insertItem(QIconSet(QPixmap(pixvariant)), description, menuvariants + counter++);
	}

	counter = 0;
	menu_theme = new KPopupMenu(this);
	s = d.findResource("data", "muehle/themerc");
	KSimpleConfig tconf(s);
	tconf.setGroup("Themes");
	themelist = tconf.readListEntry("Themes");
	for(QStringList::Iterator it = themelist.begin(); it != themelist.end(); it++)
	{
		tconf.setGroup((*it));
		QString name = tconf.readEntry("Name");
		menu_theme->insertItem(QIconSet(QPixmap(pixtheme)), name, menuthemes + counter++);
	}

	menu_player = new KPopupMenu(this);
	menu_player->insertItem(QIconSet(QPixmap(pixaction)), i18n("Offer remis"), menuplayerremis);
	menu_player->insertItem(QIconSet(QPixmap(pixaction)), i18n("Give up"), menuplayerloose);

	menuBar()->insertItem(i18n("Game"), menu_game, 1);
	menuBar()->insertItem(i18n("Variants"), menu_variants, 2);
	menuBar()->insertItem(i18n("Player"), menu_player, 3);
	menuBar()->insertItem(i18n("Theme"), menu_theme, 4);
	menuBar()->insertItem(i18n("Help"), helpMenu(), 5);

	statusBar()->insertItem("", statusplayer, 2);
	statusBar()->insertItem("", statusopponent, 2);
	statusBar()->insertItem(i18n("Welcome to Muehle."), statushint, 2);

	root = new QWidget(this);
	board = new Board(root);
	vbox = new QVBoxLayout(root);
	vbox->add(board);

	setCentralWidget(root);
	show();

	connect(menu_game, SIGNAL(activated(int)), SLOT(slotMenu(int)));
	connect(menu_player, SIGNAL(activated(int)), SLOT(slotMenu(int)));
	connect(menu_theme, SIGNAL(activated(int)), SLOT(slotMenu(int)));
	connect(menu_variants, SIGNAL(activated(int)), SLOT(slotMenu(int)));

	connect(board, SIGNAL(signalEnd()), SLOT(slotEnd()));
	connect(board, SIGNAL(signalStatus(const QString &)), SLOT(slotStatus(const QString &)));
	connect(board, SIGNAL(signalScore(const QString &, int, int, int)),
		SLOT(slotScore(const QString &, int, int, int)));

	resize(600, 620);
}

// Destructor
Toplevel::~Toplevel()
{
}

// Process a click on a menu item
void Toplevel::slotMenu(int id)
{
	switch(id)
	{
		case menugamenew:
			board->init();
			break;
		case menugamequit:
			close();
			break;
		case menuplayerremis:
			board->remis();
			break;
		case menuplayerloose:
			board->loose();
			break;
		default:
			if((id >= menuthemes) && (id < menuvariants))
			{
				board->setTheme(themelist[id - menuthemes]);
			}
			else if(id >= menuvariants)
			{
				board->setVariant(variantslist[id - menuvariants]);
			}
	}
}

// Update the status bar
void Toplevel::slotStatus(const QString &message)
{
	statusBar()->changeItem(message, statushint);
}

// Update the scores in the status bar
void Toplevel::slotScore(const QString &player, int num, int score, int stones)
{
	if(stones)
		statusBar()->changeItem(QString("%1: %1 points, %1 stones left").arg(player).arg(score).arg(stones), num);
	else
		statusBar()->changeItem(QString("%1: %1 points").arg(player).arg(score), num);
}

// Turn the game client into a network game interface
void Toplevel::enableNetwork(bool enabled)
{
	if(enabled)
	{
		menuBar()->setItemEnabled(2, false);
		menu_game->setItemEnabled(menugamenew, false);
		setCaption(i18n("Network game"));
	}
	board->enableNetwork(enabled);
}

// End the game
void Toplevel::slotEnd()
{
	menuBar()->setItemEnabled(3, false);
	setCaption(i18n("Game over"));
}


