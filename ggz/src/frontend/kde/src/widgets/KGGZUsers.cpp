/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// KGGZ - The KDE client for the GGZ Gaming Zone - Version 0.0.4           //
// Copyright (C) 2000, 2001 Josef Spillner - dr_maux@users.sourceforge.net //
// The MindX Open Source Project - http://mindx.sourceforge.net            //
// Published under GNU GPL conditions - view COPYING for details           //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

// Header definition
#include "KGGZUsers.h"

// System includes
#include <string.h>
#include <stdio.h>

// KDE includes
#include <klocale.h>

// Configuration
#include "KGGZWidgetCommon.h"

// static members
//QListViewItem *KGGZUsers::itemmain;

// Constructor
KGGZUsers::KGGZUsers(QWidget *parent, char *name)
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

	tmp = new QListViewItem(itemmain, name);
	itemmain->insertItem(tmp);
	KGGZDEBUG("Added token %s\n", name);
}

// remove a player from the list
void KGGZUsers::remove(char *name)
{
	QListViewItem *tmp;
	const char *tokentmp;

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
	char foo[128];
	QListViewItem *tmp;

	KGGZDEBUG("KGGZUsers::addTable(%i)\n", i);
	sprintf(foo, "Table: %i", i);
	tmp = new QListViewItem(this, foo);
	insertItem(tmp);
	tmp->setOpen(TRUE);
}

void KGGZUsers::addTablePlayer(int i, char *name)
{
	QListViewItem *tmp, *tmp2;
	char foo[128];

	KGGZDEBUG("KGGZUsers::addTablePlayer(%i, %s)\n", i, name);
	sprintf(foo, "%s-%i", name, i);
	remove(name);

	tmp2 = table(i);
	if(!tmp2)
	{
		KGGZDEBUG("Player %sshould go to table %i; however, it's absent!\n");
		return;
	}
	tmp = new QListViewItem(tmp2, name);
	tmp2->insertItem(tmp);
}

QListViewItem *KGGZUsers::table(int i)
{
	QListViewItem *tmp;
	char foo[128];

	KGGZDEBUG("KGGZUsers::table(%i)\n", i);
	tmp = firstChild();
	if(!tmp)
	{
		KGGZDEBUG("Error while searching table %i!\n", i);
		return NULL;
	}

	sprintf(foo, "Table: %i", i);
	while(tmp)
	{
		if(strcmp(tmp->text(0).latin1(), foo) == 0) return tmp;
		tmp = tmp->itemBelow();
	}
	return NULL;
}
