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
#include <kapplication.h>
#include <kpopupmenu.h>
#include <kmenubar.h>
#include <kstatusbar.h>
#include <ksystemtray.h>
#include <kiconloader.h>

// Qt includes
#include <qtooltip.h>

KGGZBase::KGGZBase()
: KMainWindow()
{
	int x, y, width, height, watcher;

	tray = NULL;
	m_about = NULL;
	m_rooms = 0;
	m_activity = 0;

	KSimpleConfig konfig("kggzrc");
	konfig.setGroup("kggzbase");
	x = konfig.readNumEntry("x");
	y = konfig.readNumEntry("y");
	width = konfig.readNumEntry("width");
	height = konfig.readNumEntry("height");
	watcher = konfig.readNumEntry("watcher");

	statusBar()->insertItem(i18n("Not connected"), STATUS_CONNECTION);
	statusBar()->insertItem(i18n("Loading..."), STATUS_STATE);
	statusBar()->insertItem(i18n("No room selected"), STATUS_ROOM);
	statusBar()->insertItem(QString::null, STATUS_PLAYERS);
	//statusBar()->insertItem(QString::null, STATUS_ENCRYPTION);
	m_lock = new KStatusBarLabel(QString::null, STATUS_ENCRYPTION, statusBar());
	statusBar()->addWidget(m_lock);

	kggz = new KGGZ(this, "kggz");

	m_menu_ggz = new KPopupMenu(this, "menu_ggz");
	m_menu_ggz->insertItem(kggzGetIcon(MENU_GGZ_CONNECT), i18n("&Connect"), MENU_GGZ_CONNECT);
	m_menu_ggz->insertItem(kggzGetIcon(MENU_GGZ_DISCONNECT), i18n("&Disconnect"), MENU_GGZ_DISCONNECT);
	m_menu_ggz->insertSeparator();
	m_menu_ggz->insertItem(kggzGetIcon(MENU_GGZ_MOTD), i18n("Message of the day"), MENU_GGZ_MOTD);
	m_menu_ggz->insertSeparator();
	m_menu_ggz->insertItem(kggzGetIcon(MENU_GGZ_WATCHER), i18n("Use panel watcher"), MENU_GGZ_WATCHER);
	m_menu_ggz->insertSeparator();
	m_menu_ggz->insertItem(kggzGetIcon(MENU_GGZ_STARTSERVER), i18n("Start server"), MENU_GGZ_STARTSERVER);
	m_menu_ggz->insertItem(kggzGetIcon(MENU_GGZ_STOPSERVER), i18n("Stop server"), MENU_GGZ_STOPSERVER);
	m_menu_ggz->insertSeparator();
	m_menu_ggz->insertItem(kggzGetIcon(MENU_GGZ_QUIT), i18n("&Quit"), MENU_GGZ_QUIT);

	m_menu_ggz->setItemEnabled(MENU_GGZ_DISCONNECT, FALSE);
	m_menu_ggz->setItemEnabled(MENU_GGZ_MOTD, FALSE);
	if(KGGZCommon::findProcess("ggzd") > 0) m_menu_ggz->setItemEnabled(MENU_GGZ_STARTSERVER, FALSE);
	else m_menu_ggz->setItemEnabled(MENU_GGZ_STOPSERVER, FALSE);
	m_menu_ggz->setCheckable(true);

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
	m_menu_game->insertItem(kggzGetIcon(MENU_GAME_RULES), i18n("&Rules of the game"), MENU_GAME_RULES);
	m_menu_game->insertSeparator();
	m_menu_game->insertItem(kggzGetIcon(MENU_GAME_LAUNCH), i18n("&Launch new game"), MENU_GAME_LAUNCH);
	m_menu_game->insertItem(kggzGetIcon(MENU_GAME_JOIN), i18n("&Join game"), MENU_GAME_JOIN);
#ifdef KGGZ_PATCH_SPECTATORS
	m_menu_game->insertItem(kggzGetIcon(MENU_GAME_JOIN), i18n("Join as spectator"), MENU_GAME_SPECTATOR);
#endif
	m_menu_game->insertItem(kggzGetIcon(MENU_GAME_CANCEL), i18n("&Cancel game"), MENU_GAME_CANCEL);
	m_menu_game->insertSeparator();
	m_menu_game->insertItem(kggzGetIcon(MENU_GAME_GRUBBY), i18n("&Grubby"), MENU_GAME_GRUBBY);
	m_menu_game->insertSeparator();
	m_menu_game->insertItem(kggzGetIcon(MENU_GAME_TEAM), i18n("Teams"), MENU_GAME_TEAM);

	m_menu_game->setItemEnabled(MENU_GAME_CANCEL, FALSE);

	m_menu_preferences = new KPopupMenu(this, "menu_preferences");
	m_menu_preferences->insertItem(kggzGetIcon(MENU_PREFERENCES_SETTINGS), i18n("Se&ttings"), MENU_PREFERENCES_SETTINGS);

	helpMenu()->insertItem(kggzGetIcon(MENU_HELP_GGZ), i18n("About the GGZ Gaming Zone"), 99, 5);
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
	connect(kggz, SIGNAL(signalRoom(const char*, const char*, const char*, int)), SLOT(slotRoom(const char*, const char*, const char*, int)));
	connect(kggz, SIGNAL(signalRoomChanged(const char*, const char*, int, int)), SLOT(slotRoomChanged(const char*, const char*, int, int)));
	connect(kggz, SIGNAL(signalCaption(QString, bool)), SLOT(slotCaption(QString, bool)));
	connect(kggz, SIGNAL(signalState(int)), SLOT(slotState(int)));
	connect(kggz, SIGNAL(signalLocation(QString)), SLOT(slotLocation(QString)));
	connect(kggz, SIGNAL(signalPlayers(int)), SLOT(slotPlayers(int)));
	connect(kggz, SIGNAL(signalActivity(int)), SLOT(slotActivity(int)));

	setCentralWidget(kggz);
	setCaption(i18n("offline"));
	if(x || y || width || height) setGeometry(x, y, width, height);
	else setGeometry(KApplication::desktop()->width() / 2 - 250, KApplication::desktop()->height() / 2 - 225, 500, 441);
	show();

	if(watcher) slotMenu(MENU_GGZ_WATCHER);

	kggz->menuView(KGGZ::VIEW_SPLASH);

	statusBar()->changeItem(i18n("Ready for connection."), STATUS_STATE);
}

KGGZBase::~KGGZBase()
{
	KSimpleConfig konfig("kggzrc");
	konfig.setGroup("kggzbase");
	konfig.writeEntry("x", x());
	konfig.writeEntry("y", y());
	konfig.writeEntry("width", width());
	konfig.writeEntry("height", height());
	if(tray) konfig.writeEntry("watcher", 1);
	else konfig.writeEntry("watcher", 0);
}

void KGGZBase::autoconnect(QString uri)
{
	kggz->autoconnect(uri);
}

QIconSet KGGZBase::kggzGetIcon(int menuid)
{
	QString icon;
	QIconSet iconset;

	switch(menuid)
	{
		case MENU_GGZ_CONNECT:
			return KGlobal::iconLoader()->loadIcon("connect_creating", KIcon::Small);
			break;
		case MENU_GGZ_DISCONNECT:
			return KGlobal::iconLoader()->loadIcon("connect_no", KIcon::Small);
			break;
		case MENU_GGZ_MOTD:
			return KGlobal::iconLoader()->loadIcon("news", KIcon::Small);
			break;
		case MENU_GGZ_WATCHER:
			return KGlobal::iconLoader()->loadIcon("idea", KIcon::Small);
			break;
		case MENU_GGZ_QUIT:
			return KGlobal::iconLoader()->loadIcon("exit", KIcon::Small);
			break;
		case MENU_PREFERENCES_SETTINGS:
			return KGlobal::iconLoader()->loadIcon("configure", KIcon::Small);
			break;
		case MENU_GAME_INFO:
			return KGlobal::iconLoader()->loadIcon("info", KIcon::Small);
			break;
		case MENU_GAME_RULES:
			return KGlobal::iconLoader()->loadIcon("contents", KIcon::Small);
			break;
		case MENU_GAME_CANCEL:
			return KGlobal::iconLoader()->loadIcon("cancel", KIcon::Small);
			break;
		case MENU_GAME_LAUNCH:
			return KGlobal::iconLoader()->loadIcon("1uparrow", KIcon::Small);
			break;
		case MENU_GAME_JOIN:
			return KGlobal::iconLoader()->loadIcon("1rightarrow", KIcon::Small);
			break;
		case MENU_HELP_GGZ:
			return KGlobal::iconLoader()->loadIcon("ggz", KIcon::Small);
			break;
	}

//(KGGZ_DIRECTORY "/images/icons/ggz.png")
	switch(menuid)
	{
//		case MENU_GGZ_CONNECT:
//			icon = "connect.png";
//			break;
//		case MENU_GGZ_DISCONNECT:
//			icon = "disconnect.png";
//			break;
		case MENU_GGZ_STARTSERVER:
			icon = "startserver.png";
			break;
		case MENU_GGZ_STOPSERVER:
			icon = "stopserver.png";
			break;
//		case MENU_GGZ_QUIT:
//			icon = "quit.png";
//			break;
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
//		case MENU_GAME_INFO:
//			icon = "info.png";
//			break;
//		case MENU_GAME_LAUNCH:
//			icon = "launch.png";
//			break;
//		case MENU_GAME_JOIN:
//			icon = "join.png";
//			break;
//		case MENU_GAME_CANCEL:
//			icon = "cancel.png";
//			break;
		case MENU_GAME_UPDATE:
			icon = "update.png";
			break;
		case MENU_GAME_GRUBBY:
			icon = "grubby.png";
			break;
//		case MENU_PREFERENCES_SETTINGS:
//		case MENU_PREFERENCES_PLAYERINFO:
//		case MENU_PREFERENCES_HOSTS:
//		case MENU_PREFERENCES_FTP:
//		case MENU_PREFERENCES_GAMES:
//		case MENU_PREFERENCES_THEMES:
//			icon = "pref.png";
//			break;
//		case MENU_PREFERENCES_PREFERENCES:
//			icon = "preferences.png";
//			break;
		default:
			icon = "unknown.png";
			break;
	}

	if(menuid >= MENU_ROOMS_SLOTS)
	{
		icon = QString("games/%1.png").arg(m_lastgame); //"unknown.png";
		KGGZDEBUG("gameicon: %s", icon.utf8().data());
	}

	iconset = QIconSet(QPixmap(QString(KGGZ_DIRECTORY "/images/icons/") + icon));
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
			statusBar()->changeItem(i18n("Not connected"), STATUS_CONNECTION);
			statusBar()->changeItem(QString::null, STATUS_PLAYERS);
			m_lock->setFixedWidth(0);
			break;
		case MENU_GGZ_MOTD:
			kggz->menuMotd();
			break;
		case MENU_GGZ_WATCHER:
			if(m_menu_ggz->isItemChecked(MENU_GGZ_WATCHER))
			{
				if(tray)
				{
					m_menu_ggz->setItemChecked(MENU_GGZ_WATCHER, false);
					delete tray;
					tray = NULL;
				}
			}
			else
			{
				if(!tray)
				{
					m_menu_ggz->setItemChecked(MENU_GGZ_WATCHER, true);
					tray = new KSystemTray(this);
					slotActivity(KGGZ::ACTIVITY_NONE);
					tray->show();
				}
			}
			break;
		case MENU_GGZ_STARTSERVER:
			kggz->menuServerLaunch();
			break;
		case MENU_GGZ_STOPSERVER:
			kggz->menuServerKill();
			break;
   		case MENU_GGZ_QUIT:
			ret = KMessageBox::questionYesNo(this, i18n("Do you really want to quit?"), i18n("Quit KGGZ"));
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
		case MENU_GAME_SPECTATOR:
			kggz->menuGameSpectator();
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
		case MENU_GAME_RULES:
			kggz->menuGameRules();
			break;
		case MENU_GAME_TEAM:
			kggz->menuGameTeam();
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
			m_menu_ggz->setItemEnabled(MENU_GGZ_MOTD, FALSE);
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
			m_menu_ggz->setItemEnabled(MENU_GGZ_MOTD, TRUE);
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
#ifdef KGGZ_PATCH_SPECTATORS
			m_menu_game->setItemEnabled(MENU_GAME_SPECTATOR, FALSE);
#endif
			break;
		case KGGZ::MENUSIG_GAMEOVER:
			m_menu_game->setItemEnabled(MENU_GAME_CANCEL, FALSE);
			m_menu_game->setItemEnabled(MENU_GAME_LAUNCH, TRUE);
			m_menu_game->setItemEnabled(MENU_GAME_JOIN, TRUE);
#ifdef KGGZ_PATCH_SPECTATORS
			m_menu_game->setItemEnabled(MENU_GAME_SPECTATOR, TRUE);
#endif
			break;
		case KGGZ::MENUSIG_SPECTATORS:
#ifdef KGGZ_PATCH_SPECTATORS
			m_menu_game->setItemEnabled(MENU_GAME_SPECTATOR, TRUE);
#endif
			break;
		case KGGZ::MENUSIG_NOSPECTATORS:
#ifdef KGGZ_PATCH_SPECTATORS
			m_menu_game->setItemEnabled(MENU_GAME_SPECTATOR, FALSE);
#endif
			break;
		case KGGZ::MENUSIG_RULES:
			m_menu_game->setItemEnabled(MENU_GAME_RULES, TRUE);
			break;
		case KGGZ::MENUSIG_NORULES:
			m_menu_game->setItemEnabled(MENU_GAME_RULES, FALSE);
			break;
		default:
			KGGZDEBUG("Unknown signal for menu handling: %i!\n", signal);
			break;
	}
}

void KGGZBase::slotRoom(const char *roomname, const char *protocolname, const char *category, int numplayers)
{
	QString caption;

#ifdef KGGZ_PATCH_C_AND_R
	caption = QString("%1 (%2)").arg(roomname).arg(category);
#else
	Q_UNUSED(category);

	caption = roomname;
	if(numplayers > 0) caption = QString("%1 (%2)").arg(roomname).arg(numplayers);
#endif
	m_lastgame = protocolname;
	m_menu_rooms->insertItem(kggzGetIcon(MENU_ROOMS_SLOTS + m_rooms), caption, MENU_ROOMS_SLOTS + m_rooms);
	m_rooms++;
}

void KGGZBase::slotRoomChanged(const char *roomname, const char *protocolname, int roomnumber, int numplayers)
{
	QString caption;

	caption = roomname;
	if(numplayers > 0) caption = QString("%1 (%2)").arg(roomname).arg(numplayers);

	m_lastgame = protocolname;
	m_menu_rooms->changeItem(MENU_ROOMS_SLOTS + roomnumber, kggzGetIcon(MENU_ROOMS_SLOTS + roomnumber), caption);
	KGGZDEBUG("***** ROOMS CHANGED: %s(%i)=%i", roomname, roomnumber, numplayers);
}

void KGGZBase::slotCaption(QString caption, bool encrypted)
{
	setCaption(caption);
	statusBar()->changeItem(i18n("Connected"), STATUS_CONNECTION);

	m_lock->setFixedSize(KIcon::SizeSmall, KIcon::SizeSmall);
	if(encrypted)
	{
		//m_lock->setBackgroundColor(QColor(0, 255, 0));
		QPixmap lock = KGlobal::iconLoader()->loadIcon("encrypted", KIcon::Small);
		m_lock->setBackgroundPixmap(lock);
		QToolTip::add(m_lock, i18n("Connection secured with TLS"));
	}
	else
	{
		//m_lock->setBackgroundColor(QColor(255, 0, 0));
		QPixmap lock = KGlobal::iconLoader()->loadIcon("decrypted", KIcon::Small);
		m_lock->setBackgroundPixmap(lock);
		QToolTip::add(m_lock, i18n("Insecure connection"));
	}
}

void KGGZBase::slotState(int state)
{
	statusBar()->changeItem(i18n("State: ") + KGGZCommon::state((GGZStateID)state), STATUS_STATE);
}

void KGGZBase::slotLocation(QString location)
{
	statusBar()->changeItem(location, STATUS_ROOM);
}

void KGGZBase::slotPlayers(int players)
{
	statusBar()->changeItem(QString("Players on server: %1").arg(players), STATUS_PLAYERS);
}

void KGGZBase::slotActivity(int activity)
{
	if((activity > KGGZ::ACTIVITY_NONE) && (activity <= m_activity)) return;

	m_activity = activity;

	if(tray)
	{
		if(activity == KGGZ::ACTIVITY_DIRECT)
		{
			tray->setPixmap(QPixmap(KGGZ_DIRECTORY "/images/icons/watcher_on.png"));
		}
		else if(activity == KGGZ::ACTIVITY_ROOM)
		{
			tray->setPixmap(QPixmap(KGGZ_DIRECTORY "/images/icons/watcher_room.png"));
		}
		else
		{
			tray->setPixmap(QPixmap(KGGZ_DIRECTORY "/images/icons/watcher_off.png"));
		}
	}
}

