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

// Header file
#include "KGGZBase.h"

// KGGZ includes
#include "KGGZCommon.h"

// KDE includes
#include <klocale.h>
#include <kmessagebox.h>
#include <ksimpleconfig.h>
#include <kapp.h>
#include <kpopupmenu.h>
#include <kmenubar.h>

KGGZBase::KGGZBase()
: KMainWindow()
{
	int x, y, width, height;

	m_about = NULL;
	m_rooms = 0;

	KSimpleConfig konfig("kggzrc");
	konfig.setGroup("kggzbase");
	x = konfig.readNumEntry("x");
	y = konfig.readNumEntry("y");
	width = konfig.readNumEntry("width");
	height = konfig.readNumEntry("height");

	statusBar()->insertItem(i18n("  Not connected  "), 1);
	statusBar()->insertItem(i18n("  Loading...  "), 2);
	statusBar()->insertItem(i18n("  No room selected  "), 3);

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

	m_menu_game = new KPopupMenu(this, "menu_games");
	m_menu_game->insertItem(kggzGetIcon(MENU_GAME_INFO), i18n("&Information"), MENU_GAME_INFO);
	m_menu_game->insertSeparator();
	m_menu_game->insertItem(kggzGetIcon(MENU_GAME_LAUNCH), i18n("&Launch new game"), MENU_GAME_LAUNCH);
	m_menu_game->insertItem(kggzGetIcon(MENU_GAME_JOIN), i18n("&Join game"), MENU_GAME_JOIN);
	m_menu_game->insertItem(kggzGetIcon(MENU_GAME_CANCEL), i18n("&Cancel game"), MENU_GAME_CANCEL);
	m_menu_game->insertSeparator();
	m_menu_game->insertItem(kggzGetIcon(MENU_GAME_GRUBBY), i18n("&Grubby"), MENU_GAME_GRUBBY);
	m_menu_game->setItemEnabled(MENU_GAME_CANCEL, FALSE);

	m_menu_preferences = new KPopupMenu(this, "menu_preferences");
	m_menu_preferences->insertItem(kggzGetIcon(MENU_PREFERENCES_SETTINGS), i18n("Se&ttings"), MENU_PREFERENCES_SETTINGS);

	helpMenu()->insertItem(QPixmap(KGGZ_DIRECTORY "/images/icons/ggz.png"), i18n("About the GGZ Gaming Zone"), 99, 5);
	helpMenu()->connectItem(99, this, SLOT(slotAboutGGZ()));

	menuBar()->insertItem(i18n("GG&Z"), m_menu_ggz, MENU_GGZ);
	menuBar()->insertItem(i18n("&Client"), m_menu_client, MENU_CLIENT);
	menuBar()->insertItem(i18n("&Rooms"), m_menu_rooms, MENU_ROOMS);
	menuBar()->insertItem(i18n("&Game"), m_menu_game, MENU_GAME);
	menuBar()->insertItem(i18n("&Preferences"), m_menu_preferences, MENU_PREFERENCES);
	menuBar()->insertItem(i18n("&Help"), helpMenu(), MENU_HELP);

	menuBar()->setItemEnabled(MENU_ROOMS, false);
	menuBar()->setItemEnabled(MENU_GAME, false);

	connect(m_menu_ggz, SIGNAL(activated(int)), SLOT(slotMenu(int)));
	connect(m_menu_client, SIGNAL(activated(int)), SLOT(slotMenu(int)));
	connect(m_menu_rooms, SIGNAL(activated(int)), SLOT(slotMenu(int)));
	connect(m_menu_game, SIGNAL(activated(int)), SLOT(slotMenu(int)));
	connect(m_menu_preferences, SIGNAL(activated(int)), SLOT(slotMenu(int)));
	connect(kggz, SIGNAL(signalMenu(int)), SLOT(slotMenuSignal(int)));
	connect(kggz, SIGNAL(signalRoom(const char*, const char*)), SLOT(slotRoom(const char*, const char*)));
	connect(kggz, SIGNAL(signalCaption(const char*)), SLOT(slotCaption(const char*)));
	connect(kggz, SIGNAL(signalState(int)), SLOT(slotState(int)));
	connect(kggz, SIGNAL(signalLocation(const char*)), SLOT(slotLocation(const char*)));

	setCentralWidget(kggz);
	setCaption(i18n("offline"));
	if(x || y || width || height) setGeometry(x, y, width, height);
	else setGeometry(KApplication::desktop()->width() / 2 - 250, KApplication::desktop()->height() / 2 - 225, 500, 441);
	show();

	kggz->menuView(KGGZ::VIEW_SPLASH);

	statusBar()->changeItem(i18n("  Ready for connection.  "), 2);
}

KGGZBase::~KGGZBase()
{
	KSimpleConfig konfig("kggzrc");
	konfig.writeEntry("x", x());
	konfig.writeEntry("y", y());
	konfig.writeEntry("width", width());
	konfig.writeEntry("height", height());
}

QIconSet KGGZBase::kggzGetIcon(int menuid)
{
	const char *icon = NULL;
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
		case MENU_GAME_CANCEL:
			icon = "cancel.png";
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
	if(!m_about) m_about = new KGGZAbout(NULL, "KGGZAbout");
	m_about->show();
}

void KGGZBase::slotMenu(int id)
{
	int ret;

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
		case MENU_GAME_CANCEL:
			kggz->menuGameCancel();
			break;
		case MENU_GAME_GRUBBY:
			kggz->menuGrubby();
			break;
		case MENU_GAME_INFO:
			kggz->menuGameInfo();
			break;
		case MENU_PREFERENCES_SETTINGS:
			kggz->menuPreferencesSettings();
			break;
		default:
			kggz->menuView(KGGZ::VIEW_CHAT);
			kggz->menuRoom(id - MENU_ROOMS_SLOTS);
	}
}

void KGGZBase::slotMenuSignal(int signal)
{
	switch(signal)
	{
		case KGGZ::MENUSIG_DISCONNECT:
			for(int i = 0; i < m_rooms; i++)
				m_menu_rooms->removeItemAt(0);
			m_rooms = 0;
			m_menu_ggz->setItemEnabled(MENU_GGZ_CONNECT, TRUE);
			m_menu_ggz->setItemEnabled(MENU_GGZ_DISCONNECT, FALSE);
			m_menu_client->setItemEnabled(MENU_CLIENT_CHAT, FALSE);
			m_menu_client->setItemEnabled(MENU_CLIENT_TABLES, FALSE);
			m_menu_client->setItemEnabled(MENU_CLIENT_PLAYERS, FALSE);
			menuBar()->setItemEnabled(MENU_ROOMS, false);
			menuBar()->setItemEnabled(MENU_GAME, false);
			setCaption(i18n("offline"));
			break;
		case KGGZ::MENUSIG_ROOMLIST:
			menuBar()->setItemEnabled(MENU_ROOMS, true);
			break;
		case KGGZ::MENUSIG_ROOMENTER:
			menuBar()->setItemEnabled(MENU_GAME, true);
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
		case KGGZ::MENUSIG_GAMESTART:
			m_menu_game->setItemEnabled(MENU_GAME_CANCEL, TRUE);
			m_menu_game->setItemEnabled(MENU_GAME_LAUNCH, FALSE);
			m_menu_game->setItemEnabled(MENU_GAME_JOIN, FALSE);
			break;
		case KGGZ::MENUSIG_GAMEOVER:
			m_menu_game->setItemEnabled(MENU_GAME_CANCEL, FALSE);
			m_menu_game->setItemEnabled(MENU_GAME_LAUNCH, TRUE);
			m_menu_game->setItemEnabled(MENU_GAME_JOIN, TRUE);
			break;
		default:
			KGGZDEBUG("Unknown signal for menu handling: %i!\n", signal);
			break;
	}
}

void KGGZBase::slotRoom(const char *roomname, const char *category)
{
	QString caption;

#ifdef KGGZ_PATCH_C_AND_R
	caption = QString("%1 (%2)").arg(roomname).arg(category);
#else
	caption = roomname;
#endif
	m_menu_rooms->insertItem(kggzGetIcon(MENU_ROOMS_SLOTS + m_rooms), caption, MENU_ROOMS_SLOTS + m_rooms);
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

void KGGZBase::slotLocation(const char *location)
{
	statusBar()->changeItem(QString("  ") + location + "  ", 3);
}

