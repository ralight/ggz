/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//    KGGZ - The KDE client for the GGZ Gaming Zone - Version 0.0.5pre             //
//    Copyright (C) 2000 - 2002 Josef Spillner - dr_maux@users.sourceforge.net     //
//    The MindX Open Source Project - http://mindx.sourceforge.net                 //
//    Published under GNU GPL conditions - view COPYING for details                //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//    This program is free software; you can redistribute it and/or modify         //
//    it under the terms of the GNU General Public License as published by         //
//    the Free Software Foundation; either version 2 of the License, or            //
//    (at your option) any later version.                                          //
//                                                                                 //
//    This program is distributed in the hope that it will be useful,              //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of               //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                //
//    GNU General Public License for more details.                                 //
//                                                                                 //
//    You should have received a copy of the GNU General Public License            //
//    along with this program; if not, write to the Free Software                  //
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA    //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
// KGGZUsers: Display all users in the current room, and place them on the tables. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

// Header file
#include "KGGZUsers.h"

// KGGZ includes
#include "KGGZCommon.h"
#include "KGGZChat.h"

// GGZCore++ includes
#include "GGZCoreConfio.h"
#include "GGZCoreRoom.h"
#include "GGZCorePlayer.h"

// KDE includes
#include <klocale.h>
#include <kmessagebox.h>
#include <kinputdialog.h>

// Qt includes
#include <qpixmap.h>
#include <qimage.h>
#include <qpopupmenu.h>
#include <qdir.h>
#include <qpainter.h>

// Constructor
KGGZUsers::KGGZUsers(QWidget *parent, const char *name)
: KListView(parent, name)
{
	addColumn(i18n("Players"));
	addColumn(i18n("Icon"));
	addColumn(i18n("Lag"));

	removeall();

	setRootIsDecorated(TRUE);

	m_room = NULL;
	m_menu = NULL;

	m_menu_assign = new QPopupMenu(NULL);
	m_menu_assign->insertItem(QIconSet(QPixmap(KGGZ_DIRECTORY "/images/icons/players/normal.png")),
		i18n("Neutral"), creditnone);
	m_menu_assign->insertItem(QIconSet(QPixmap(KGGZ_DIRECTORY "/images/icons/players/buddy.png")),
		i18n("Buddy"), creditbuddy);
	m_menu_assign->insertItem(QIconSet(QPixmap(KGGZ_DIRECTORY "/images/icons/players/banned.png")),
		i18n("Banned"), creditbanned);

	m_menu_info = new QPopupMenu(NULL);
	m_menu_info->insertItem(i18n("Record"), inforecord);

	connect(this,
		SIGNAL(rightButtonPressed(QListViewItem*, const QPoint&, int)),
		SLOT(slotClicked(QListViewItem*, const QPoint&, int)));
	connect(m_menu_assign, SIGNAL(activated(int)), SLOT(slotCredited(int)));
	connect(m_menu_info, SIGNAL(activated(int)), SLOT(slotInformation(int)));
}

// Destructor
KGGZUsers::~KGGZUsers()
{
	delete m_menu_assign;
}

// add a player to the list
void KGGZUsers::add(QString name)
{
	KListViewItem *tmp;

	tmp = new KListViewItem(itemmain, name);
	lag(tmp, 1);

	credit(name, creditauto);
}

// set player's lag
void KGGZUsers::lag(QListViewItem *item, int lag)
{
	item->setPixmap(2, QPixmap(QString(KGGZ_DIRECTORY "/images/icons/players/lag%1.png").arg(lag)));
}

// remove a player from the list
void KGGZUsers::remove(QString name)
{
	QListViewItem *tmp;

	tmp = findItem(name, 0);
	if(tmp) delete tmp;
}

// remove all players from the list
void KGGZUsers::removeall()
{
	clear();

	itemmain = new QListViewItem(this, i18n("Not playing"));
	itemmain->setOpen(TRUE);
}

void KGGZUsers::addTable(int i)
{
	KListViewItem *tmp;
	QString tablename;

	tablename = i18n("Table: %1").arg(i);
	tmp = new KListViewItem(this, tablename);
	tmp->setOpen(TRUE);
}

void KGGZUsers::addTablePlayer(int i, QString name)
{
	QListViewItem *tmp, *tmp2;

	remove(name);

	tmp2 = table(i);
	if(!tmp2) return;

	tmp = new QListViewItem(tmp2, name);
	credit(name, creditauto);
}

QListViewItem *KGGZUsers::table(int i)
{
	QString tablename = i18n("Table: %1").arg(i);
	return findItem(tablename, 0);
}

// Returns the item which represents the wanted player
QListViewItem *KGGZUsers::player(QString player)
{
	return findItem(player, 0);
}

// Click on a player
void KGGZUsers::slotClicked(QListViewItem *item, const QPoint& point, int column)
{
	if(!item) return;
	if(!item->parent()) return;

	if(m_menu) delete m_menu;
	m_menu = new QPopupMenu(this);

	if(item->text(0) != m_self)
	{
		m_menu->insertItem(i18n("Send private message"), infomessage);
		m_menu->insertItem(i18n("Assign a credit"), m_menu_assign);
	}
	m_menu->insertItem(i18n("Player information"), m_menu_info);

	connect(m_menu, SIGNAL(activated(int)), SLOT(slotContext(int)));

	m_menu->popup(point);
}

// Assign credit to player
void KGGZUsers::slotCredited(int id)
{
	QListViewItem *tmp;

	tmp = selectedItem();
	if(!tmp) return;

	credit(tmp->text(0), id);
}

void KGGZUsers::assignSelf(QString self)
{
	m_self = self;
	credit(self, credityou);
}

void KGGZUsers::display(QString playername)
{
	QPixmap rolepixmap, creditpixmap, playerpixmap;
	QString pixmap;
	QListViewItem *item;
	int role, credit;

	item = player(playername);
	if(!item) return;

	credit = m_credits[playername];
	role = m_roles[playername];

	switch(role)
	{
		case assignplayer:
			pixmap = "player.png";
			break;
		case assignguest:
			pixmap = "guest.png";
			break;
		case assignbot:
			pixmap = "botgrubby.png";
			break;
		case assignadmin:
			pixmap = "admin.png";
			break;
	}

	rolepixmap = QPixmap(KGGZ_DIRECTORY "/images/icons/players/" + pixmap);

	switch(credit)
	{
		case creditbuddy:
			pixmap = "buddy.png";
			break;
		case creditbanned:
			pixmap = "banned.png";
			break;
		case credityou:
			pixmap = "you.png";
			break;
	}

	creditpixmap = QPixmap(KGGZ_DIRECTORY "/images/icons/players/" + pixmap);

	playerpixmap = composite(rolepixmap, creditpixmap);
	item->setPixmap(1, playerpixmap);
}

QPixmap KGGZUsers::composite(QPixmap bottom, QPixmap top)
{
	QPixmap comp;

	QImage topim = top.convertToImage();
	QImage bottomim = bottom.convertToImage();

	for(int j = 0; j < bottom.height(); j++)
		for(int i = 0; i < bottom.width(); i++)
		{
			if(qAlpha(topim.pixel(i, j)))
				bottomim.setPixel(i, j, topim.pixel(i, j));
		}
	comp.convertFromImage(bottomim);
	return comp;
}

void KGGZUsers::credit(QString playername, int credit)
{
	GGZCoreConfio *config;
	int save;

	save = 1;
	if(credit == creditauto)
	{
		save = 0;
		config = new GGZCoreConfio(QDir::home().path() + "/.ggz/kggz.rc", GGZCoreConfio::readonly);
		credit = config->read("Credits", playername.latin1(), creditnone);
		delete config;
	}
	else if(credit != credityou)
	{
		config = new GGZCoreConfio(QDir::home().path() + "/.ggz/kggz.rc", GGZCoreConfio::readwrite | GGZCoreConfio::create);
		config->write("Credits", playername.latin1(), credit);
		config->commit();
		delete config;
	}

	m_credits[playername] = credit;
	display(playername);
}

void KGGZUsers::assignRole(QString playername, int role)
{
	m_roles[playername] = role;
	display(playername);
}

// Set a player's lag
void KGGZUsers::setLag(QString playername, int lagvalue)
{
	if(lagvalue < 0) lagvalue = 0;
	if(lagvalue > 5) lagvalue = 5;
	lag(player(playername), lagvalue);
}

void KGGZUsers::slotInformation(int id)
{
	QListViewItem *tmp;
	int wins, losses, ties, forfeits;
	int rating, ranking, highscore;
	QString playername, text;
	GGZCorePlayer *player, *tmpplayer;

	if(!m_room) return;

	tmp = selectedItem();
	if(!tmp) return;

	playername = tmp->text(0);
	player = NULL;
	for(int i = 0; i < m_room->countPlayers(); i++)
	{
		tmpplayer = m_room->player(i);
		if(playername == tmpplayer->name())
		{
			player = tmpplayer;
			break;
		}
	}
	if(!player) return;

	wins = player->recordWins();
	losses = player->recordLosses();
	ties = player->recordTies();
	forfeits = player->recordForfeits();

	highscore = player->highscore();
	ranking = player->ranking();
	rating = player->rating();

	text = i18n("Information about %1:\n").arg(playername);
	text = text + i18n("Wins: %1\nLosses: %2\n").arg(wins).arg(losses);
	text = text + i18n("Ties: %1\nForfeits: %2\n").arg(ties).arg(forfeits);
	if(rating)
		text = text + i18n("Rating: %1\n").arg(rating);
	if(ranking)
		text = text + i18n("Ranking: %1\n").arg(ranking);
	if(highscore)
		text = text + i18n("Highscore: %1\n").arg(highscore);

	KMessageBox::information(this, text, i18n("Player information"));
}

void KGGZUsers::setRoom(GGZCoreRoom *room)
{
	m_room = room;
}

void KGGZUsers::slotContext(int id)
{
	QString player, message;
	QListViewItem *tmp;

	tmp = selectedItem();
	if(!tmp) return;

	player = tmp->text(0);

	if(id == infomessage)
	{
		message = KInputDialog::getText(i18n("Private message"), i18n("Message text"));
		if(!message.isNull()) emit signalChat(message, player, KGGZChat::RECEIVE_PERSONAL);
	}
}

