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
#include <qdir.h>

#include <stdlib.h> // abs

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

	backgroundmenu = new KPopupMenu(this);
	backgroundmenu->insertItem(i18n("Bayeux"), option_map_bayeux);
	backgroundmenu->insertItem(i18n("Standard map"), option_map_map);
	backgroundmenu->insertItem(i18n("Harold's return"), option_map_haroldsreturn);

	displaymenu = new KPopupMenu(this);
	displaymenu->insertItem(i18n("Map"), option_map);
	displaymenu->insertItem(i18n("Knights"), option_knights);
	displaymenu->insertItem(i18n("Possession"), option_possession);

	displaymenu->setItemChecked(option_map, true);
	displaymenu->setItemChecked(option_knights, true);
	displaymenu->setItemChecked(option_possession, true);

	optionmenu = new KPopupMenu(this);
	optionmenu->insertItem(i18n("Background"), backgroundmenu);
	optionmenu->insertItem(i18n("Display mode"), displaymenu);
	optionmenu->insertItem(i18n("Animation"), option_animation);

	optionmenu->setItemChecked(option_animation, true);

	menuBar()->insertItem(i18n("Game"), gamemenu);
	menuBar()->insertItem(i18n("Options"), optionmenu);
	menuBar()->insertItem(i18n("Help"), helpMenu());

	statusBar()->insertItem(i18n("Not yet playing"), status_state, 0);
	statusBar()->insertItem(i18n("No level selected"), status_level, 0);
	statusBar()->insertItem(i18n("Select a level..."), status_task, 0);

	connect(gamemenu, SIGNAL(activated(int)), SLOT(slotMenu(int)));
	connect(backgroundmenu, SIGNAL(activated(int)), SLOT(slotMenu(int)));
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
		case option_map_map:
			map->setBackground("map");
			break;
		case option_map_bayeux:
			map->setBackground("bayeux");
			break;
		case option_map_haroldsreturn:
			map->setBackground("haroldsreturn");
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
		QDir dir(GGZDATADIR "/fyrdman");
		QStringList s = dir.entryList();
		for(QStringList::iterator it = s.begin(); it != s.end(); it++)
		{
			Level *level = new Level();
			bool ret = level->loadFromFile(QString("%1/fyrdman/%2").arg(GGZDATADIR).arg((*it)));
			if(ret)
			{
				m_levels.append(level);
				l.addLevel(level);
			}
			else delete level;
		}

		/*Level *level = new Level();
		level->loadFromFile(GGZDATADIR "/fyrdman/battle_of_hastings");
		m_levels.append(level);
		l.addLevel(level);*/
	}
	ret = l.exec();
	if(ret == QDialog::Accepted)
	{
		statusBar()->changeItem(i18n("Level: %1").arg(l.level()), status_level);
		if(network)
		{
			ggz_write_int(network->fd(), Network::sndmap);
			ggz_write_string(network->fd(), l.level().latin1());
			statusBar()->changeItem(i18n("Waiting for players"), status_state);
		}
		else
		{
			statusBar()->changeItem(i18n("Game started"), status_state);
			statusBar()->changeItem(i18n("Select a knight"), status_task);
		}

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

	statusBar()->insertItem(i18n("Wait..."), status_task, 0);
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
			statusBar()->changeItem(i18n("Select a knight"), status_task);
			break;
		case Network::rspmove:
			ggz_read_char(network->fd(), &status);
			switch(status)
			{
				case 0:
					map->move(m_movex, m_movey, m_movex2, m_movey2);
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
			statusBar()->changeItem(i18n("Wait..."), status_task);
			ggz_read_int(network->fd(), &seat);
			ggz_read_int(network->fd(), &movesrcx);
			ggz_read_int(network->fd(), &movesrcy);
			ggz_read_int(network->fd(), &movedstx);
			ggz_read_int(network->fd(), &movedsty);
			kdDebug() << "* Move by: " << seat << endl;
			map->move(movesrcx, movesrcy, movedstx, movedsty);
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
			statusBar()->changeItem(i18n("Game over"), status_state);
			statusBar()->changeItem(i18n("No level selected"), status_level);
			ggz_read_char(network->fd(), &winner);
			KMessageBox::information(this,
				i18n("Player %1 has won.").arg(winner),
				i18n("Game over"));
			break;
		default:
			statusBar()->changeItem(i18n("Game interrupted"), status_state);
			statusBar()->changeItem(i18n("No level selected"), status_level);
			network->shutdown();
			KMessageBox::error(this,
				i18n("Bogus network message received."),
				i18n("Network error"));
			break;
	}
}

void MainWindow::slotMove(int x, int y, int x2, int y2)
{
	m_movex = x;
	m_movey = y;
	m_movex2 = x2;
	m_movey2 = y2;

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
	else
	{
		statusBar()->changeItem(i18n("Calculating..."), status_task);

		statusBar()->changeItem(i18n("Checking move..."), status_state);
		checkMove();
		statusBar()->changeItem(i18n("Move checked"), status_state);

		statusBar()->changeItem(i18n("Moving AI..."), status_state);
		aiMove();
		statusBar()->changeItem(i18n("AI moved"), status_state);

		statusBar()->changeItem(i18n("Wait..."), status_task);
	}
}

bool MainWindow::checkMove()
{
	int x, y, x2, y2;
	Level *l;
	int self;

	self = 1;

	l = map->level();

	x = m_movex;
	y = m_movey;
	x2 = m_movex2;
	y2 = m_movey2;

	if((x < 0) || (y < 0) || (x > l->width()) || (y > l->height())) return false;
	if((x2 < 0) || (y2 < 0) || (x2 > l->width()) || (y2 > l->height())) return false;

	if(((abs(x2 - x) > 1) || (abs(y2 - y) > 2))) return false;
	if((abs(x2 - x) == 1) && (abs(y2 - y) > 1)) return false;
	if((abs(y2 - y) == 0) && (abs(x2 - x) > 0)) return false;
	if((abs(y2 - y) == 2) && (abs(x2 - x) > 0)) return false;

//	if((x2 - x == -1) && (y2 - y == -1) && (y % 2 != 0)) return false;
//	if((x2 - x == -1) && (y2 - y == 1) && (y % 2 != 0)) return false;
//	if((x2 - x == 1) && (y2 - y == -1) && (y % 2 == 0)) return false;

	kdDebug() << "cell(x,y)=" << l->cell(x, y) << endl;
	kdDebug() << "cell(x2,y2)=" << l->cell(x2, y2) << endl;
	kdDebug() << "cellboard(x2,y2)=" << l->cellboard(x2, y2) << endl;

	if(l->cell(x, y) == -1) return false;
	if(l->cell(x2, y2) == self) return false;
	if(l->cellboard(x2, y2) == -1) return false;

	// ...

	map->move(m_movex, m_movey, m_movex2, m_movey2);

	return true;
}

void MainWindow::aiMove()
{
	Level *l;
	int self;
	
	self = -1;

	l = map->level();

	for(int bot = 0; bot < l->players(); bot++)
	{
		if(bot == self) continue;

		for(int x = 0; x < l->width(); x++)
			for(int y = 0; y < l->height(); y++)
				if(l->cell(x, y) == bot)
				{
					// ...
				}
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

