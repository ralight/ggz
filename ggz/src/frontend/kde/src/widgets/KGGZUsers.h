/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// KGGZ - The KDE client for the GGZ Gaming Zone - Version 0.0.4           //
// Copyright (C) 2000, 2001 Josef Spillner - dr_maux@users.sourceforge.net //
// The MindX Open Source Project - http://mindx.sourceforge.net            //
// Published under GNU GPL conditions - view COPYING for details           //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef KGGZ_USERS_H
#define KGGZ_USERS_H

// Qt includes
#include <qlistview.h>

// KGGZ_Users: list of all players in current room
class KGGZUsers : public QListView
{
Q_OBJECT
public:
	// constructor
	KGGZUsers(QWidget *parent, char *name);
	// dectructor
	~KGGZUsers();
	// add a user to the table
	void add(char *name);
	// remove user from table
	void remove(char *name);
	// remove all users
	void removeall();

	void addTable(int i);
	void addTablePlayer(int i, char *name);
private:
	QListViewItem *table(int i);
	// the list displaying the users
	QListViewItem *itemmain;
};

#endif
