#include "mainwindow.h"

#include "map.h"

#include <kmenubar.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <kapplication.h>

MainWindow::MainWindow()
: KMainWindow()
{
	Map *map;
	KPopupMenu *gamemenu, *optionmenu, *displaymenu;

	map = new Map(this);
	setCentralWidget(map);

	gamemenu = new KPopupMenu(this);
	gamemenu->insertItem(i18n("Information"), game_info);
	gamemenu->insertItem(i18n("Synchronize"), game_sync);
	gamemenu->insertSeparator();
	gamemenu->insertItem(i18n("Quit"), game_quit);

	displaymenu = new KPopupMenu(this);
	displaymenu->insertItem(i18n("Map"), option_map);
	displaymenu->insertItem(i18n("Knights"), option_knights);
	displaymenu->insertItem(i18n("Possession"), option_possession);

	displaymenu->setItemChecked(option_map, true);
	displaymenu->setItemChecked(option_knights, false);
	displaymenu->setItemChecked(option_possession, true);

	optionmenu = new KPopupMenu(this);
	optionmenu->insertItem(i18n("Display mode"), displaymenu);
	optionmenu->insertItem(i18n("Animation"), option_animation);

	optionmenu->setItemChecked(option_animation, false);

	menuBar()->insertItem(i18n("Game"), gamemenu);
	menuBar()->insertItem(i18n("Options"), optionmenu);
	menuBar()->insertItem(i18n("Help"), helpMenu());

	connect(gamemenu, SIGNAL(activated(int)), SLOT(slotMenu(int)));
	connect(displaymenu, SIGNAL(activated(int)), SLOT(slotMenu(int)));
	connect(optionmenu, SIGNAL(activated(int)), SLOT(slotMenu(int)));

	setCaption(i18n("Fyrdman"));
	show();
}

MainWindow::~MainWindow()
{
}

void MainWindow::slotMenu(int id)
{
	switch(id)
	{
		case game_info:
			break;
		case game_sync:
			break;
		case game_quit:
			kapp->quit();
			break;
		case option_map:
			break;
		case option_knights:
			break;
		case option_possession:
			break;
		case option_animation:
			break;
	}
}

