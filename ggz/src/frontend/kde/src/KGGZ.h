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
// KGGZ: The main GGZ Gaming Zone object. Controls servers, rooms and games/rooms. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

#ifndef KGGZ_H
#define KGGZ_H

// KGGZ includes
#include "KGGZCommon.h"
#include "KGGZCallback.h"
#include "KGGZLaunch.h"
#include "KGGZConnect.h"
#include "KGGZMotd.h"
#include "KGGZGameInfo.h"

// KDE includes
#include <kpopupmenu.h>

// Qt includes
#include <qevent.h>
#include <qwidget.h>
#include <qdns.h>

// GGZCore++ includes
#include "GGZCore.h"
#include "GGZCoreConf.h"
#include "GGZCoreServer.h"
#include "GGZCoreRoom.h"
#include "GGZCoreGame.h"
#include "GGZCoreGametype.h"
#include "GGZCoreModule.h"
#include "GGZCoreTable.h"

// Forward declarations
class KGGZWorkspace;
class KGGZSplash;
#ifdef KGGZ_BROWSER
class KGGZBrowser;
#endif
class KGGZGrubby;
class KGGZPrefEnv;
class KGGZSelector;
class KGGZGameInfoDialog;
class KGGZTeam;
class QSocketNotifier;

// Here comes KGGZ.
class KGGZ : public QWidget
{
	Q_OBJECT
	public:
		// Constructor
		KGGZ(QWidget *parent = NULL, const char *name = NULL);
		// Destructor
		~KGGZ();

		// Receive events of these types
		enum Collectortypes
		{
			COLLECTOR_ROOM,
			COLLECTOR_SERVER,
			COLLECTOR_GAME
		};

		// Pre-configures view modes
		enum Viewtypes
		{
			VIEW_CHAT,
			VIEW_USERS,
			VIEW_TABLES,
			VIEW_SPLASH,
			VIEW_BROWSER
		};

		// Signals received from the KGGZBase object
		enum Menusignals
		{
			MENUSIG_DISCONNECT,
			MENUSIG_ROOMLIST,
			MENUSIG_LOGIN,
			MENUSIG_SERVERSTART,
			MENUSIG_SERVERSTOP,
			MENUSIG_ROOMENTER,
			MENUSIG_GAMESTART,
			MENUSIG_GAMEOVER
		};

	signals:
		// Emitted to dis/enable menu items
		void signalMenu(int signal);
		// Emitted to announce new room
		void signalRoom(const char *roomname, const char *category);
		// Emitted to change window caption
		void signalCaption(const char *caption);
		// Emitted if connection state changes
		void signalState(int state);
		// Emitted if room changes
		void signalLocation(const char *location);

	public slots:
		// Connect to a game server
		void menuConnect();
		// Disconnect again
		void menuDisconnect();
		// Display MOTD
		void menuMotd();
		// Run a server on localhost
		void menuServerLaunch();
		// Shutdown local server
		void menuServerKill();
		// Change the view of the workspace
		void menuView(int viewtype);
		// Launch a game at a table
		void menuGameLaunch();
		// Join a running game
		void menuGameJoin();
		// Join a game as spectator
		void menuGameSpectator();
		// Display game type information
		void menuGameInfo();
		// Change into the given room
		void menuRoom(int room);
		// Display the grubby dialog
		void menuGrubby();
		// Raise a configuration window
		void menuPreferencesSettings();
		// Cancel a running game
		void menuGameCancel();
		// Invoke the team dialog
		void menuGameTeam();

		// Receive connection parameters
		void slotConnected(const char *host, int port, const char *username, const char *password, int mode);
		// Actually start the connection with the pre-resolved host
		void slotConnectedStart();
		// Receive chat message
		void slotChat(const char *text, char *player, int mode);
		// Send grubby requests
		void slotGrubby(const char *grubby, const char *argument, int id);
		// Prepares a game launch
		void slotGamePrepare(int frontend);
		// Launch the game
		void slotGameStart();

		// Game data
		void slotGameData();
		// Server data
		void slotServerData();

	protected:
		// Handle resizing
		void resizeEvent(QResizeEvent *e);
		// Timer for slightly delayed input from sockets
		void timerEvent(QTimerEvent *e);

		// catch server atoms
		void serverCollector(unsigned int id, void* data);
		// catch room atoms
		void roomCollector(unsigned int id, void* data);
		// catch game atoms
		void gameCollector(unsigned int id, void* data);
		// catch all types of atoms and distribute them
		static GGZHookReturn hookOpenCollector(unsigned int id, void* event_data, void* user_data);

	private:
		// ensure that KGGZ is in a proper state
		void dispatcher();

		// set up room callbacks
		void attachRoomCallbacks();
		// remove room callbacks
		void detachRoomCallbacks();
		// setup callbacks for server events
		void attachServerCallbacks();
		// remove server callbacks
		void detachServerCallbacks();
		// setup hooks for game events
		void attachGameCallbacks();
		// remove game callbacks
		void detachGameCallbacks();

		// cancel a game
		void eventLeaveGame();
		// exit a table
		void eventLeaveTable(int force);
		// leave a room
		void eventLeaveRoom();

		// attempt to list all players
		void listPlayers();
		// display lag of all players
		void lagPlayers();
		// attempt to list active tables
		void listTables();

		// Launches a game of the specified game type
		//void slotLaunchGame(GGZCoreGametype *gametype);
		void slotGameFire();
		void slotGameFrontend();
		// Advices logo widget to load game logo
		void slotLoadLogo();

		// The commonly used workspace
		KGGZWorkspace *m_workspace;
		// GGZCore++ server object (unique)
		GGZCoreServer *kggzserver;
		// GGZCore++ room object (unique)
		GGZCoreRoom *kggzroom;
		// GGZCore++ game object (unique)
		GGZCoreGame *kggzgame;
		// GGZCore++ core object
		GGZCore *m_core;
		// GGZCore++ configuration object
		GGZCoreConf *m_config;
		// Saved gametype on frontend selection
		GGZCoreModule *m_module;
		// Saved table for frontend selection
		GGZCoreTable *m_table;
		// Callback atoms (all unique)
		KGGZCallback *kggzservercallback, *kggzroomcallback, *kggzgamecallback;
		// Game information
		KGGZGameInfo *m_gameinfo;
		// User data saved from connection dialog
		char *m_save_username, *m_save_password, *m_save_host;
		// Connection data saved from connection dialog
		int m_save_loginmode, m_save_port, m_save_encryption;
		// Special flag for failed negotiations and logout
		int m_killserver;

		// Launch dialog
		KGGZLaunch *m_launch;
		// Connection dialog
		KGGZConnect *m_connect;
		// Splash screen
		KGGZSplash *m_splash;
		//KPopupMenu *m_menu_client, *m_menu_rooms, *m_menu_ggz, *m_menu_game, *m_menu_preferences;
#ifdef KGGZ_BROWSER
		// Embedded HTML browser
		KGGZBrowser *m_browser;
#endif
		// Message of the day widget
		KGGZMotd *m_motd;
		// Grubby dialog
		KGGZGrubby *m_grubby;
		// Configuration
		KGGZPrefEnv *m_prefenv;
		// Frontend selection
		KGGZSelector *m_selector;
		// Game information
		KGGZGameInfoDialog *m_gameinfodialog;
		// Team dialog
		KGGZTeam *m_team;

		// Non-blocking DNS for easysock's gethostbyname
		QDns *m_dns;
		// File descriptor of current channel
		int m_channelfd;
		// Current game fd
		int m_gamefd;
		// Display MOTD upon login
		int m_showmotd;
		// Game socket notifier
		QSocketNotifier *m_sn_game;
		// Server socket notifier
		QSocketNotifier *m_sn_server;
};

#endif

