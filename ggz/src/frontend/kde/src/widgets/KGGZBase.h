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
// KGGZBase: handle the main window, status and menu bar. This is the application. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

#ifndef KGGZ_BASE_H
#define KGGZ_BASE_H

// KGGZ includes
#include "KGGZ.h"
#include "KGGZAbout.h"

// KDE includes
#include <kmainwindow.h>

// Qt includes
#include <qiconset.h>

// Forward declarations
class KPopupMenu;
class KSystemTray;

// This class represents the visible main window of KGGZ.
class KGGZBase : public KMainWindow
{
	Q_OBJECT
	public:
		// Constructor
		KGGZBase();
		// Destructor
		~KGGZBase();
		// Connect to an URI
		void autoconnect(QString uri);

	public slots:
		// Handle a specific menu item activated by the user
		void slotMenu(int id);
		// Create the "About GGZ" window and show it
		void slotAboutGGZ();
		// Receive a dis/enable event from the KGGZ object
		void slotMenuSignal(int signal);
		// Receive a room to be added to the menu
		void slotRoom(const char *roomname, const char *category);
		// Receive the caption dynamically from KGGZ
		void slotCaption(const char *caption);
		// Display the state in the status bar
		void slotState(int state);
		// Show room or outside of room message
		void slotLocation(const char *location);
		// Activity
		void slotActivity(int activity);

	private:
		// assign an icon to a menu item
		QIconSet kggzGetIcon(int menuid);

		// All menu items visible in the main window's menu bar
		enum MenuItems
		{
			MENU_GGZ_TEST,
			MENU_GGZ_CONNECT,
			MENU_GGZ_DISCONNECT,
			MENU_GGZ_MOTD,
			MENU_GGZ_WATCHER,
			MENU_GGZ_STARTSERVER,
			MENU_GGZ_STOPSERVER,
			MENU_GGZ_QUIT,

			MENU_CLIENT_STARTUP,
			MENU_CLIENT_CHAT,
			MENU_CLIENT_TABLES,
			MENU_CLIENT_PLAYERS,
			MENU_CLIENT_HELP,

			MENU_GAME_INFO,
			MENU_GAME_LAUNCH,
			MENU_GAME_JOIN,
			MENU_GAME_SPECTATOR,
			MENU_GAME_UPDATE,
			MENU_GAME_GRUBBY,
			MENU_GAME_NEWS,
			MENU_GAME_TEAM,
			MENU_GAME_CANCEL,

			MENU_PREFERENCES_SETTINGS,
			MENU_PREFERENCES_PLAYERINFO,
			MENU_PREFERENCES_HOSTS,
			MENU_PREFERENCES_FTP,
			MENU_PREFERENCES_GAMES,
			MENU_PREFERENCES_THEMES,
			MENU_PREFERENCES_PREFERENCES,

			MENU_ROOMS_SLOTS // MENU_ROOMS_SLOTS _must_ be the last element
		};

		// All menus, some of them are created dynamically
		enum Menus
		{
			MENU_GGZ,
			MENU_CLIENT,
			MENU_ROOMS,
			MENU_GAME,
			MENU_PREFERENCES,
			MENU_HELP
		};

		// The KGGZ object
		KGGZ *kggz;
		// "About GGZ" dialog
		KGGZAbout *m_about;
		// All available menus (except help)
		KPopupMenu *m_menu_client, *m_menu_rooms, *m_menu_ggz, *m_menu_game, *m_menu_preferences;
		// The number of available rooms on a server
		int m_rooms;
		// System tray watcher
		KSystemTray *tray;
		// The current activity status
		int m_activity;
};

#endif

