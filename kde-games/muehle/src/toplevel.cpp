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
#include <kstddirs.h>

// Configuration includes
#include "config.h"

// Constructor: create main Muehle window including menus
Toplevel::Toplevel()
: KMainWindow()
{
	KPopupMenu *menu_game, *menu_theme, *menu_player, *menu_variants;
	int counter;
	KStandardDirs d;
	QString s;

	menu_game = new KPopupMenu(this);
	menu_game->insertItem("New", menugamenew);
	menu_game->insertSeparator();
	menu_game->insertItem("Quit", menugamequit);

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
		menu_variants->insertItem(description, menuvariants + counter++);
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
		menu_theme->insertItem(name, menuthemes + counter++);
	}

	menu_player = new KPopupMenu(this);
	menu_player->insertItem("Offer remis", menuplayerremis);
	menu_player->insertItem("Give up", menuplayerloose);

	menuBar()->insertItem("Game", menu_game);
	menuBar()->insertItem("Variants", menu_variants);
	menuBar()->insertItem("Player", menu_player);
	menuBar()->insertItem("Theme", menu_theme);
	menuBar()->insertItem("Help", helpMenu());

	statusBar()->insertItem("josef: 2", 0, 2);
	statusBar()->insertItem("pHr3ak: 3", 1, 2);
	statusBar()->insertItem("Place a stone onto the board.", 2, 2);

	board = new Board(this);
	setCentralWidget(board);
	show();

	connect(menu_game, SIGNAL(activated(int)), SLOT(slotMenu(int)));
	connect(menu_player, SIGNAL(activated(int)), SLOT(slotMenu(int)));
	connect(menu_theme, SIGNAL(activated(int)), SLOT(slotMenu(int)));
	connect(menu_variants, SIGNAL(activated(int)), SLOT(slotMenu(int)));

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

