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
#include "kggz_profiles.h"
#include "kggz_server.h"
#include "kggz_state.h"
#include <kmenubar.h>
//#include <qtextbrowser.h>
#include <qiconview.h>
#include <iostream.h>

QString instdir = "/usr/local/share/kggz/";
QTextBrowser *helpbrowser;
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
  	menu_ggz->insertItem("&Connect", MENU_GGZ_CONNECT);
  	menu_ggz->insertItem("&Disconnect", MENU_GGZ_DISCONNECT);
	menu_ggz->insertItem("&Quit", MENU_GGZ_QUIT);
	menu_ggz->setItemEnabled(MENU_GGZ_DISCONNECT, FALSE);

 	menu_client = new KPopupMenu(this, "menu_client");
  	menu_client->insertItem("&Startup Screen", MENU_CLIENT_STARTUP);
  	menu_client->insertItem("&Chat", MENU_CLIENT_CHAT);
  	menu_client->insertItem("&Available tables", MENU_CLIENT_TABLES);
  	menu_client->insertItem("&List of players", MENU_CLIENT_PLAYERS);
	menu_client->insertItem("&Quick Help", MENU_CLIENT_HELP);
	menu_client->setItemEnabled(MENU_CLIENT_CHAT, FALSE);
	menu_client->setItemEnabled(MENU_CLIENT_TABLES, FALSE);
	menu_client->setItemEnabled(MENU_CLIENT_PLAYERS, FALSE);

 	menu_rooms = new KPopupMenu(this, "menu_rooms");
	menu_rooms->setEnabled(FALSE);

	menu_game = new KPopupMenu(this, "menu_game");
	menu_game->insertItem("&Launch new game", MENU_GAME_LAUNCH);
	menu_game->insertItem("&Join game", MENU_GAME_JOIN);
	menu_game->setEnabled(FALSE);

	menu_preferences = new KPopupMenu(this, "menu_preferences");
	menu_preferences->insertItem("Se&ttings", MENU_PREFERENCES_SETTINGS);
	menu_preferences->insertItem("&Profiles", MENU_PREFERENCES_PROFILES);
	menu_preferences->insertSeparator();
	menu_preferences->insertItem("&All Preferences", MENU_PREFERENCES_PREFERENCES);

	menu_help = helpMenu(NULL, FALSE);

 	menu->insertItem("GG&Z", menu_ggz);
 	menu->insertItem("&Client", menu_client);
 	menu->insertItem("&Rooms", menu_rooms);
 	menu->insertItem("&Games", menu_game);
 	menu->insertItem("&Preferences", menu_preferences);
	menu->insertItem("&Help", menu_help);

	startup = new KGGZ_Startup(this, "startup");
	startup->setBackgroundColor(QColor(255, 255, 255));
	startup->setBackgroundPixmap(QPixmap(instdir + "images/startup.png"));

	tables = new KGGZ_Tables(this, "tables");
	tables->hide();

	userlist = new KGGZ_Users(this, "userlist");
	userlist->hide();

	chat = new KGGZ_Chat(this, "chat");
	chat->hide();

	helpbrowser = new QTextBrowser(this, "helpbrowser");
	helpbrowser->setSource(instdir + "help/index.html");
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
}

KGGZ::~KGGZ()
{
	KGGZ_Server::disconnect();
	KGGZ_Server::shutdown();
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
	KGGZ_Settings *tmp;
	KGGZ_Connect *tmp2;
	KGGZ_Launch *tmp3;
	KGGZ_Preferences *tmp4;
	KGGZ_Profiles *tmp5;

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
		case MENU_PREFERENCES_SETTINGS:
			tmp = new KGGZ_Settings(NULL, "settings");
			tmp->show();
			break;
		case MENU_PREFERENCES_PROFILES:
			tmp5 = new KGGZ_Profiles(NULL, "profiles");
			tmp5->show();
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