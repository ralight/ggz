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

// GGZCore++ includes
#include "GGZCoreConfio.h"
#include "GGZCoreRoom.h"
#include "GGZCorePlayer.h"

// KDE includes
#include <klocale.h>
#include <kmessagebox.h>

// Qt includes
#include <qpixmap.h>
#include <qpopupmenu.h>

// System includes
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Constructor
KGGZUsers::KGGZUsers(QWidget *parent, const char *name)
: KListView(parent, name)
{
	itemmain = new QListViewItem(this, i18n("Not playing"));
	itemmain->setOpen(TRUE);

	addColumn(i18n("Players"));
	addColumn(i18n("Table"));
	addColumn(i18n("Lag"));
	insertItem(itemmain);

	setRootIsDecorated(TRUE);

	m_room = NULL;
	m_menu = NULL;

	m_menu_assign = new QPopupMenu(NULL);
	m_menu_assign->insertItem(QIconSet(QPixmap(KGGZ_DIRECTORY "/images/icons/players/player.png")), i18n("Player"), assignplayer);
	m_menu_assign->insertItem(QIconSet(QPixmap(KGGZ_DIRECTORY "/images/icons/players/buddy.png")), i18n("Buddy"), assignbuddy);
	m_menu_assign->insertItem(QIconSet(QPixmap(KGGZ_DIRECTORY "/images/icons/players/banned.png")), i18n("Banned"), assignbanned);
	m_menu_assign->insertItem(QIconSet(QPixmap(KGGZ_DIRECTORY "/images/icons/players/bot.png")), i18n("Bot"), assignbot);

	m_menu_info = new QPopupMenu(NULL);
	m_menu_info->insertItem(i18n("Record"), inforecord);

	connect(this, SIGNAL(rightButtonPressed(QListViewItem*, const QPoint&, int)), SLOT(slotClicked(QListViewItem*, const QPoint&, int)));
	connect(m_menu_assign, SIGNAL(activated(int)), SLOT(slotAssigned(int)));
	connect(m_menu_info, SIGNAL(activated(int)), SLOT(slotInformation(int)));

	//startTimer(1000);
}

// Destructor
KGGZUsers::~KGGZUsers()
{
	delete m_menu_assign;
}

// add a player to the list
void KGGZUsers::add(const char *name)
{
	KListViewItem *tmp;

	tmp = new KListViewItem(itemmain, name);
	itemmain->insertItem(tmp);
	assign(tmp, -1);
	lag(tmp, 1);
}

// set player's lag
void KGGZUsers::lag(QListViewItem *item, int lag)
{
	item->setPixmap(2, QPixmap(QString(KGGZ_DIRECTORY "/images/icons/players/lag%1.png").arg(lag)));
}

// remove a player from the list
void KGGZUsers::remove(const char *name)
{
	QListViewItem *tmp;
	const char *tokentmp;

	if(itemmain->firstChild()) tmp = itemmain->firstChild();
	else
	{
		KGGZDEBUG("Error while removing %s!\n", name);
		return;
	}

	while(tmp)
	{
		tokentmp = tmp->text(0).latin1();
		if(strcmp(tokentmp, name) == 0)
		{
			delete tmp;
			tmp = NULL;
		}
		else
		{
			if(tmp->itemBelow() == NULL) KGGZDEBUG("itemBelow is NULL!\n");
			if(tmp->itemAbove() == NULL) KGGZDEBUG("itemAbove is NULL!\n");
			if(tmp->nextSibling() == NULL) KGGZDEBUG("nextSibling is NULL!\n");
			tmp = tmp->itemBelow();
		}
	}
	KGGZDEBUG("Removed token %s\n", name);
}

// remove all players from the list
void KGGZUsers::removeall()
{
	QListViewItem *tmp;

	if(!itemmain) return;
	tmp = NULL;

	KGGZDEBUG("Remove all players from the list\n");

	if(itemmain->firstChild()) tmp = itemmain->firstChild();

	while(tmp)
	{
		KGGZDEBUG("removeall: %s\n", tmp->text(0).latin1());
		delete tmp;
		tmp = itemmain->firstChild();
	}

	if(firstChild()) tmp = firstChild();
	else tmp = NULL;
	while(tmp)
	{
		delete tmp;
		//takeItem(tmp);
		tmp = firstChild();
	}
	itemmain = new QListViewItem(this, i18n("Not playing"));
	insertItem(itemmain);
	itemmain->setOpen(TRUE);
}

void KGGZUsers::addTable(int i)
{
	KListViewItem *tmp;
	QString foo;

	foo = i18n("Table: %1").arg(i);
	tmp = new KListViewItem(this, foo);
	insertItem(tmp);
	tmp->setOpen(TRUE);
}

void KGGZUsers::addTablePlayer(int i, const char *name)
{
	QListViewItem *tmp, *tmp2;
	QString foo;

	foo.sprintf("%s-%i", name, i);
	remove(name);

	tmp2 = table(i);
	if(!tmp2)
	{
		KGGZDEBUG("Player %s should go to table %i; however, it's absent!\n");
		return;
	}
	tmp = new QListViewItem(tmp2, name);
	tmp2->insertItem(tmp);
	assign(tmp, -1);
}

QListViewItem *KGGZUsers::table(int i)
{
	QListViewItem *tmp;
	QString foo;

	tmp = firstChild();
	if(!tmp)
	{
		KGGZDEBUG("Error while searching table %i!\n", i);
		return NULL;
	}

	foo.sprintf("Table: %i", i);
	while(tmp)
	{
		if(tmp->text(0) == foo) return tmp;
		if(tmp == NULL) KGGZDEBUG("ALERT!!!! isNull()!!!\n");
		tmp = tmp->nextSibling();
		if(tmp) KGGZDEBUG("This one is new: %s\n", tmp->text(0).latin1());
	}
	return NULL;
}

// Returns the item which represents the wanted player
QListViewItem *KGGZUsers::player(const char *player)
{
	QListViewItem *tmp;

	if(!player) return NULL;
	tmp = firstChild();
	while(tmp)
	{
		if(tmp->text(0) == player) return tmp;
		tmp = tmp->itemBelow();
	}
	return NULL;
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
		m_menu->insertItem(i18n("Send private message"), -1);
		m_menu->insertItem(i18n("Assign a role"), m_menu_assign);
	}
	m_menu->insertItem(i18n("Player information"), m_menu_info);
	//connect(m_menu, SIGNAL(activated(int)), SLOT(slotAssigned(int)));

	m_menu->popup(point);
}

// Assign role to player
void KGGZUsers::slotAssigned(int id)
{
	QListViewItem *tmp;

	tmp = selectedItem();
	if(!tmp) return;

	assign(tmp, id);
}

void KGGZUsers::assignSelf(QString self)
{
	m_self = self;
	assign(player(self.latin1()), assignyou);
}

void KGGZUsers::assign(QListViewItem *item, int role)
{
	QString pixmap;
	GGZCoreConfio *config;
	int save;

	if(!item) return;

	save = 1;
	if(role == -1)
	{
		save = 0;
		config = new GGZCoreConfio(QString("%1/.ggz/kggz.rc").arg(getenv("HOME")), GGZCoreConfio::readonly);
		role = config->read("Assignments", item->text(0).latin1(), assignplayer);
		delete config;
	}

	switch(role)
	{
		case assignplayer:
			pixmap = "player.png";
			break;
		case assignbuddy:
			pixmap = "buddy.png";
			break;
		case assignbanned:
			pixmap = "banned.png";
			break;
		case assignbot:
			pixmap = "bot.png";
			break;
		case assignyou:
			pixmap = "you.png";
			break;
		case assignadmin:
			pixmap = "admin.png";
			break;
	}
	item->setPixmap(1, QPixmap(KGGZ_DIRECTORY "/images/icons/players/" + pixmap));

	if(save)
	{
		config = new GGZCoreConfio(QString("%1/.ggz/kggz.rc").arg(getenv("HOME")), GGZCoreConfio::readwrite | GGZCoreConfio::create);
		config->write("Assignments", item->text(0).latin1(), role);
		config->commit();
		delete config;
	}
}

// Set a player's lag
void KGGZUsers::setLag(const char *playername, int lagvalue)
{
	if(lagvalue < 0) lagvalue = 0;
	if(lagvalue > 5) lagvalue = 5;
	lag(player(playername), lagvalue);
}

void KGGZUsers::assignRole(const char *playername, int role)
{
	assign(player(playername), role);
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
		text = text + i18n("Rating: %1\nRanking: %2\nHighscore: %3\n").arg(rating).arg(ranking).arg(highscore);

	KMessageBox::information(this, text, i18n("Player information"));
}

void KGGZUsers::setRoom(GGZCoreRoom *room)
{
	m_room = room;
}

