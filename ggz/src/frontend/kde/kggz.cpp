#include "kggz.h"
#include "kggz_chat.h"
#include "kggz_startup.h"
#include "kggz_tables.h"
#include "kggz_roomsmenu.h"
#include "kggz_settings.h"
#include "kggz_preferences.h"
#include "kggz_users.h"
#include "kggz_connect.h"
#include "kggz_launch.h"
#include "kggz_hosts.h"
#include "kggz_ftp.h"
#include "kggz_games.h"
#include "kggz_server.h"
#include "kggz_state.h"
#include "kggz_browser.h"
#ifdef USE_SERVER
#include <kmessagebox.h>
#include <klocale.h>
#endif
#include <kmenubar.h>
#include <qiconview.h>
#include <iostream.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define instdir "/usr/local/share/ggz/"
#endif

#include <ggzcore.h>

KGGZ_Browser *helpbrowser;
KMenuBar *menu;

KGGZ_Startup *startup;
KGGZ_Chat *chat;
KGGZ_Tables *tables;
KGGZ_Users *userlist;
KPopupMenu *menu_client, *menu_game, *menu_ggz, *menu_rooms;

KGGZ::KGGZ()
: KTMainWindow("window_main")
{
	KPopupMenu *menu_preferences, *menu_help;

	menu = new KMenuBar(this);

 	menu_ggz = new KPopupMenu(this, "menu_ggz");
  	menu_ggz->insertItem(getIcon(MENU_GGZ_CONNECT), i18n("&Connect"), MENU_GGZ_CONNECT);
  	menu_ggz->insertItem(getIcon(MENU_GGZ_DISCONNECT), i18n("&Disconnect"), MENU_GGZ_DISCONNECT);
#ifdef USE_SERVER
	menu_ggz->insertSeparator();
	menu_ggz->insertItem(getIcon(MENU_GGZ_STARTSERVER), i18n("Start server"), MENU_GGZ_STARTSERVER);
	menu_ggz->insertItem(getIcon(MENU_GGZ_STOPSERVER), i18n("Stop server"), MENU_GGZ_STOPSERVER);
#endif
	menu_ggz->insertSeparator();
	menu_ggz->insertItem(getIcon(MENU_GGZ_QUIT), i18n("&Quit"), MENU_GGZ_QUIT);
	menu_ggz->setItemEnabled(MENU_GGZ_DISCONNECT, FALSE);

	menu_client = new KPopupMenu(this, "menu_client");
	menu_client->insertItem(getIcon(MENU_CLIENT_STARTUP), i18n("&Startup Screen"), MENU_CLIENT_STARTUP);
	menu_client->insertItem(getIcon(MENU_CLIENT_CHAT), i18n("&Chat"), MENU_CLIENT_CHAT);
	menu_client->insertItem(getIcon(MENU_CLIENT_TABLES), i18n("&Available tables"), MENU_CLIENT_TABLES);
	menu_client->insertItem(getIcon(MENU_CLIENT_PLAYERS), i18n("&List of players"), MENU_CLIENT_PLAYERS);
#ifdef USE_KHTML
	menu_client->insertItem(getIcon(MENU_CLIENT_HELP), i18n("&Web Browser"), MENU_CLIENT_HELP);
#else
	menu_client->insertItem(getIcon(MENU_CLIENT_HELP), i18n("&Quick Help"), MENU_CLIENT_HELP);
#endif
	menu_client->setItemEnabled(MENU_CLIENT_CHAT, FALSE);
	menu_client->setItemEnabled(MENU_CLIENT_TABLES, FALSE);
	menu_client->setItemEnabled(MENU_CLIENT_PLAYERS, FALSE);

 	menu_rooms = new KPopupMenu(this, "menu_rooms");
	menu_rooms->setEnabled(FALSE);

	menu_game = new KPopupMenu(this, "menu_game");
	menu_game->insertItem(getIcon(MENU_GAME_LAUNCH), i18n("&Launch new game"), MENU_GAME_LAUNCH);
	menu_game->insertItem(getIcon(MENU_GAME_JOIN), i18n("&Join game"), MENU_GAME_JOIN);
#ifdef USE_FTP
	menu_game->insertItem(getIcon(MENU_GAME_UPDATE), i18n("&Update from FTP"), MENU_GAME_UPDATE);
#endif
	menu_game->insertSeparator();
	menu_game->insertItem(getIcon(MENU_GAME_GRUBBY), i18n("&Grubby"), MENU_GAME_GRUBBY);
	menu_game->setEnabled(FALSE);

	menu_preferences = new KPopupMenu(this, "menu_preferences");
	menu_preferences->insertItem(getIcon(MENU_PREFERENCES_SETTINGS), i18n("Se&ttings"), MENU_PREFERENCES_SETTINGS);
	menu_preferences->insertItem(getIcon(MENU_PREFERENCES_PLAYERINFO), i18n("&Player information"), MENU_PREFERENCES_PLAYERINFO);
	menu_preferences->insertItem(getIcon(MENU_PREFERENCES_HOSTS), i18n("Ga&me servers"), MENU_PREFERENCES_HOSTS);
	menu_preferences->insertItem(getIcon(MENU_PREFERENCES_FTP), i18n("&FTP servers"), MENU_PREFERENCES_FTP);
	menu_preferences->insertItem(getIcon(MENU_PREFERENCES_GAMES), i18n("&Games"), MENU_PREFERENCES_GAMES);
	menu_preferences->insertSeparator();
	menu_preferences->insertItem(getIcon(MENU_PREFERENCES_PREFERENCES), i18n("&All Preferences"), MENU_PREFERENCES_PREFERENCES);

	menu_help = helpMenu(NULL, FALSE);

 	menu->insertItem(i18n("GG&Z"), menu_ggz);
 	menu->insertItem(i18n("&Client"), menu_client);
 	menu->insertItem(i18n("&Rooms"), menu_rooms);
 	menu->insertItem(i18n("&Games"), menu_game);
 	menu->insertItem(i18n("&Preferences"), menu_preferences);
	menu->insertItem(i18n("&Help"), menu_help);

	startup = new KGGZ_Startup(this, "startup");
	startup->setBackgroundColor(QColor(255, 255, 255));

#ifdef HAVE_CONFIG_H
	startup->setBackgroundPixmap(QPixmap(QString(instdir) + "images/startup.png"));
#endif

	tables = new KGGZ_Tables(this, "tables");
	tables->hide();

	userlist = new KGGZ_Users(this, "userlist");
	userlist->hide();

	chat = new KGGZ_Chat(this, "chat");
	chat->hide();

	helpbrowser = new KGGZ_Browser(this, "helpbrowser");
	helpbrowser->hide();

        // can't connect directly to menu because help results in code 2 (exit)
	connect(menu_ggz, SIGNAL(activated(int)), SLOT(handleMenu(int)));
	connect(menu_client, SIGNAL(activated(int)), SLOT(handleMenu(int)));
        connect(menu_rooms, SIGNAL(activated(int)), SLOT(handleMenu(int)));
        connect(menu_game, SIGNAL(activated(int)), SLOT(handleMenu(int)));
        connect(menu_preferences, SIGNAL(activated(int)), SLOT(handleMenu(int)));

	KGGZ_Server::init();
	KGGZ_State::registerStates();

	resize(500, 430);
	show();

	// Show connection dialog on startup, if desired
	if(ggzcore_conf_read_int("KGGZ-Settings", "Startup", 0))
	{
		handleMenu(MENU_GGZ_CONNECT);
	}

	// Is this necessary?
	m_grubby = NULL;
#ifdef USE_FTP
	m_update = NULL;
#endif
	m_playerinfo = NULL;
}

KGGZ::~KGGZ()
{
	KGGZ_Server::disconnect();
	KGGZ_Server::shutdown();
}

// this is subject for "disable full-featured client"
// ...and for themes, if any
QIconSet KGGZ::getIcon(int menuid)
{
	char *icon = NULL;

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
			icon = "pref.png";
			break;
		case MENU_PREFERENCES_PREFERENCES:
			icon = "preferences.png";
			break;
		case MENU_ROOMS_SLOTS:
			icon = "unknown.png";
			break;
	}

	return QIconSet(QPixmap(QString(instdir) + QString("images/icons/") + QString(icon)));
}

KPopupMenu *KGGZ::menuRooms()
{
	return menu_rooms;
}

KPopupMenu *KGGZ::menuGGZ()
{
	return menu_ggz;
}

KPopupMenu *KGGZ::menuClient()
{
	return menu_client;
}

KPopupMenu *KGGZ::menuGame()
{
	return menu_game;
}

void KGGZ::hideChilds()
{
	chat->hide();
	helpbrowser->hide();
	tables->hide();
	userlist->hide();
	startup->hide();
}

void KGGZ::handleMenu(int id)
{
	KGGZ_Settings *tmp1;
	KGGZ_Connect *tmp2;
	KGGZ_Launch *tmp3;
	KGGZ_Preferences *tmp4;
	KGGZ_Hosts *tmp51;
	KGGZ_Ftp *tmp52;
	KGGZ_Games *tmp6;
	int result;

	cout << "kggz::handlemenu: got id: " << id << endl;

	switch(id)
	{
		case MENU_CLIENT_STARTUP:
		case MENU_CLIENT_CHAT:
		case MENU_CLIENT_TABLES:
		case MENU_CLIENT_PLAYERS:
		case MENU_CLIENT_HELP:
			hideChilds();
			break;
	}

	switch(id)
	{
		case MENU_GGZ_CONNECT:
			tmp2 = new KGGZ_Connect(NULL, "connect");
			tmp2->show();
			break;
		case MENU_GGZ_DISCONNECT:
			KGGZ_Server::disconnect();
			//KGGZ_Server::shutdown();
			break;
#ifdef USE_SERVER
		case MENU_GGZ_STARTSERVER:
			result = KGGZ_Server::launchServer();
			if(result == -1) KMessageBox::error(this, i18n("Could not start ggzd!"), i18n("Error!"));
			if(result == -2) KMessageBox::error(this, i18n("The ggzd server is already running!"), i18n("Error!"));
			break;
		case MENU_GGZ_STOPSERVER:
			result = KGGZ_Server::killServer();
			if(result == -1) KMessageBox::error(this, i18n("An error occured! Sure it runs? Sure you don't need to be root?"), i18n("Error!"));
			if(result == 0) KMessageBox::error(this, i18n("Could only kill server via sigkill!"), i18n("Error!"));
			break;
#endif
   		case MENU_GGZ_QUIT:
			exit(0);
			break;
		case MENU_CLIENT_STARTUP:
			startup->show();
			break;
		case MENU_CLIENT_CHAT:
			chat->show();
			break;
		case MENU_CLIENT_TABLES:
			tables->show();
			break;
		case MENU_CLIENT_PLAYERS:
			userlist->show();
			break;
		case MENU_CLIENT_HELP:
			helpbrowser->show();
			break;
		case MENU_GAME_LAUNCH:
			tmp3 = new KGGZ_Launch(NULL, "launch");
			tmp3->show();
			break;
#ifdef USE_FTP
		case MENU_GAME_UPDATE:
			if(!m_update) m_update = new KGGZ_Update(NULL, "update");
			m_update->show();
			break;
#endif
		case MENU_GAME_GRUBBY:
			if (!m_grubby) m_grubby = new KGGZ_Grubby(NULL, "grubby");
			m_grubby->show();
			break;
		case MENU_PREFERENCES_SETTINGS:
			tmp1 = new KGGZ_Settings(NULL, "settings");
			tmp1->show();
			break;
		case MENU_PREFERENCES_PLAYERINFO:
			if (!m_playerinfo) m_playerinfo = new KGGZ_Playerinfo(NULL, "playerinfo");
			m_playerinfo->show();
			break;
		case MENU_PREFERENCES_HOSTS:
			tmp51 = new KGGZ_Hosts(NULL, "hosts");
			tmp51->show();
			break;
		case MENU_PREFERENCES_FTP:
			tmp52 = new KGGZ_Ftp(NULL, "ftp");
			tmp52->show();
			break;
		case MENU_PREFERENCES_GAMES:
			tmp6 = new KGGZ_Games(NULL, "games");
			tmp6->show();
			break;
		case MENU_PREFERENCES_PREFERENCES:
			tmp4 = new KGGZ_Preferences(NULL, "preferences");
			tmp4->show();
			break;
		default:
			if(id >= MENU_ROOMS_SLOTS)
			{
				KGGZ_Server::changeRoom(id - MENU_ROOMS_SLOTS);
				hideChilds();
				chat->show();
			}
	}
}

void KGGZ::resizeEvent(QResizeEvent *e)
{
	resize(e->size());
	startup->setGeometry(0, 30, e->size().width(), e->size().height() - 30);
	chat->setGeometry(0, 30, e->size().width(), e->size().height() - 30);
	tables->setGeometry(0, 30, e->size().width(), e->size().height() - 30);
	userlist->setGeometry(0, 30, e->size().width(), e->size().height() - 30);
	helpbrowser->setGeometry(0, 30, e->size().width(), e->size().height() - 30);
	menu->setGeometry(0, 0, e->size().width(), 20);
}
