/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//    KGGZ - The KDE client for the GGZ Gaming Zone - Version 0.0.4                //
//    Copyright (C) 2000, 2001 Josef Spillner - dr_maux@users.sourceforge.net      //
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

// KDE includes
#include <klocale.h>

// System includes
#include <string.h>
#include <stdio.h>

// static members
//QListViewItem *KGGZUsers::itemmain;

// Constructor
KGGZUsers::KGGZUsers(QWidget *parent = NULL, char *name = NULL)
: QListView(parent, name)
{
	itemmain = new QListViewItem(this, i18n("Not playing"));
	itemmain->setOpen(TRUE);

	addColumn(i18n("Players"));
	addColumn(i18n("Table"));
	insertItem(itemmain);

	setRootIsDecorated(TRUE);
}

// Destructor
KGGZUsers::~KGGZUsers()
{
}

// add a player to the list
void KGGZUsers::add(char *name)
{
	QListViewItem *tmp;

	KGGZDEBUG("USER CONTROL: ===========> add player %s\n", name);
	tmp = new QListViewItem(itemmain, name);
	itemmain->insertItem(tmp);
	KGGZDEBUG("Added token %s\n", name);
}

// remove a player from the list
void KGGZUsers::remove(char *name)
{
	QListViewItem *tmp;
	const char *tokentmp;

	KGGZDEBUG("USER CONTROL: ===========> remove player %s\n", name);
	KGGZDEBUG("It must be %s!\n", name);
	if(itemmain->firstChild())
	{
		KGGZDEBUG("(1) ==> %s\n", itemmain->firstChild()->text(0).latin1());
		tmp = itemmain->firstChild(); //!
	}
	else
	{
		KGGZDEBUG("Error while removing %s!\n", name);
		return;
	}

	while(tmp)
	{
		tokentmp = tmp->text(0).latin1();
		KGGZDEBUG("Is it %s?\n", tokentmp);
		if(strcmp(tokentmp, name) == 0)
		{
			KGGZDEBUG("Yes it is %s, removing...!\n", tokentmp);
			itemmain->takeItem(tmp);
			tmp = NULL;
		}
		else
		{
			KGGZDEBUG("Nope. Next please!\n");
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

	KGGZDEBUG("USER CONTROL: ===========> remove all\n");
	if(!itemmain) return;
	tmp = NULL;

	KGGZDEBUG("Remove all players from the list\n");

	if(itemmain->firstChild())
	{
		KGGZDEBUG("(1) |==> %s\n", itemmain->firstChild()->text(0).latin1());
		tmp = itemmain->firstChild(); //!
	}

	while(tmp)
	{
		KGGZDEBUG("removeall: %s\n", tmp->text(0).latin1());
		itemmain->takeItem(tmp);
		tmp = itemmain->firstChild(); //!
		//tmp = tmp->itemBelow();
	}

	KGGZDEBUG("Remove all tables from the list\n");
	if(firstChild()) tmp = firstChild();
	else tmp = NULL;
	while(tmp)
	{
		takeItem(tmp);
		tmp = firstChild();
	}
	itemmain = new QListViewItem(this, i18n("Not playing"));
	insertItem(itemmain);
	itemmain->setOpen(TRUE);
}


void KGGZUsers::addTable(int i)
{
	//char foo[128];
	QListViewItem *tmp;
	QString foo;

	KGGZDEBUG("USER CONTROL: ===========> add table %i\n", i);
	KGGZDEBUG("KGGZUsers::addTable(%i)\n", i);
	foo.sprintf("Table: %i", i);
	tmp = new QListViewItem(this, foo);
	insertItem(tmp);
	tmp->setOpen(TRUE);
}

void KGGZUsers::addTablePlayer(int i, char *name)
{
	QListViewItem *tmp, *tmp2;
	//char foo[128];
	QString foo;

	KGGZDEBUG("USER CONTROL: ===========> add player %s into table %i\n", name, i);
	KGGZDEBUG("KGGZUsers::addTablePlayer(%i, %s)\n", i, name);
	foo.sprintf("%s-%i", name, i);
	remove(name);

	tmp2 = table(i);
	if(!tmp2)
	{
		KGGZDEBUG("Player %sshould go to table %i; however, it's absent!\n");
		return;
	}
	KGGZDEBUG("Create new player item!\n");
	tmp = new QListViewItem(tmp2, name);
	tmp2->insertItem(tmp);
}

QListViewItem *KGGZUsers::table(int i)
{
	QListViewItem *tmp;
	//char foo[128];
	QString foo;

	KGGZDEBUG("KGGZUsers::table(%i)\n", i);
	tmp = firstChild();
	if(!tmp)
	{
		KGGZDEBUG("Error while searching table %i!\n", i);
		return NULL;
	}

	KGGZDEBUG("Scanning table...\n");
	foo.sprintf("Table: %i", i);
	KGGZDEBUG("Compare: %s\n", tmp->text(0).latin1());
	KGGZDEBUG("To: %s\n", foo.latin1());
	while(tmp)
	{
		if(tmp->text(0) == foo) return tmp;
		KGGZDEBUG("Now or never: get next item!\n");
		if(tmp == NULL) KGGZDEBUG("ALERT!!!! isNull()!!!\n");
		//tmp = tmp->itemBelow();
		tmp = tmp->nextSibling();
		KGGZDEBUG("Nope, that's a no-op\n");
		if(tmp) KGGZDEBUG("This one is new: %s\n", tmp->text(0).latin1());
	}
	KGGZDEBUG("Hm, none found :(\n");
	return NULL;
}
