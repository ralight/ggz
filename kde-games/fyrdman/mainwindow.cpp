#include "mainwindow.h"

#include "map.h"
#include "levelselector.h"
#include "network.h"
#include "level.h"

#include <kmenubar.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <kapplication.h>
#include <kstatusbar.h>
#include <kmessagebox.h>
#include <kdebug.h>

#include <qsocketnotifier.h>

#include "config.h"

MainWindow::MainWindow()
: KMainWindow()
{
	network = NULL;
	sn = NULL;
	snc = NULL;

	m_levels.setAutoDelete(true);

	map = new Map(this);
	setCentralWidget(map);

	gamemenu = new KPopupMenu(this);
	gamemenu->insertItem(i18n("New game"), game_new);
	gamemenu->insertSeparator();
	gamemenu->insertItem(i18n("Information"), game_info);
	gamemenu->insertItem(i18n("Synchronize"), game_sync);
	gamemenu->insertSeparator();
	gamemenu->insertItem(i18n("Quit"), game_quit);

	gamemenu->setItemEnabled(game_sync, false);

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
	connect(map, SIGNAL(signalMove(int, int, int, int)), SLOT(slotMove(int, int, int, int)));

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
			synchronize();
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

void MainWindow::levelSelector()
{
	int ret;
	int count;

	LevelSelector l(this);
	if(network)
	{
		ggz_read_int(network->fd(), &count);
		for(int i = 0; i < count; i++)
		{
			Level *level;
			level = new Level();
			level->loadFromNetwork(network->fd());
			m_levels.append(level);

			l.addLevel(level);
		}
	}
	else
	{
		Level *level = new Level();
		level->loadFromFile(GGZDATADIR "/fyrdman/battle_of_hastings");
		m_levels.append(level);
		l.addLevel(level);
	}
	ret = l.exec();
	if(ret == QDialog::Accepted)
	{
		if(network)
		{
			ggz_write_int(network->fd(), Network::sndmap);
			ggz_write_string(network->fd(), l.level().latin1());
		}
		statusBar()->changeItem(i18n("Level: %1").arg(l.level()), status_level);
		statusBar()->changeItem(i18n("Waiting for players"), status_state);

		for(Level *le = m_levels.first(); le; le = m_levels.next())
			if(le->title() == l.level()) map->setupMap(le);
	}
}

void MainWindow::enableNetwork()
{
	if(network) return;
	network = new Network();
	connect(network, SIGNAL(signalData()), SLOT(slotData()));
	network->connect();

	snc = new QSocketNotifier(network->cfd(), QSocketNotifier::Read, this);
	connect(snc, SIGNAL(activated(int)), network, SLOT(slotDispatch()));

	gamemenu->setItemEnabled(game_new, false);
	gamemenu->setItemEnabled(game_sync, true);
}

void MainWindow::slotData()
{
	int op;
	int seat;
	int playernum;
	int seats[8];
	char names[8][17];
	char winner;
	char status;
	int movesrcx, movesrcy, movedstx, movedsty;
	int turn;
	char cell;

	kdDebug() << "data!" << endl;
	if(!sn)
	{
		sn = new QSocketNotifier(network->fd(), QSocketNotifier::Read, this);
		connect(sn, SIGNAL(activated(int)), SLOT(slotData()));
	}

	ggz_read_int(network->fd(), &op);
	switch(op)
	{
		case Network::msgmaps:
			statusBar()->changeItem(i18n("Selecting map"), status_state);
			levelSelector();
			break;
		case Network::msgseat:
			ggz_read_int(network->fd(), &seat);
			kdDebug() << "-> Seat: " << seat << endl;
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
				else
				{
					strcpy(names[i], i18n("Bot").latin1());
				}
				kdDebug() << "-> Player: " << names[i] << endl;
			}

			ggz_write_int(network->fd(), Network::reqinit);
			break;
		case Network::reqmove:
			statusBar()->changeItem(i18n("Select a knight"), status_state);
			break;
		case Network::rspmove:
			ggz_read_char(network->fd(), &status);
			switch(status)
			{
				case 0:
					statusBar()->changeItem(i18n("Move accepted"), status_state);
					break;
				case Network::errstate:
					statusBar()->changeItem(i18n("Server not ready yet"), status_state);
					break;
				case Network::errturn:
					statusBar()->changeItem(i18n("Not your turn"), status_state);
					break;
				case Network::errbound:
					statusBar()->changeItem(i18n("Out of bounds move"), status_state);
					break;
				case Network::errempty:
					statusBar()->changeItem(i18n("Nothing to move"), status_state);
					break;
				case Network::errfull:
					statusBar()->changeItem(i18n("Space occupied"), status_state);
					break;
				case Network::errdist:
					statusBar()->changeItem(i18n("Distance too large"), status_state);
					break;
				case Network::errmap:
					statusBar()->changeItem(i18n("Disallowed by map"), status_state);
					break;
				default:
					break;
			}
			break;
		case Network::msgmove:
			ggz_read_int(network->fd(), &seat);
			ggz_read_int(network->fd(), &movesrcx);
			ggz_read_int(network->fd(), &movesrcy);
			ggz_read_int(network->fd(), &movedstx);
			ggz_read_int(network->fd(), &movedsty);
			kdDebug() << "* Move by: " << seat << endl;
			break;
		case Network::sndsync:
			statusBar()->changeItem(i18n("Performing sync..."), status_state);
			ggz_read_int(network->fd(), &turn);
			for (int i = 0; i < 6; i++)
				for (int j = 0; j < 19; j++)
				{
					ggz_read_char(network->fd(), &cell);
				}
			for (int i = 0; i < 6; i++)
				for (int j = 0; j < 19; j++)
				{
					ggz_read_char(network->fd(), &cell);
				}
			statusBar()->changeItem(i18n("Sync successful"), status_state);
			break;
		case Network::msggameover:
			ggz_read_char(network->fd(), &winner);
			KMessageBox::information(this,
				i18n("Player %1 has won.").arg(winner),
				i18n("Game over"));
			break;
		default:
			network->shutdown();
			KMessageBox::error(this,
				i18n("Bogus network message received."),
				i18n("Network error"));
			break;
	}
}

void MainWindow::slotMove(int x, int y, int x2, int y2)
{
	if(network)
	{
		statusBar()->changeItem(i18n("Sending move..."), status_state);
		ggz_write_int(network->fd(), Network::sndmove);
		ggz_write_int(network->fd(), x);
		ggz_write_int(network->fd(), y);
		ggz_write_int(network->fd(), x2);
		ggz_write_int(network->fd(), y2);
		statusBar()->changeItem(i18n("Move sent"), status_state);
	}
}

void MainWindow::synchronize()
{
	if(network)
	{
		statusBar()->changeItem(i18n("Request synchronization"), status_state);
		ggz_write_int(network->fd(), Network::reqsync);
	}
}

