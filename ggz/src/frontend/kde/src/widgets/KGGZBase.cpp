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
// KGGZBase: handle the main window, status and menu bar. This is the application. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

// Header file
#include "KGGZBase.h"

// KGGZ includes
#include "KGGZCommon.h"

// KDE includes
#include <klocale.h>
#include <kmessagebox.h>
#include <ksimpleconfig.h>

KGGZBase::KGGZBase(char *name)
: KTMainWindow(name)
{
	KPopupMenu *menu_help;
	int x, y, width, height;

	KGGZDEBUGF("KGGZBase::KGGZBase()\n");

	m_about = NULL;
	m_rooms = 0;

	konfig = new KSimpleConfig("kggz");
	konfig->setGroup("kggzbase");
	x = konfig->readNumEntry("x");
	y = konfig->readNumEntry("y");
	width = konfig->readNumEntry("width");
	height = konfig->readNumEntry("height");

	m_menu = new KMenuBar(this);
	enableStatusBar();
	statusBar()->insertItem(i18n("  Not connected  "), 1);
	statusBar()->insertItem(i18n("  Loading...  "), 2);

        kggz = new KGGZ(this, "kggz");

 	m_menu_ggz = new KPopupMenu(this, "menu_ggz");
  	m_menu_ggz->insertItem(kggzGetIcon(MENU_GGZ_CONNECT), i18n("&Connect"), MENU_GGZ_CONNECT);
  	m_menu_ggz->insertItem(kggzGetIcon(MENU_GGZ_DISCONNECT), i18n("&Disconnect"), MENU_GGZ_DISCONNECT);
	m_menu_ggz->insertSeparator();
	m_menu_ggz->insertItem(kggzGetIcon(MENU_GGZ_STARTSERVER), i18n("Start server"), MENU_GGZ_STARTSERVER);
	m_menu_ggz->insertItem(kggzGetIcon(MENU_GGZ_STOPSERVER), i18n("Stop server"), MENU_GGZ_STOPSERVER);
	m_menu_ggz->insertSeparator();
	m_menu_ggz->insertItem(kggzGetIcon(MENU_GGZ_QUIT), i18n("&Quit"), MENU_GGZ_QUIT);
	m_menu_ggz->setItemEnabled(MENU_GGZ_DISCONNECT, FALSE);
	if(KGGZCommon::findProcess("ggzd") > 0) m_menu_ggz->setItemEnabled(MENU_GGZ_STARTSERVER, FALSE);
	else m_menu_ggz->setItemEnabled(MENU_GGZ_STOPSERVER, FALSE);

	m_menu_client = new KPopupMenu(this, "menu_client");
	m_menu_client->insertItem(kggzGetIcon(MENU_CLIENT_STARTUP), i18n("&Startup Screen"), MENU_CLIENT_STARTUP);
	m_menu_client->insertItem(kggzGetIcon(MENU_CLIENT_CHAT), i18n("&Chat"), MENU_CLIENT_CHAT);
	m_menu_client->insertItem(kggzGetIcon(MENU_CLIENT_TABLES), i18n("&Available tables"), MENU_CLIENT_TABLES);
	m_menu_client->insertItem(kggzGetIcon(MENU_CLIENT_PLAYERS), i18n("&List of players"), MENU_CLIENT_PLAYERS);
#ifdef KGGZ_BROWSER
	m_menu_client->insertItem(kggzGetIcon(MENU_CLIENT_HELP), i18n("&Web Browser"), MENU_CLIENT_HELP);
#endif
	m_menu_client->setItemEnabled(MENU_CLIENT_CHAT, FALSE);
	m_menu_client->setItemEnabled(MENU_CLIENT_TABLES, FALSE);
	m_menu_client->setItemEnabled(MENU_CLIENT_PLAYERS, FALSE);

 	m_menu_rooms = new KPopupMenu(this, "menu_rooms");
	m_menu_rooms->setEnabled(FALSE);

	m_menu_game = new KPopupMenu(this, "menu_games");
	m_menu_game->insertItem(kggzGetIcon(MENU_GAME_INFO), i18n("&Information"), MENU_GAME_INFO);
	m_menu_game->insertSeparator();
	m_menu_game->insertItem(kggzGetIcon(MENU_GAME_LAUNCH), i18n("&Launch new game"), MENU_GAME_LAUNCH);
	m_menu_game->insertItem(kggzGetIcon(MENU_GAME_JOIN), i18n("&Join game"), MENU_GAME_JOIN);
	//m_menu_game->insertItem(kggzGetIcon(MENU_GAME_UPDATE), i18n("&Update from FTP"), MENU_GAME_UPDATE);
	//m_menu_game->insertItem(kggzGetIcon(MENU_GAME_NEWS), i18n("&GGZ News"), MENU_GAME_NEWS);
	m_menu_game->insertSeparator();
	m_menu_game->insertItem(kggzGetIcon(MENU_GAME_GRUBBY), i18n("&Grubby"), MENU_GAME_GRUBBY);
	m_menu_game->setEnabled(FALSE);

	m_menu_preferences = new KPopupMenu(this, "menu_preferences");
	m_menu_preferences->insertItem(kggzGetIcon(MENU_PREFERENCES_SETTINGS), i18n("Se&ttings"), MENU_PREFERENCES_SETTINGS);
	m_menu_preferences->insertItem(kggzGetIcon(MENU_PREFERENCES_PLAYERINFO), i18n("&Player information"), MENU_PREFERENCES_PLAYERINFO);
	//m_menu_preferences->insertItem(kggzGetIcon(MENU_PREFERENCES_HOSTS), i18n("Ga&me servers"), MENU_PREFERENCES_HOSTS);
	//m_menu_preferences->insertItem(kggzGetIcon(MENU_PREFERENCES_FTP), i18n("&FTP servers"), MENU_PREFERENCES_FTP);
	m_menu_preferences->insertItem(kggzGetIcon(MENU_PREFERENCES_GAMES), i18n("&Games"), MENU_PREFERENCES_GAMES);
	//m_menu_preferences->insertItem(kggzGetIcon(MENU_PREFERENCES_THEMES), i18n("Th&emes"), MENU_PREFERENCES_THEMES);
	//m_menu_preferences->insertSeparator();
	//m_menu_preferences->insertItem(kggzGetIcon(MENU_PREFERENCES_PREFERENCES), i18n("&All Preferences"), MENU_PREFERENCES_PREFERENCES);

	menu_help = helpMenu(NULL, FALSE);
	menu_help->insertItem(QPixmap(KGGZ_DIRECTORY "/images/icons/ggz.png"), i18n("About the GGZ Gaming Zone"), 99, 5);
	menu_help->connectItem(99, this, SLOT(slotAboutGGZ()));
	KGGZDEBUG("GGZ Icon at: " KGGZ_DIRECTORY "\n");

	m_menu->insertItem(i18n("GG&Z"), m_menu_ggz);
	m_menu->insertItem(i18n("&Client"), m_menu_client);
	m_menu->insertItem(i18n("&Rooms"), m_menu_rooms);
	m_menu->insertItem(i18n("&Game"), m_menu_game);
	m_menu->insertItem(i18n("&Preferences"), m_menu_preferences);
	m_menu->insertItem(i18n("&Help"), menu_help);

	connect(m_menu_ggz, SIGNAL(activated(int)), SLOT(slotMenu(int)));
	connect(m_menu_client, SIGNAL(activated(int)), SLOT(slotMenu(int)));
	connect(m_menu_rooms, SIGNAL(activated(int)), SLOT(slotMenu(int)));
	connect(m_menu_game, SIGNAL(activated(int)), SLOT(slotMenu(int)));
	connect(kggz, SIGNAL(signalMenu(int)), SLOT(slotMenuSignal(int)));
	connect(kggz, SIGNAL(signalRoom(char*)), SLOT(slotRoom(char*)));
	connect(kggz, SIGNAL(signalCaption(const char*)), SLOT(slotCaption(const char*)));
	connect(kggz, SIGNAL(signalState(int)), SLOT(slotState(int)));

	setView(kggz);
	setCaption("KGGZ - [offline]");
	if(x || x || width || height) setGeometry(x, y, width, height);
	else resize(500, 430);
	show();

	kggz->menuView(KGGZ::VIEW_SPLASH);

	statusBar()->changeItem(i18n("  Ready for connection.  "), 2);

	KGGZDEBUGF("KGGZBase::KGGZBase() done\n");
}

KGGZBase::~KGGZBase()
{
	konfig->writeEntry("x", x());
	konfig->writeEntry("y", y());
	konfig->writeEntry("width", width());
	konfig->writeEntry("height", height());
	delete konfig;
}

QIconSet KGGZBase::kggzGetIcon(int menuid)
{
	char *icon = NULL;
	QIconSet iconset;

	switch(menuid)
	{
		case MENU_GGZ_CONNECT:
			icon = "connect.png";
			break;
		case MENU_GGZ_DISCONNECT:
			icon = "disconnect.png";
			break;
		case MENU_GGZ_STARTSERVER:
			icon = "startserver.png";
			break;
		case MENU_GGZ_STOPSERVER:
			icon = "stopserver.png";
			break;
		case MENU_GGZ_QUIT:
			icon = "quit.png";
			break;
		case MENU_CLIENT_STARTUP:
			icon = "startup.png";
			break;
		case MENU_CLIENT_CHAT:
			icon = "chat.png";
			break;
		case MENU_CLIENT_TABLES:
			icon = "tables.png";
			break;
		case MENU_CLIENT_PLAYERS:
			icon = "players.png";
			break;
		case MENU_CLIENT_HELP:
			icon = "browser.png";
			break;
		case MENU_GAME_INFO:
			icon = "info.png";
			break;
		case MENU_GAME_LAUNCH:
			icon = "launch.png";
			break;
		case MENU_GAME_JOIN:
			icon = "join.png";
			break;
		case MENU_GAME_UPDATE:
			icon = "update.png";
			break;
		case MENU_GAME_GRUBBY:
			icon = "grubby.png";
			break;
		case MENU_PREFERENCES_SETTINGS:
		case MENU_PREFERENCES_PLAYERINFO:
		case MENU_PREFERENCES_HOSTS:
		case MENU_PREFERENCES_FTP:
		case MENU_PREFERENCES_GAMES:
		case MENU_PREFERENCES_THEMES:
			icon = "pref.png";
			break;
		case MENU_PREFERENCES_PREFERENCES:
			icon = "preferences.png";
			break;
		default:
			icon = "unknown.png";
			break;
	}

	if(menuid >= MENU_ROOMS_SLOTS)
	{
		icon = "unknown.png";
	}

	iconset = QIconSet(QPixmap(QString(KGGZ_DIRECTORY "/images/icons/") + QString(icon)));
	return iconset;
}

void KGGZBase::slotAboutGGZ()
{
	KGGZDEBUGF("KGGZBase::slotAboutGGZ()\n");
	KGGZDEBUG("About it... hm, it's pretty cool and open for everyone!\n");
	if(!m_about) m_about = new KGGZAbout(NULL, "KGGZAbout");
	m_about->show();
}

void KGGZBase::slotMenu(int id)
{
	int ret;

	KGGZDEBUGF("KGGZBase::slotMenu()\n");
	KGGZDEBUG("got id: %i\n", id);

	switch(id)
	{
		case MENU_GGZ_CONNECT:
			kggz->menuConnect();
			break;
		case MENU_GGZ_DISCONNECT:
			kggz->menuDisconnect();
			statusBar()->changeItem(i18n("Not connected"), 1);
			break;
		case MENU_GGZ_STARTSERVER:
			kggz->menuServerLaunch();
			break;
		case MENU_GGZ_STOPSERVER:
			kggz->menuServerKill();
			break;
   		case MENU_GGZ_QUIT:
			ret = KMessageBox::questionYesNo(this, "Do you really want to quit?", "Quit KGGZ");
			if(ret == KMessageBox::Yes) close();
			break;
		case MENU_CLIENT_STARTUP:
			kggz->menuView(KGGZ::VIEW_SPLASH);
			break;
		case MENU_CLIENT_CHAT:
			kggz->menuView(KGGZ::VIEW_CHAT);
			break;
		case MENU_CLIENT_TABLES:
			kggz->menuView(KGGZ::VIEW_TABLES);
			break;
		case MENU_CLIENT_PLAYERS:
			kggz->menuView(KGGZ::VIEW_USERS);
			break;
		case MENU_CLIENT_HELP:
			kggz->menuView(KGGZ::VIEW_BROWSER);
			break;
		case MENU_GAME_LAUNCH:
			kggz->menuGameLaunch();
			break;
		case MENU_GAME_JOIN:
			kggz->menuGameJoin();
			break;
		case MENU_GAME_GRUBBY:
			KGGZDEBUG("Aaaaargh.... ;)\n");
			break;
		case MENU_GAME_INFO:
			kggz->menuGameInfo();
			break;
		default:
			kggz->menuView(KGGZ::VIEW_CHAT);
			kggz->menuRoom(id - MENU_ROOMS_SLOTS);
	}
	KGGZDEBUGF("KGGZBase::slotMenu() ready\n");
}

void KGGZBase::slotMenuSignal(int signal)
{
	switch(signal)
	{
		case KGGZ::MENUSIG_DISCONNECT:
			for(int i = 0; i < m_rooms; i++) m_menu_rooms->removeItemAt(0);
			m_rooms = 0;
			m_menu_rooms->setEnabled(FALSE);
			m_menu_ggz->setItemEnabled(MENU_GGZ_CONNECT, TRUE);
			m_menu_ggz->setItemEnabled(MENU_GGZ_DISCONNECT, FALSE);
			m_menu_client->setItemEnabled(MENU_CLIENT_CHAT, FALSE);
			m_menu_client->setItemEnabled(MENU_CLIENT_TABLES, FALSE);
			m_menu_client->setItemEnabled(MENU_CLIENT_PLAYERS, FALSE);
			m_menu_game->setEnabled(FALSE);
			setCaption("KGGZ - [offline]");
			break;
		case KGGZ::MENUSIG_ROOMLIST:
			m_menu_rooms->setEnabled(TRUE);
			m_menu_game->setEnabled(TRUE);
			break;
		case KGGZ::MENUSIG_LOGIN:
			m_menu_client->setItemEnabled(MENU_CLIENT_CHAT, TRUE);
			m_menu_client->setItemEnabled(MENU_CLIENT_TABLES, TRUE);
			m_menu_client->setItemEnabled(MENU_CLIENT_PLAYERS, TRUE);
			m_menu_ggz->setItemEnabled(MENU_GGZ_CONNECT, FALSE);
			m_menu_ggz->setItemEnabled(MENU_GGZ_DISCONNECT, TRUE);
			break;
		case KGGZ::MENUSIG_SERVERSTOP:
			m_menu_ggz->setItemEnabled(MENU_GGZ_STARTSERVER, TRUE);
			m_menu_ggz->setItemEnabled(MENU_GGZ_STOPSERVER, FALSE);
			break;
		case KGGZ::MENUSIG_SERVERSTART:
			m_menu_ggz->setItemEnabled(MENU_GGZ_STARTSERVER, FALSE);
			m_menu_ggz->setItemEnabled(MENU_GGZ_STOPSERVER, TRUE);
			break;
		default:
			KGGZDEBUG("Unknown signal for menu handling: %i!\n", signal);
			break;
	}
}

void KGGZBase::slotRoom(char *roomname)
{
	m_menu_rooms->insertItem(kggzGetIcon(MENU_ROOMS_SLOTS + m_rooms), roomname, MENU_ROOMS_SLOTS + m_rooms);
	m_rooms++;
}

void KGGZBase::slotCaption(const char *caption)
{
	setCaption(caption);
	statusBar()->changeItem(i18n("  Connected  "), 1);
}

void KGGZBase::slotState(int state)
{
	statusBar()->changeItem(i18n("  State: ") + KGGZCommon::state((GGZStateID)state) + "  ", 2);
}
