#include "mainwindow.h"

#include "map.h"
#include "levelselector.h"
#include "network.h"
#include "level.h"
#include "unitinfo.h"

#include <kmenubar.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <kapplication.h>
#include <kstatusbar.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kprocess.h>
#include <kstandarddirs.h>

#include <qsocketnotifier.h>
#include <qdir.h>

#include <stdlib.h> // abs
#include <unistd.h> // getpid

#include "config.h"

#ifdef HAVE_KNEWSTUFF
#include <knewstuff/downloaddialog.h>
#endif

MainWindow::MainWindow()
: DCOPObject("fyrdman"), KMainWindow()
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
	gamemenu->insertItem(i18n("Unit information"), game_unitinfo);
	gamemenu->insertItem(i18n("Game information"), game_gameinfo);
	gamemenu->insertItem(i18n("Synchronize"), game_sync);
	gamemenu->insertSeparator();
#ifdef HAVE_KNEWSTUFF
	gamemenu->insertItem(KGlobal::iconLoader()->loadIcon("knewstuff", KIcon::Small), i18n("Get levels"), game_newlevels);
	gamemenu->insertSeparator();
#endif
	gamemenu->insertItem(i18n("Quit"), game_quit);

	gamemenu->setItemEnabled(game_info, false);
	gamemenu->setItemEnabled(game_unitinfo, false);
	gamemenu->setItemEnabled(game_gameinfo, false);
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
	QDir d;

	switch(id)
	{
		case game_new:
			levelSelector();
			break;
		case game_info:
			levelInformation();
			break;
		case game_unitinfo:
			unitInformation(m_self);
			break;
		case game_gameinfo:
			gameInformation();
			break;
		case game_sync:
			synchronize();
			break;
#ifdef HAVE_KNEWSTUFF
		case game_newlevels:
			d.mkdir(QDir::home().path() + "/.ggz");
			d.mkdir(QDir::home().path() + "/.ggz/games");
			d.mkdir(QDir::home().path() + "/.ggz/games/fyrdman");
			KNS::DownloadDialog::open("fyrdman/level");
			scanNewLevels();
			break;
#endif
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

void MainWindow::levelInformation()
{
	if(!map->level()) return;

	LevelSelector l(true, this);
	l.addLevel(map->level());
	l.exec();
}

void MainWindow::unitInformation(int num)
{
	UnitInfo u(num, this);
	u.exec();
}

void MainWindow::gameInformation()
{
	QString text;
	int territory1, territory2, soldiers1, soldiers2;
	Level *l;

	l = map->level();
	territory1 = 0;
	territory2 = 0;
	soldiers1 = 0;
	soldiers2 = 0;
	for(int i = 0; i < l->width(); i++)
		for(int j = 0; j < l->height(); j++)
		{
			if(l->cellown(i, j) == -1) continue;
			if(l->cellown(i, j) % 2 == m_self % 2) territory1++;
			else territory2++;
			if(l->cell(i, j) == -1) continue;
			if(l->cell(i, j) % 2 == m_self % 2) soldiers1++;
			else soldiers2++;
		}
	territory1 = 100 * territory1 / (l->width() * l->height());
	territory2 = 100 * territory2 / (l->width() * l->height());

	text = i18n("Your alliance has conquered %1 percent of the territory.\n").arg(territory1);
	text += i18n("The enemy claims %1 percent instead.\n").arg(territory2);
	text += i18n("You have %1 soldiers to offer.\n").arg(soldiers1);
	text += i18n("The enemy has %1.\n").arg(soldiers2);

	KMessageBox::information(this, text, i18n("Game information"));
}

ASYNC MainWindow::newLevel(QString level)
{
	KProcess proc;
	proc << "tar";
	proc << "-C" << QString(QDir::home().path() + "%1").arg("/.ggz/games/fyrdman").latin1();
	proc << "-xvzf" << level.latin1();
	proc.start(KProcess::Block);
}

void MainWindow::scanLevels(QString basedir)
{
	QDir dir(basedir + "/fyrdman");
	QStringList s = dir.entryList();
	for(QStringList::iterator it = s.begin(); it != s.end(); it++)
	{
		if(((*it) == ".") || ((*it) == "..")) continue;
		QString filename = QString("%1/fyrdman/%2").arg(basedir).arg((*it));
		bool ret = true;
		for(Level *l = m_levels.first(); l; l = m_levels.next())
			if(QString(l->location()) == filename) ret = false;
		if(ret)
		{
			Level *level = new Level();
			ret = level->loadFromFile(filename);
			if(ret) m_levels.append(level);
			else delete level;
		}
	}
}

void MainWindow::scanNewLevels()
{
	KStandardDirs d;
	QString basedir = d.findResource("data", "fyrdman/");
	QDir dir(basedir);
	QStringList s = dir.entryList();
	for(QStringList::iterator it = s.begin(); it != s.end(); it++)
	{
		if(((*it) == ".") || ((*it) == "..")) continue;
		newLevel(basedir + "/" + (*it));
	}
	scanLevels(dir.home().path() + "/.ggz/games");
}

void MainWindow::levelSelector()
{
	int ret;
	int count;

	LevelSelector l(false, this);
	if(network)
	{
		ggz_read_int(network->fd(), &count);
		for(int i = 0; i < count; i++)
		{
			Level *level;
			level = new Level();
			level->loadFromNetwork(network->fd());
			m_levels.append(level);
		}
	}
	else
	{
		QDir d;
		scanLevels(GGZDATADIR);
		scanLevels(d.home().path() + "/.ggz/games");
	}

	for(Level *level = m_levels.first(); level; level = m_levels.next())
		l.addLevel(level);

	ret = l.exec();
	if(ret == QDialog::Accepted)
	{
		statusBar()->changeItem(i18n("Level: %1").arg(l.level()), status_level);
		gamemenu->setItemEnabled(game_info, true);
		gamemenu->setItemEnabled(game_unitinfo, true);
		gamemenu->setItemEnabled(game_gameinfo, true);

		if(network)
		{
			ggz_write_int(network->fd(), Network::sndmap);
			ggz_write_string(network->fd(), l.level().latin1());
			statusBar()->changeItem(i18n("Waiting for players"), status_state);
		}
		else
		{
			m_self = 1;

			statusBar()->changeItem(i18n("Game started"), status_state);
			statusBar()->changeItem(i18n("Select a knight"), status_task);
		}

		for(Level *le = m_levels.first(); le; le = m_levels.next())
			if(le->title() == l.level()) map->setupMap(le);

		if(!network)
			unitInformation(m_self);
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

			unitInformation(seat);
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
	bool ret;

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
		ret = checkMove(m_self, true);

		if(ret)
		{
			statusBar()->changeItem(i18n("Move checked"), status_state);

			statusBar()->changeItem(i18n("Moving AI..."), status_state);
			aiMove();
			statusBar()->changeItem(i18n("AI moved"), status_state);
		}
		else
		{
			statusBar()->changeItem(i18n("Invalid move"), status_state);
		}

		statusBar()->changeItem(i18n("Select a knight"), status_task);
	}
}

bool MainWindow::checkMove(int self, bool execute)
{
	int x, y, x2, y2;
	Level *l;

	l = map->level();

	x = m_movex;
	y = m_movey;
	x2 = m_movex2;
	y2 = m_movey2;

	if((x < 0) || (y < 0) || (x > l->width()) || (y > l->height())) return false;
	if((x2 < 0) || (y2 < 0) || (x2 > l->width()) || (y2 > l->height())) return false;

	if(((abs(x2 - x) > 1) || (abs(y2 - y) > 2))) return false;
	if(abs(y2 - y) == 0) return false;
	if((abs(x2 - x) == 1) && (abs(y2 - y) > 1)) return false;
	if((x2 - x == -1) && (y % 2 == 0)) return false;
	if((x2 - x == 1) && (y % 2 == 1)) return false;

	if(l->cell(x, y) != self) return false;
	if(l->cell(x2, y2) == self) return false;
	if(l->cell(x2, y2) % 2 == self % 2) return false;
	if(l->cellboard(x2, y2) != 'x') return false;

	if(execute)
		map->move(m_movex, m_movey, m_movex2, m_movey2);

	return true;
}

int MainWindow::cellValue(int bot, int x, int y, bool neighbours)
{
	int k, j, tmpval, mval;
	Level *l;

	mval = 0;
	m_movex = x;
	m_movey = y;

	l = map->level();

	for(k = -2; k < 3; k++)
	{
		for(j = -2; j < 3; j++)
		{
			if(neighbours)
			{
				if((!j) && (!k)) continue;
			}
			else if((j) || (k)) continue;
			m_movex2 = x + k;
			m_movey2 = y + j;
			if((!neighbours) || (checkMove(bot, false)))
			{
				if(!mval) tmpval = 1;
				else tmpval = 0;
				if(!neighbours)
				{
					if(l->cellown(m_movex2, m_movey2) == -1) tmpval += 4;
					else if(l->cellown(m_movex2, m_movey2) % 2 != bot % 2) tmpval += 7;
					if(l->cell(m_movex2, m_movey2) == -1) tmpval += 0;
					else if(l->cell(m_movex2, m_movey2) % 2 != bot % 2) tmpval += 20;
				}
				else
				{
					if(l->cell(m_movex2, m_movey2) == -1) tmpval += 0;
					else if(l->cell(m_movex2, m_movey2) % 2 != bot % 2) tmpval -= 12;
					else if(l->cell(m_movex2, m_movey2) % 2 == bot % 2)
					{
						if(l->cell(m_movex2, m_movey2) != bot) tmpval += 7;
					}
				}
				mval += tmpval;
			}
		}
	}
	return mval;
}

void MainWindow::aiMove()
{
	Level *l;
	int k, j;
	int mx, my, mx2, my2, mval, tmpval;
	int mxtmp, mytmp, mxtmp2, mytmp2;
	int s1, s2, winner;
	
	l = map->level();

	mx = 0;
	my = 0;
	mx2 = 0;
	my2 = 0;
	for(int bot = 0; bot < l->players(); bot++)
	{
		if(bot == m_self) continue;

		mval = 0;
		for(int x = 0; x < l->width(); x++)
		{
			for(int y = 0; y < l->height(); y++)
			{
				if(l->cell(x, y) == bot)
				{
					m_movex = x;
					m_movey = y;
					kdDebug() << "=== ** move bot at " << x << ", " << y << endl;

					if(!mval)
					{
						for(int k = 0; k < l->width(); k++)
						{
							for(int j = 0; j < l->height(); j++)
							{
								if((l->cell(k, j) != -1) && (l->cell(k, j) % 2 != bot % 2))
								{
									m_movex2 = x + ((k - x) ? (k - x) / abs(k - x) : 0);
									m_movey2 = y + ((j - y) ? (j - y) / abs(j - y) : 0);
									if(checkMove(bot, false))
									{
										mval = (l->width() + l->height() - abs(k - x) - abs(j - y)) / 5;
										mx = m_movex;
										my = m_movey;
										mx2 = m_movex2;
										my2 = m_movey2;
									}
								}
							}
						}
					}

					for(k = -2; k < 3; k++)
					{
						for(j = -2; j < 3; j++)
						{
							if((!j) && (!k)) continue;
							m_movex2 = x + k;
							m_movey2 = y + j;
							if(checkMove(bot, false))
							{
								mxtmp = m_movex;
								mytmp = m_movey;
								mxtmp2 = m_movex2;
								mytmp2 = m_movey2;
								tmpval = cellValue(bot, m_movex2, m_movey2, false);
//kdDebug() << "** pre-CELLVALUE at " << mxtmp2 << "," << mytmp2 << " valued at " << tmpval << endl;
								tmpval += cellValue(bot, m_movex2, m_movey2, true);
//kdDebug() << "** CELLVALUE at " << mxtmp2 << "," << mytmp2 << " valued at " << tmpval << endl;
								m_movex = mxtmp;
								m_movey = mytmp;
								if(tmpval > mval)
								{
									mval = tmpval;
									mx = mxtmp;
									my = mytmp;
									mx2 = mxtmp2;
									my2 = mytmp2;
								}
							}
						}
					}
				}
			}
		}

		if(mval)
		{
//kdDebug() << "===> move bot at " << mx << ", " << my << " to " << mx2 << ", " << my2 << " -- value " << mval << endl;
			m_movex = mx;
			m_movey = my;
			m_movex2 = mx2;
			m_movey2 = my2;
			checkMove(bot, true);
		}
	}

	s1 = 0;
	s2 = 0;
	for(int x = 0; x < l->width(); x++)
		for(int y = 0; y < l->height(); y++)
			if(l->cell(x, y) == -1);
			else if(l->cell(x, y) % 2 == m_self % 2) s1++;
			else if(l->cell(x, y) % 2 != m_self % 2) s2++;

	winner = -1;
	if(!s1) winner = 1;
	if(!s2) winner = 0;
//kdDebug() << "S1 " << s1 << " S2 " << s2 << endl;
	if(winner >= 0)
	{
		KMessageBox::information(this,
			i18n("The game is over. Winner is alliance %1.").arg(winner),
			i18n("Game over!"));

		statusBar()->changeItem(i18n("Not playing"), status_level);
		gamemenu->setItemEnabled(game_info, false);
		gamemenu->setItemEnabled(game_unitinfo, false);
		gamemenu->setItemEnabled(game_gameinfo, false);
		map->setupMap(NULL);
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

