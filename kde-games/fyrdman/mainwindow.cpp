#include "mainwindow.h"

#include "map.h"
#include "levelselector.h"
#include "network.h"

#include <kmenubar.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <kapplication.h>

#include <qsocketnotifier.h>

MainWindow::MainWindow()
: KMainWindow()
{
	KPopupMenu *gamemenu;

	network = NULL;

	map = new Map(this);
	setCentralWidget(map);

	gamemenu = new KPopupMenu(this);
	gamemenu->insertItem(i18n("New game"), game_new);
	gamemenu->insertSeparator();
	gamemenu->insertItem(i18n("Information"), game_info);
	gamemenu->insertItem(i18n("Synchronize"), game_sync);
	gamemenu->insertSeparator();
	gamemenu->insertItem(i18n("Quit"), game_quit);

	displaymenu = new KPopupMenu(this);
	displaymenu->insertItem(i18n("Map"), option_map);
	displaymenu->insertItem(i18n("Knights"), option_knights);
	displaymenu->insertItem(i18n("Possession"), option_possession);

	displaymenu->setItemChecked(option_map, true);
	displaymenu->setItemChecked(option_knights, true);
	displaymenu->setItemChecked(option_possession, true);

	optionmenu = new KPopupMenu(this);
	optionmenu->insertItem(i18n("Display mode"), displaymenu);
	optionmenu->insertItem(i18n("Animation"), option_animation);

	optionmenu->setItemChecked(option_animation, true);

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
		case game_new:
			levelSelector();
			break;
		case game_info:
			break;
		case game_sync:
			break;
		case game_quit:
			kapp->quit();
			break;
		case option_map:
			displaymenu->setItemChecked(option_map, !displaymenu->isItemChecked(option_map));
			map->setMap(displaymenu->isItemChecked(option_map));
			break;
		case option_knights:
			displaymenu->setItemChecked(option_knights, !displaymenu->isItemChecked(option_knights));
			map->setKnights(displaymenu->isItemChecked(option_knights));
			break;
		case option_possession:
			displaymenu->setItemChecked(option_possession, !displaymenu->isItemChecked(option_possession));
			map->setPossession(displaymenu->isItemChecked(option_possession));
			break;
		case option_animation:
			optionmenu->setItemChecked(option_animation, !optionmenu->isItemChecked(option_animation));
			map->setAnimation(optionmenu->isItemChecked(option_animation));
			break;
	}
}

void MainWindow::levelSelector()
{
	int ret;

	LevelSelector l(this);
	ret = l.exec();
	if(ret == QDialog::Accepted)
	{
		// ...
	}
}

void MainWindow::enableNetwork()
{
	if(network) return;
	network = new Network();
	connect(network, SIGNAL(signalData()), SLOT(slotData()));
	network->connect();

	QSocketNotifier *sn = new QSocketNotifier(network->cfd(), QSocketNotifier::Read, this);
	connect(sn, SIGNAL(activated(int)), network, SLOT(slotDispatch()));
}

void MainWindow::slotData()
{
	// ...
}

