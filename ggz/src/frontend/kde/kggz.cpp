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
#include <kmenubar.h>
#include <kpopupmenu.h>
#include <qtextbrowser.h>
#include <qiconview.h>
#include <iostream.h>

QString instdir = "/home/comtec/games/KGGZ/";
QTextBrowser *helpbrowser;
KMenuBar *menu;

KGGZ_Startup *startup;
KGGZ_Chat *chat;
KGGZ_Tables *tables;
KGGZ_Users *userlist;

KGGZ::KGGZ()
: KTMainWindow("window_main")
{
	KPopupMenu *menu_ggz, *menu_client, *menu_rooms, *menu_game, *menu_preferences, *menu_help;

	menu = new KMenuBar(this);

 	menu_ggz = new KPopupMenu(this, "menu_ggz");
  	menu_ggz->insertItem("&Connect", MENU_GGZ_CONNECT);
  	menu_ggz->insertItem("&Disconnect", MENU_GGZ_DISCONNECT);
	menu_ggz->insertItem("&Quit", MENU_GGZ_QUIT);

 	menu_client = new KPopupMenu(this, "menu_client");
  	menu_client->insertItem("&Startup Screen", MENU_CLIENT_STARTUP);
  	menu_client->insertItem("&Chat", MENU_CLIENT_CHAT);
  	menu_client->insertItem("&Available tables", MENU_CLIENT_TABLES);
  	menu_client->insertItem("&List of players", MENU_CLIENT_PLAYERS);
	menu_client->insertItem("&Quick Help", MENU_CLIENT_HELP);

 	menu_rooms = new KPopupMenu(this, "menu_rooms");
  	menu_rooms->insertItem("TicTacToe", new KGGZ_RoomsMenu("TicTacToe", this, NULL), MENU_ROOMS_SLOTS);
	menu_rooms->insertItem("NetSpades", new KGGZ_RoomsMenu("NetSpades", this, NULL), MENU_ROOMS_SLOTS + 1);
	menu_rooms->insertItem("La Pocha", new KGGZ_RoomsMenu("La Pocha", this, NULL), MENU_ROOMS_SLOTS + 2);
  	menu_rooms->insertItem("Hastings1066", new KGGZ_RoomsMenu("Hastings1066", this, NULL), MENU_ROOMS_SLOTS + 3);
	menu_rooms->insertItem("Reversi", new KGGZ_RoomsMenu("Reversi", this, NULL), MENU_ROOMS_SLOTS + 4);

	menu_game = new KPopupMenu(this, "menu_game");
	menu_game->insertItem("&Launch new game", MENU_GAME_LAUNCH);
	menu_game->insertItem("&Join game", MENU_GAME_JOIN);

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
	startup->setBackgroundPixmap(QPixmap(instdir + "images/startup.png"));

	chat = new KGGZ_Chat(this, "chat");
	chat->hide();

	tables = new KGGZ_Tables(this, "tables");
	tables->hide();

	userlist = new KGGZ_Users(this, "userlist");
	userlist->hide();

	helpbrowser = new QTextBrowser(this, "helpbrowser");
	helpbrowser->setSource(instdir + "help/index.html");
	helpbrowser->hide();

        // can't connect directly to menu because help results in code 2 (exit)
	connect(menu_ggz, SIGNAL(activated(int)), SLOT(handleMenu(int)));
        connect(menu_rooms, SIGNAL(activated(int)), SLOT(handleMenu(int)));
        connect(menu_game, SIGNAL(activated(int)), SLOT(handleMenu(int)));
        connect(menu_preferences, SIGNAL(activated(int)), SLOT(handleMenu(int)));

	resize(500, 430);
	show();
}

KGGZ::~KGGZ()
{
}

void KGGZ::handleMenu(int id)
{
	KGGZ_Settings *tmp;
	KGGZ_Connect *tmp2;
	KGGZ_Launch *tmp3;
	KGGZ_Preferences *tmp4;
	KGGZ_Profiles *tmp5;

	switch(id)
	{
		case MENU_GGZ_CONNECT:
			tmp2 = new KGGZ_Connect(NULL, "connect");
			tmp2->show();
			break;
		case MENU_GGZ_DISCONNECT:
			KGGZ_Server::disconnect();
			KGGZ_Server::shutdown();
			break;
   		case MENU_GGZ_QUIT:
			exit(0);
			break;
		case MENU_CLIENT_STARTUP:
			chat->hide();
			helpbrowser->hide();
			tables->hide();
			userlist->hide();
			startup->show();
			break;
		case MENU_CLIENT_CHAT:
			startup->hide();
			helpbrowser->hide();
			tables->hide();
			userlist->hide();
			chat->show();
			break;
		case MENU_CLIENT_TABLES:
			startup->hide();
			helpbrowser->hide();
			chat->hide();
			userlist->hide();
			tables->show();
			break;
		case MENU_CLIENT_PLAYERS:
			startup->hide();
			helpbrowser->hide();
			chat->hide();
			tables->hide();
			userlist->show();
			break;
		case MENU_CLIENT_HELP:
			startup->hide();
			chat->hide();
			tables->hide();
			userlist->hide();
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