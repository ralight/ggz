#include "mainwindow.h"

#include "map.h"
#include "levelselector.h"
#include "network.h"

#include <kmenubar.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <kapplication.h>
#include <kstatusbar.h>
#include <kmessagebox.h>

#include <qsocketnotifier.h>

MainWindow::MainWindow()
: KMainWindow()
{
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

	statusBar()->insertItem(i18n("Not yet playing"), status_state, 0);
	statusBar()->insertItem(i18n("No level selected"), status_level, 0);

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
			levelSelector(false);
			break;
		case game_info:
			break;
		case game_sync:
			break;
		case game_quit:
			if(network) network->shutdown();
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

void MainWindow::levelSelector(bool networking)
{
	int ret;
	int count;

	LevelSelector l(this);
	if(networking)
	{
		ggz_read_int(network->fd(), &count);
		for(int i = 0; i < count; i++)
		{
			char *title, *author, *version;
			int mapwidth, mapheight;
			char board[30], boardmap[30];
			ggz_read_string_alloc(network->fd(), &title);
			ggz_read_string_alloc(network->fd(), &author);
			ggz_read_string_alloc(network->fd(), &version);
			ggz_read_int(network->fd(), &mapwidth);
			ggz_read_int(network->fd(), &mapheight);
			for(int j = 0; j < mapheight; j++)
				ggz_readn(network->fd(), &board, 30);
			for(int j = 0; j < mapheight; j++)
				ggz_readn(network->fd(), &boardmap, 30);

			l.addLevel(title);
		}
	}
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

	gamemenu->setItemEnabled(game_new, false);
}

void MainWindow::slotData()
{
	int op;
	int seat;
	int playernum;
	int seats[8];
	char names[8][17];

	ggz_read_int(network->fd(), &op);
	switch(op)
	{
		case Network::msgmaps:
			levelSelector(true);
			break;
		case Network::msgseat:
			ggz_read_int(network->fd(), &seat);
			break;
		case Network::msgplayers:
			ggz_read_int(network->fd(), &playernum);
			for(int i = 0; i < playernum; i++)
			{
				ggz_read_int(network->fd(), &seats[i]);
				if(seats[i] != GGZ_SEAT_OPEN)
				{
					ggz_read_string(network->fd(), (char*)names[i], 17);
				}
			}
			break;
		case Network::reqmove:
			break;
		case Network::rspmove:
			break;
		case Network::msgmove:
			break;
		case Network::sndsync:
			break;
		case Network::msggameover:
			break;
		default:
			KMessageBox::error(this,
				i18n("Bogus network message received."),
				i18n("Network error"));
			network->shutdown();
			break;
	}
}

