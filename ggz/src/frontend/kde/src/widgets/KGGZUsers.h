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

#ifndef KGGZ_USERS_H
#define KGGZ_USERS_H

// Qt includes
#include <klistview.h>

// Forward declarations
class QPopupMenu;
class GGZCoreRoom;

// KGGZ_Users: list of all players in current room
class KGGZUsers : public KListView
{
	Q_OBJECT
	public:
		// Constructor
		KGGZUsers(QWidget *parent = NULL, const char *name = NULL);
		// Dectructor
		~KGGZUsers();
		// Add a user to the table
		void add(QString name);
		// Remove user from table
		void remove(QString name);
		// Remove all users
		void removeall();
		// Add a table
		void addTable(int i);
		// Add a player to a table, or move him there
		void addTablePlayer(int i, QString name);
		// Specify the player's name
		void assignSelf(QString self);
		// Assign a role to a player
		void assignRole(QString playername, int role);
		// Set a player's lag
		void setLag(QString playername, int lagvalue);
		// Set current room
		void setRoom(GGZCoreRoom *room);

		// Player credits assigned by user
		enum Credits
		{
			creditnone,
			creditbuddy,
			creditbanned,
			credityou,
			creditauto
		};

		// Player roles assigned by server
		enum Assignments
		{
			assignunknown,
			assignplayer,
			assignguest,
			assignbot,
			assignadmin
		};

		// Possible information actions
		enum Information
		{
			inforecord
		};

	public slots:
		// Click on a player
		void slotClicked(QListViewItem *item, const QPoint& point, int column);
		// Player crediting
		void slotCredited(int id);
		// Get player information
		void slotInformation(int id);

	private:
		// Returns the item which represents the requested table
		QListViewItem *table(int i);
		// Returns the item which represents the wanted player
		QListViewItem *player(QString player);
		// Assign a credit
		void credit(QString playername, int credit);
		// Display icon with role and credit
		void display(QString playername);
		// Set lag
		void lag(QListViewItem *item, int lag);
		// Helper function: pixmap composition
		QPixmap composite(QPixmap bottom, QPixmap top);
		
		// The list displaying the users
		QListViewItem *itemmain;
		// Context menu for players
		QPopupMenu *m_menu;
		// Menu for assignments
		QPopupMenu *m_menu_assign, *m_menu_info;
		// Name of the player
		QString m_self;
		// Player roles
		QMap<QString, int> m_roles;
		// Player credits
		QMap<QString, int> m_credits;

		// Current room
		GGZCoreRoom *m_room;
};

#endif

