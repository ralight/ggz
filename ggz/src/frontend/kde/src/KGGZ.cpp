// Header file
#include "KGGZ.h"

// KGGZ classes
#include "KGGZWorkspace.h"
#include "KGGZConnect.h"

// KDE includes
#include <kmessagebox.h>
#include <klocale.h>

// Qt includes
#include <qiconview.h>

// System includes
#include <stdio.h>
#include <iostream.h>
#include <stdlib.h>

// GGZCore++ functions
#include "GGZCore.h"
#include "GGZCoreConf.h"
#include "GGZCoreConfio.h"
#include "GGZCoreTable.h"
#include "GGZCoreModule.h"

KGGZConnect *m_connect;
GGZCore *m_core;
GGZCoreConf *m_config;

KGGZ::KGGZ()
: KGGZBase("window_main")
{
	KPopupMenu *menu_help;
	int result;

	KGGZDEBUGF("KGGZ::KGGZ()\n");
	KGGZDEBUG("Starting...\n");

	kggzroom = NULL;
	kggzserver = NULL;
	m_save_rooms = 0;
	m_save_username = NULL;
	m_save_password = NULL;
	m_save_host = NULL;
	m_lock = 0;
	m_launch = NULL;
	m_about = NULL;
	kggzgame = NULL;

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
	m_menu_client->insertItem(kggzGetIcon(MENU_CLIENT_HELP), i18n("&Web Browser"), MENU_CLIENT_HELP);
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
	m_menu_game->insertItem(kggzGetIcon(MENU_GAME_UPDATE), i18n("&Update from FTP"), MENU_GAME_UPDATE);
	m_menu_game->insertItem(kggzGetIcon(MENU_GAME_NEWS), i18n("&GGZ News"), MENU_GAME_NEWS);
	m_menu_game->insertSeparator();
	m_menu_game->insertItem(kggzGetIcon(MENU_GAME_GRUBBY), i18n("&Grubby"), MENU_GAME_GRUBBY);
	m_menu_game->setEnabled(FALSE);

	m_menu_preferences = new KPopupMenu(this, "menu_preferences");
	m_menu_preferences->insertItem(kggzGetIcon(MENU_PREFERENCES_SETTINGS), i18n("Se&ttings"), MENU_PREFERENCES_SETTINGS);
	m_menu_preferences->insertItem(kggzGetIcon(MENU_PREFERENCES_PLAYERINFO), i18n("&Player information"), MENU_PREFERENCES_PLAYERINFO);
	m_menu_preferences->insertItem(kggzGetIcon(MENU_PREFERENCES_HOSTS), i18n("Ga&me servers"), MENU_PREFERENCES_HOSTS);
	m_menu_preferences->insertItem(kggzGetIcon(MENU_PREFERENCES_FTP), i18n("&FTP servers"), MENU_PREFERENCES_FTP);
	m_menu_preferences->insertItem(kggzGetIcon(MENU_PREFERENCES_GAMES), i18n("&Games"), MENU_PREFERENCES_GAMES);
	m_menu_preferences->insertItem(kggzGetIcon(MENU_PREFERENCES_THEMES), i18n("Th&emes"), MENU_PREFERENCES_THEMES);
	m_menu_preferences->insertSeparator();
	m_menu_preferences->insertItem(kggzGetIcon(MENU_PREFERENCES_PREFERENCES), i18n("&All Preferences"), MENU_PREFERENCES_PREFERENCES);

	menu_help = helpMenu(NULL, FALSE);
	menu_help->insertItem(QPixmap(KGGZ_DIRECTORY "/images/icons/ggz.png"), i18n("About the GGZ Gaming Zone"), 99, 5);
	menu_help->connectItem(99, this, SLOT(slotAboutGGZ()));
	KGGZDEBUG("GGZ Icon at: " KGGZ_DIRECTORY "\n");

	m_menu->insertItem(i18n("GG&Z"), m_menu_ggz);
	m_menu->insertItem(i18n("&Client"), m_menu_client);
	m_menu->insertItem(i18n("&Rooms"), m_menu_rooms);
	m_menu->insertItem(i18n("&Game"), m_menu_game);
	m_menu->insertItem(i18n("&Help"), menu_help);

	m_workspace = new KGGZWorkspace(this, "workspace");

	connect(m_menu_ggz, SIGNAL(activated(int)), SLOT(slotHandleMenu(int)));
	connect(m_menu_client, SIGNAL(activated(int)), SLOT(slotHandleMenu(int)));
        connect(m_menu_rooms, SIGNAL(activated(int)), SLOT(slotHandleMenu(int)));
	connect(m_menu_game, SIGNAL(activated(int)), SLOT(slotHandleMenu(int)));

	connect(m_workspace->widgetChat(), SIGNAL(signalChat(char *)), SLOT(slotChat(char *)));

	KGGZDEBUG("Initializing GGZCore...\n");
	m_core = new GGZCore();
	result = m_core->init(GGZCore::parser | GGZCore::modules, "/tmp/kggz.debug", GGZCore::all);
	if(result == -1)
	{
		KGGZDEBUG("Critical: Could not initialize ggzcore!\n");
		m_core = NULL;
	}

	KGGZDEBUG("Loading configuration...\n");
	m_config = new GGZCoreConf();
	result = m_config->init(KGGZCommon::append(KGGZ_DIRECTORY, "/kggzrc"), KGGZCommon::append(getenv("HOME"), "/.ggz/kggz.rc"));
	if(result == -1)
	{
		KGGZDEBUG("Critical: Could not open configuration file!\n");
		m_config = NULL;
	}
	else
	{
		if(m_config->read("Preferences", "Showdialog", 0))
		{
			KGGZDEBUG("- load connection dialog");
			dialogConnection();
		}
		else m_connect = NULL;
	}

	kggzroomcallback = new KGGZCallback(this, COLLECTOR_ROOM);
	kggzservercallback = new KGGZCallback(this, COLLECTOR_SERVER);
	kggzgamecallback = new KGGZCallback(this, COLLECTOR_GAME);

	setCaption("KGGZ - [offline]");
	resize(500, 430);
	show();

	// VERY DANGEROUS !!!
	startTimer(30);

	KGGZDEBUGF("KGGZ::KGGZ() ready\n");
}

KGGZ::~KGGZ()
{
	KGGZDEBUGF("KGGZ::~KGGZ()\n");

	dispatcher();

	KGGZDEBUGF("KGGZ::~KGGZ() ready\n");
	exit(0);
}

void KGGZ::slotHandleMenu(int id)
{
	int result;
	GGZCoreGametype *gametype;
	char buffer[2048];
	int number;

	KGGZDEBUGF("KGGZ::slotHandleMenu()\n");
	KGGZDEBUG("got id: %i\n", id);

	switch(id)
	{
		case MENU_GGZ_CONNECT:
			dialogConnection();
			break;
		case MENU_GGZ_DISCONNECT:
			slotDisconnect();
			break;
		case MENU_GGZ_STARTSERVER:
			serverLaunch();
			break;
		case MENU_GGZ_STOPSERVER:
			serverKill();
			break;
   		case MENU_GGZ_QUIT:
			close();
			break;
		case MENU_CLIENT_CHAT:
			m_workspace->widgetChat()->show();
			m_workspace->widgetTables()->show();
			m_workspace->widgetUsers()->show();
			m_workspace->show();
			break;
		case MENU_CLIENT_TABLES:
			m_workspace->widgetChat()->hide();
			m_workspace->widgetTables()->show();
			m_workspace->widgetUsers()->hide();
			m_workspace->show();
			break;
		case MENU_CLIENT_PLAYERS:
			m_workspace->widgetChat()->hide();
			m_workspace->widgetTables()->hide();
			m_workspace->widgetUsers()->show();
			m_workspace->show();
			break;
		case MENU_GAME_LAUNCH:
			if(!kggzroom)
			{
				KMessageBox::information(this, "You cannot launch a game outside a room!", "Error!");
				return;
			}
			if(!m_launch) m_launch = new KGGZLaunch(NULL, "KGGZLaunch");
			m_launch->initLauncher(m_save_username, kggzroom->gametype()->maxPlayers());
			m_launch->show();
			connect(m_launch, SIGNAL(signalLaunch()), SLOT(slotLaunch()));
			break;
		case MENU_GAME_JOIN:
			if((!m_workspace) || (!m_workspace->widgetTables()))
			{
				KGGZDEBUG("Critical! Workspace is broken!\n");
				return;
			}
			if(!kggzroom)
			{
				KMessageBox::information(this, "You must be in a room to join a table!", "Error!");
				return;
			}
			number = m_workspace->widgetTables()->tablenum();
			if(number == -1)
			{
				if(kggzroom->countTables() == 1)
				{
					KGGZDEBUG("Phew... what a luck this is the only table here. Gonna take this!\n");
					number = 0;
				}
				else
				{
					KMessageBox::information(this, "Please select a table to join!", "Error!");
					return;
				}
			}
			kggzroom->joinTable(number);
			break;
		case MENU_GAME_GRUBBY:
			KGGZDEBUG("Aaaaargh.... ;)\n");
			if(kggzserver->dataPending()) kggzserver->dataRead();
			break;
		case MENU_GAME_INFO:
			if(!kggzserver)
			{
				KGGZDEBUG("Critical! No server found.\n");
				return;
			}
			if(!kggzroom)
			{
				//KGGZDEBUG("Critical! No room found.\n");
				KMessageBox::information(this, "Please join a room first.", "Info:");
				return;
			}
			gametype = kggzroom->gametype();
			if(!gametype)
			{
				KGGZDEBUG("Critical! No game type found.\n");
				return;
			}
			KGGZDEBUG("Name: %s\n", gametype->name());
			KGGZDEBUG("Author: %s\n", gametype->author());
			KGGZDEBUG("Version: %s\n", gametype->version());
			KGGZDEBUG("URL: %s\n", gametype->url());
			KGGZDEBUG("Description: %s\n", gametype->description());
			KGGZDEBUG("Protocol: %s\n", gametype->protocol());
			sprintf(buffer, "Name: %s\nDescription: %s\nAuthor: %s\nVersion: %s\nProtocol: %s\nURL: %s",
				gametype->name(), gametype->description(), gametype->author(), gametype->version(), gametype->protocol(), gametype->url());
			KMessageBox::information(this, buffer, "Game Type Information");
			break;
		default:
			if(id >= MENU_ROOMS_SLOTS)
			{
				if(kggzserver)
				{
					kggzserver->joinRoom(id - MENU_ROOMS_SLOTS);
					if((m_workspace) && (m_workspace->widgetChat()))
					{
						m_workspace->widgetChat()->init();
						m_workspace->show();
					}
					else KGGZDEBUG("Critical: Workspace/Chat absent!\n");
				}
				else KGGZDEBUG("Critical: No server found.\n");
			}
	}
	KGGZDEBUGF("KGGZ::slotHandleMenu() ready\n");
}

void KGGZ::resizeEvent(QResizeEvent *e)
{
	resize(e->size());
	m_workspace->setGeometry(0, 30, e->size().width(), e->size().height() - 30);
	m_menu->setGeometry(0, 0, e->size().width(), 20);
}

void KGGZ::slotConnect(const char *host, int port, const char *username, const char *password, int mode, int server)
{
	int result;

	KGGZDEBUGF("KGGZ::slotConnect()\n");
	delete m_connect;
	m_connect = NULL;

	if(server)
	{
		KGGZDEBUG("Start server\n");
		host = "localhost";
		serverLaunch();
	}

	KGGZDEBUG("Connect with: host=%s port=%i username=%s password=%s mode=%i\n", host, port, username, password, mode);

	m_save_loginmode = mode;
	m_save_username = strdup(username);
	m_save_password = strdup(password);
	m_save_host = strdup(host);
	m_save_port = port;
	KGGZDEBUG("Values are: username=%s password=%s mode=%i\n", m_save_username, m_save_password, m_save_loginmode);

	kggzserver = new GGZCoreServer();
	attachServerCallbacks();

	kggzserver->setHost(host, port);
	result = kggzserver->connect();
	if(result == -1)
	{
		if(kggzserver)
		{
			detachServerCallbacks();
			delete kggzserver;
			kggzserver = NULL;
			KMessageBox::error(this, i18n("Attempt to connect refused!"), "Error!");
		}
		return;
	}
}

void KGGZ::slotDisconnect()
{
	KGGZDEBUGF("KGGZ::slotDisconnect()\n");

	if(!kggzserver)
	{
		KGGZDEBUG("Critical: No server found!\n");
		return;
	}
	if((!kggzserver->isLoggedIn()) || (!kggzserver->isOnline()))
	{
		KGGZDEBUG("Critical: Not logged in or connected!\n");
		return;
	}

	m_save_rooms = kggzserver->countRooms();
	kggzserver->logout();
}

void KGGZ::serverLaunch()
{
	int result;
	GGZCoreConfio *config;
	char *process;

	config = new GGZCoreConfio(KGGZCommon::append(getenv("HOME"), "/.ggz/kggz.rc"), GGZCoreConfio::readwrite | GGZCoreConfio::create);
    KGGZCommon::clear();
    process = config->read("Environment", "Server", (char*)NULL);
	delete config;

	if(!process)
	{
		KMessageBox::information(this, i18n("GGZ Gaming Zone server not found.\n"
			"Please get it from http://ggz.sourceforge.net.\n"
			"Make sure to add its path to kggz.rc!\n"
			"See help for details.\n"), "Information");
		return;
	}

	result = KGGZCommon::launchProcess("ggzd", process);

	switch(result)
	{
		case -1:
			KMessageBox::error(this, i18n("Could not start ggzd!"), "Error!");
			break;
		case -2:
			KMessageBox::error(this, i18n("The ggzd server is already running!"), "Error!");
			break;
		default:
			m_menu_ggz->setItemEnabled(MENU_GGZ_STARTSERVER, FALSE);
			m_menu_ggz->setItemEnabled(MENU_GGZ_STOPSERVER, TRUE);
			break;
	}
}

void KGGZ::serverKill()
{
	int result;

	result = KGGZCommon::killProcess("ggzd");
	switch(result)
	{
		case -1:
			KMessageBox::error(this, i18n("An error occured! Sure it runs? Sure you don't need to be root?"), i18n("Error!"));
			break;
		case 0:
			KMessageBox::error(this, i18n("Could only kill server via sigkill!"), i18n("Error!"));
			m_menu_ggz->setItemEnabled(MENU_GGZ_STARTSERVER, TRUE);
			m_menu_ggz->setItemEnabled(MENU_GGZ_STOPSERVER, FALSE);
			break;
		default:
			m_menu_ggz->setItemEnabled(MENU_GGZ_STARTSERVER, TRUE);
			m_menu_ggz->setItemEnabled(MENU_GGZ_STOPSERVER, FALSE);
			break;
	}
}

void KGGZ::dialogConnection()
{
	if(!m_connect) m_connect = new KGGZConnect(NULL, "connect");
	m_connect->show();
	connect(m_connect, SIGNAL(signalConnect(const char*, int, const char*, const char*, int, int)), SLOT(slotConnect(const char*, int, const char*, const char*, int, int)));
}

void KGGZ::dispatch_free(char *var, char *description)
{
	if(var)
	{
		KGGZDEBUG("- %s\n", description);
		free(var);
		var = NULL;
	}
}

void KGGZ::dispatch_delete(void *object, char *description)
{
	if(object)
	{
		KGGZDEBUG("- %s\n", description);
		delete object;
		object = NULL;
	}
}

void KGGZ::dispatcher()
{
	KGGZDEBUGF("KGGZ::Dispatcher()\n");
	KGGZDEBUG("Deleting objects...\n");
	dispatch_delete((void*)m_connect, "connection dialog");
	dispatch_delete((void*)kggzroom, "room");
	dispatch_delete((void*)kggzserver, "server");
	dispatch_delete((void*)kggzroomcallback, "room callback");
	dispatch_delete((void*)kggzservercallback, "server callback");
	dispatch_delete((void*)kggzgamecallback, "game callback");
	dispatch_delete((void*)m_config, "GGZCore++ configuration object");
	dispatch_delete((void*)m_core, "GGZCore++ core object");
	dispatch_free(m_save_username, "stored user name");
	dispatch_free(m_save_password, "stored password");
	dispatch_free(m_save_host, "stored host name");
	KGGZDEBUGF("KGGZ::Dispatcher() ready\n");
}

void KGGZ::timerEvent(QTimerEvent *e)
{
	if(m_lock) return;
	if(!kggzserver) return;

	if(kggzgame)
	{
		if(kggzgame->dataPending()) kggzgame->dataRead();
	}

	if((!m_lock) && (kggzserver))
	{
		while(kggzserver->dataPending())
		{
			if(!kggzserver) return;
			kggzserver->dataRead();
			if(!kggzserver) return;
		}
	}
}

void KGGZ::listTables()
{
	int number;
	GGZCoreTable *table;
	GGZCoreGametype *gametype;
	int tableseats, tableseatsopen;
	char *tabledescription;
	char *playername;

	KGGZDEBUGF("KGGZ::listTables()\n");
	if(!kggzroom)
	{
		KGGZDEBUG("Critical! We are not in a room, are we?\n");
		return;
	}
	if(!m_workspace)
	{
		KGGZDEBUG("Feurio! No workspace here!\n");
		return;
	}
	gametype = kggzroom->gametype();
	if(!gametype)
	{
		KGGZDEBUG("Critical: Error when fetching game type!\n");
		return;
	}
	m_workspace->widgetTables()->reset();
	number = kggzroom->countTables();
	for(int i = 0; i < number; i++)
	{
		m_workspace->widgetUsers()->addTable(i);
		table = kggzroom->table(i);
		tabledescription = table->description();
		tableseats = table->countSeats();
		tableseatsopen = 0;
		for(int j = 0; j < tableseats; j++)
		{
			if(table->playerType(j) == GGZ_SEAT_OPEN) tableseatsopen++;
			else
			{
				if(table->playerType(j) == GGZ_SEAT_PLAYER)
				{
					playername = table->playerName(j);
					if(!playername) playername = "Bot";
					if(strlen(playername) == 0)
					{
						playername = "(open)";
						tableseatsopen++;
					}
					KGGZDEBUG("Going to add: %s\n", playername);
					m_workspace->widgetUsers()->addTablePlayer(i, playername);
				}
			}
		}
		KGGZDEBUG(" // table: %s (%i/%i)\n", tabledescription, tableseats - tableseatsopen, tableseats);
		m_workspace->widgetTables()->add(gametype->name(), tabledescription, tableseats - tableseatsopen, tableseats);
	}
}

void KGGZ::listPlayers()
{
	int number;
	GGZPlayer *player;
	char *playername;

	KGGZDEBUGF("KGGZ::listPlayers()\n");
	m_workspace->widgetUsers()->removeall();
	KGGZDEBUG("* Removed all, now adding anew...\n");
	number = kggzroom->countPlayers();
	for(int i = 0; i < number; i++)
	{
		player = kggzroom->player(i);
		playername = ggzcore_player_get_name(player);
		KGGZDEBUG(" # player: %s\n", playername);
		m_workspace->widgetUsers()->add(playername);
	}
}

void KGGZ::gameCollector(unsigned int id, void* data)
{
	switch(id)
	{
		case GGZCoreGame::launched:
			KGGZDEBUG("launched\n");
			break;
		case GGZCoreGame::launchfail:
			KGGZDEBUG("launchfail\n");
			break;
		case GGZCoreGame::negotiated:
			KGGZDEBUG("negotiated\n");
			break;
		case GGZCoreGame::negotiatefail:
			KGGZDEBUG("negotiatefail\n");
			break;
		case GGZCoreGame::data:
			KGGZDEBUG("data\n");
			/*if(!kggzgame)
			{
				KGGZDEBUG("Panic! No game running!\n");
				return;
			}
			kggzgame->dataSend((char*)data);*/
			if(!kggzroom)
			{
				KGGZDEBUG("Panic! No room found!\n");
				return;
			}
			kggzroom->sendData((char*)data);
			break;
		case GGZCoreGame::over:
			KGGZDEBUG("over\n");
			if(!kggzgame)
			{
				KGGZDEBUG("Now I'm confused: What do they want from me?\n");
				return;
			}
			detachGameCallbacks();
			delete kggzgame;
			kggzgame = NULL;
			listPlayers();
			listTables();
			break;
		case GGZCoreGame::ioerror:
			KGGZDEBUG("ioerror\n");
			break;
		case GGZCoreGame::protoerror:
			KGGZDEBUG("protoerror\n");
			break;
		default:
			KGGZDEBUG("unknown!\n");
			break;
	}
}

void KGGZ::roomCollector(unsigned int id, void* data)
{
	char *chatsender, *chatmessage;

	switch(id)
	{
		case GGZCoreRoom::chatnormal:
		case GGZCoreRoom::chatannounce:
		case GGZCoreRoom::chatprivate:
		case GGZCoreRoom::chatbeep:
			chatsender = ((char**)data)[0];
			chatmessage = ((char**)data)[1];
			KGGZDEBUG("Chat receives: %s from %s\n", chatmessage, chatsender);
			if((!m_workspace) || (!m_workspace->widgetChat()))
			{
				KGGZDEBUG("Critical: Workspace/Chat absent!\n");
				return;
			}
			break;
	}

	switch(id)
	{
		case GGZCoreRoom::playerlist:
			KGGZDEBUG("playerlist\n");
			listPlayers();
			break;
		case GGZCoreRoom::tablelist:
			KGGZDEBUG("tablelist\n");
			listTables();
			break;
		case GGZCoreRoom::chatnormal:
			KGGZDEBUG("chatnormal\n");
			m_workspace->widgetChat()->receive(chatsender, chatmessage, KGGZChat::RECEIVE_CHAT);
			break;
		case GGZCoreRoom::chatannounce:
			KGGZDEBUG("chatnormal\n");
			m_workspace->widgetChat()->receive(chatsender, chatmessage, KGGZChat::RECEIVE_OWN);
			break;
		case GGZCoreRoom::chatprivate:
			KGGZDEBUG("chatnormal\n");
			m_workspace->widgetChat()->receive(chatsender, chatmessage, KGGZChat::RECEIVE_INFO);
			break;
		case GGZCoreRoom::chatbeep:
			KGGZDEBUG("chatnormal\n");
			m_workspace->widgetChat()->beep();
			break;
		case GGZCoreRoom::enter:
			KGGZDEBUG("enter\n");
			m_workspace->widgetUsers()->add((char*)data);
			break;
		case GGZCoreRoom::leave:
			KGGZDEBUG("leave\n");
			m_workspace->widgetUsers()->remove((char*)data);
			break;
		case GGZCoreRoom::tableupdate:
			KGGZDEBUG("tableupdate\n");
			m_workspace->widgetUsers()->removeall();
			listPlayers();
			listTables();
			break;
		case GGZCoreRoom::tablelaunched:
			KGGZDEBUG("tablelaunched\n");
			m_workspace->widgetUsers()->removeall();
			listPlayers();
			listTables();
			break;
		case GGZCoreRoom::tablelaunchfail:
			KGGZDEBUG("tablelaunchfail\n");
			KMessageBox::information(this, "Couldn't launch table!", "Error!");
			listPlayers();
			listTables();
			break;
		case GGZCoreRoom::tablejoined:
			KGGZDEBUG("tablejoined\n");
			m_workspace->widgetUsers()->removeall();
			listPlayers();
			listTables();
			slotLaunchGame(kggzroom->gametype());
			break;
		case GGZCoreRoom::tablejoinfail:
			KGGZDEBUG("tablejoinfail\n");
			KMessageBox::information(this, "Couldn't join table!", "Error");
			listPlayers();
			listTables();
			break;
		case GGZCoreRoom::tableleft:
			KGGZDEBUG("tableleft\n");
			m_workspace->widgetUsers()->removeall();
			listPlayers();
			listTables();
			break;
		case GGZCoreRoom::tableleavefail:
			KGGZDEBUG("tableleavefail\n");
			break;
		case GGZCoreRoom::tabledata:
			KGGZDEBUG("tabledata\n");
			if(!kggzgame)
			{
				KGGZDEBUG("Don't wanna bother, but... we don't have any game running!!\n");
				//return;
				kggzroom->sendData((char*)data);
			}
			else
			{
				KGGZDEBUG("What a luck a game is running ;)\n");
				kggzgame->dataSend((char*)data);
			}
			break;
		default:
			KGGZDEBUG("unknown\n");
			break;
	}
}

void KGGZ::serverCollector(unsigned int id, void* data)
{
	int result;
	GGZCoreServer *lock;
	char buffer[1024];

	switch(id)
	{
		case GGZCoreServer::connected:
			KGGZDEBUG("connected\n");
			// possibly ggzcore bug:
			// state has not been updated yet here
			while(!kggzserver->isOnline()) kggzserver->dataRead();
			kggzserver->setLogin(m_save_loginmode, m_save_username, m_save_password);
			result = kggzserver->login();
			if(result == -1)
			{
				detachServerCallbacks();
				delete kggzserver;
				kggzserver = NULL;
				KMessageBox::error(this, i18n("Attempt to login refused!"), "Error!");
				dialogConnection();
				return;
			}
			break;
		case GGZCoreServer::connectfail:
			KGGZDEBUG("connectfail\n");
			detachServerCallbacks();
			delete kggzserver;
			kggzserver = NULL;
			KMessageBox::error(this, i18n("Couldn't connect to server!"), "Error!");
			dialogConnection();
			break;
		case GGZCoreServer::negotiated:
			KGGZDEBUG("negotiated\n");
			break;
		case GGZCoreServer::negotiatefail:
			KGGZDEBUG("negotiatefail\n");
			break;
		case GGZCoreServer::loggedin:
			KGGZDEBUG("loggedin\n");
			m_menu_client->setItemEnabled(KGGZ::MENU_CLIENT_CHAT, TRUE);
			m_menu_client->setItemEnabled(KGGZ::MENU_CLIENT_TABLES, TRUE);
			m_menu_client->setItemEnabled(KGGZ::MENU_CLIENT_PLAYERS, TRUE);
			m_menu_ggz->setItemEnabled(MENU_GGZ_CONNECT, FALSE);
			m_menu_ggz->setItemEnabled(MENU_GGZ_DISCONNECT, TRUE);
			sprintf(buffer, "KGGZ - [logged in as %s@%s:%i]", m_save_username, m_save_host, m_save_port);
			setCaption(buffer);
			if(m_save_loginmode == GGZCoreServer::firsttime)
			{
				KGGZDEBUG("First time login!\n");
				sprintf(buffer, "You are welcome as a new GGZ Gaming Zone player.\n"
					"Your personal password is: %s", kggzserver->password());
				KMessageBox::information(this, buffer, "Information");
			}
			break;
		case GGZCoreServer::loginfail:
			KGGZDEBUG("loginfail\n");
			KMessageBox::error(this, i18n("Login refused!"), "Error!");
			dialogConnection();
			break;
		case GGZCoreServer::motdloaded:
			KGGZDEBUG("motdloaded\n");
			// load motd here
			kggzserver->listRooms(-1, 0);
			if(kggzserver->listGames(1) != 0) // NEVER use 0 here, it will hang the client !!!
			{
				KGGZDEBUG("HUH? Don't give me game type list?!\n");
				return;
			}
			break;
		case GGZCoreServer::roomlist:
			KGGZDEBUG("roomlist\n");
			m_menu_rooms->setEnabled(TRUE);
			m_menu_game->setEnabled(TRUE);
			for(int i = 0; i < kggzserver->countRooms(); i++)
			{
				KGGZDEBUG("Found: %s\n", kggzserver->room(i)->name());
				m_menu_rooms->insertItem(kggzGetIcon(MENU_ROOMS_SLOTS + i), kggzserver->room(i)->name(), MENU_ROOMS_SLOTS + i);
			}
			break;
		case GGZCoreServer::typelist:
			KGGZDEBUG("typelist\n");
			break;
		case GGZCoreServer::entered:
			KGGZDEBUG("entered\n");
			KGGZDEBUG("==> creating room object\n");
			m_lock = 1;
			if(kggzroom)
			{
				detachRoomCallbacks();
				delete kggzroom;
				kggzroom = NULL;
				//kggzserver->resetRoom(); // This is damned required!!!! -> update: no :-) does now work automatically
				m_workspace->widgetUsers()->removeall();
			}
			kggzroom = kggzserver->room();
			//kggzroom = new GGZCoreRoom(ggzcore_server_get_cur_room(kggzserver->server()));
			attachRoomCallbacks();
			m_lock = 0;
			kggzroom->listPlayers();
			kggzroom->listTables(-1, 0);
			break;
		case GGZCoreServer::enterfail:
			KGGZDEBUG("enterfail\n");
			break;
		case GGZCoreServer::loggedout:
			KGGZDEBUG("loggedout\n");
			m_lock = 1;
			if(kggzroom)
			{
				detachRoomCallbacks();
				delete kggzroom;
				kggzroom = NULL;
				m_workspace->widgetUsers()->removeall();
				m_workspace->widgetTables()->reset();
				m_workspace->widgetChat()->shutdown();
			}
			detachServerCallbacks();
			delete kggzserver;
			kggzserver = NULL;
			m_lock = 0;
			KGGZDEBUG("Disconnection successful.\n");
 			for(int i = 0; i < m_save_rooms; i++) m_menu_rooms->removeItemAt(0);
			m_save_rooms = 0;
			m_menu_rooms->setEnabled(FALSE);
			m_menu_ggz->setItemEnabled(MENU_GGZ_CONNECT, TRUE);
			m_menu_ggz->setItemEnabled(MENU_GGZ_DISCONNECT, FALSE);
			m_menu_client->setItemEnabled(KGGZ::MENU_CLIENT_CHAT, FALSE);
			m_menu_client->setItemEnabled(KGGZ::MENU_CLIENT_TABLES, FALSE);
			m_menu_client->setItemEnabled(KGGZ::MENU_CLIENT_PLAYERS, FALSE);
			m_menu_game->setEnabled(FALSE);
			setCaption("KGGZ - [offline]");
			break;
		case GGZCoreServer::neterror:
			KGGZDEBUG("neterror\n");
			break;
		case GGZCoreServer::protoerror:
			KGGZDEBUG("protoerror\n");
			break;
		case GGZCoreServer::chatfail:
			KGGZDEBUG("chatfail\n");
			break;
		case GGZCoreServer::statechange:
			KGGZDEBUG("statechange\n");
			KGGZDEBUG("State is now: %s\n", KGGZCommon::state(kggzserver->state()));
			if(kggzserver->state() == GGZ_STATE_IN_ROOM)
			{
				m_workspace->widgetLogo()->setBackgroundPixmap(QPixmap(NULL));
			}
			break;
		default:
			KGGZDEBUG("unknown\n");
			break;
	}
}

GGZHookReturn KGGZ::hookOpenCollector(unsigned int id, void* event_data, void* user_data)
{
	KGGZCallback* kggzcallback;
	KGGZ* kggz;
	int opcode;

	KGGZDEBUGF("KGGZ::hookOpenCollector()\n");

	KGGZDEBUG("Received a KGGZCallback atom\n");
	kggzcallback = (KGGZCallback*)user_data;

	kggz = (KGGZ*)kggzcallback->pointer();
	opcode = kggzcallback->opcode();

	if(kggz == NULL)
	{
		KGGZDEBUG("KGGZ is NULL!\n");
		return GGZ_HOOK_OK;
	}

	switch(opcode)
	{
		case COLLECTOR_SERVER:
			KGGZDEBUG("atom event: server\n");
			kggz->serverCollector(id, event_data);
			break;
		case COLLECTOR_ROOM:
			KGGZDEBUG("atom event: room\n");
			kggz->roomCollector(id, event_data);
			break;
		case COLLECTOR_GAME:
			KGGZDEBUG("atom event: game\n");
			kggz->gameCollector(id, event_data);
			break;
		default:
			KGGZDEBUG("atom event: unknown\n");
			break;
	}
}

void KGGZ::slotChat(char *text)
{
	if((kggzserver) && (kggzroom))
	{
		KGGZDEBUG("Chat sends: %s\n", text);
		kggzroom->chat(GGZ_CHAT_NORMAL, NULL, strdup(text));
	}
	else KGGZDEBUG("Critical: No server or room found!\n");
}


void KGGZ::attachRoomCallbacks()
{
	KGGZDEBUG("== attaching hooks to room at %i\n", kggzroom);
	kggzroom->addHook(GGZCoreRoom::playerlist, &KGGZ::hookOpenCollector, (void*)kggzroomcallback);
	kggzroom->addHook(GGZCoreRoom::tablelist, &KGGZ::hookOpenCollector, (void*)kggzroomcallback);
	kggzroom->addHook(GGZCoreRoom::chatnormal, &KGGZ::hookOpenCollector, (void*)kggzroomcallback);
	kggzroom->addHook(GGZCoreRoom::chatannounce, &KGGZ::hookOpenCollector, (void*)kggzroomcallback);
	kggzroom->addHook(GGZCoreRoom::chatprivate, &KGGZ::hookOpenCollector, (void*)kggzroomcallback);
	kggzroom->addHook(GGZCoreRoom::chatbeep, &KGGZ::hookOpenCollector, (void*)kggzroomcallback);
	kggzroom->addHook(GGZCoreRoom::enter, &KGGZ::hookOpenCollector, (void*)kggzroomcallback);
	kggzroom->addHook(GGZCoreRoom::leave, &KGGZ::hookOpenCollector, (void*)kggzroomcallback);
	kggzroom->addHook(GGZCoreRoom::tableupdate, &KGGZ::hookOpenCollector, (void*)kggzroomcallback);
	kggzroom->addHook(GGZCoreRoom::tablelaunched, &KGGZ::hookOpenCollector, (void*)kggzroomcallback);
	kggzroom->addHook(GGZCoreRoom::tablelaunchfail, &KGGZ::hookOpenCollector, (void*)kggzroomcallback);
	kggzroom->addHook(GGZCoreRoom::tablejoined, &KGGZ::hookOpenCollector, (void*)kggzroomcallback);
	kggzroom->addHook(GGZCoreRoom::tablejoinfail, &KGGZ::hookOpenCollector, (void*)kggzroomcallback);
	kggzroom->addHook(GGZCoreRoom::tableleft, &KGGZ::hookOpenCollector, (void*)kggzroomcallback);
	kggzroom->addHook(GGZCoreRoom::tableleavefail, &KGGZ::hookOpenCollector, (void*)kggzroomcallback);
	kggzroom->addHook(GGZCoreRoom::tabledata, &KGGZ::hookOpenCollector, (void*)kggzroomcallback);
	KGGZDEBUG("== done\n");
}

void KGGZ::detachRoomCallbacks()
{
	KGGZDEBUG("== detaching hooks from room at %i\n", kggzroom);
	kggzroom->removeHook(GGZCoreRoom::playerlist, &KGGZ::hookOpenCollector);
	kggzroom->removeHook(GGZCoreRoom::tablelist, &KGGZ::hookOpenCollector);
	kggzroom->removeHook(GGZCoreRoom::chatnormal, &KGGZ::hookOpenCollector);
	kggzroom->removeHook(GGZCoreRoom::chatannounce, &KGGZ::hookOpenCollector);
	kggzroom->removeHook(GGZCoreRoom::chatprivate, &KGGZ::hookOpenCollector);
	kggzroom->removeHook(GGZCoreRoom::chatbeep, &KGGZ::hookOpenCollector);
	kggzroom->removeHook(GGZCoreRoom::enter, &KGGZ::hookOpenCollector);
	kggzroom->removeHook(GGZCoreRoom::leave, &KGGZ::hookOpenCollector);
	kggzroom->removeHook(GGZCoreRoom::tableupdate, &KGGZ::hookOpenCollector);
	kggzroom->removeHook(GGZCoreRoom::tablelaunched, &KGGZ::hookOpenCollector);
	kggzroom->removeHook(GGZCoreRoom::tablelaunchfail, &KGGZ::hookOpenCollector);
	kggzroom->removeHook(GGZCoreRoom::tablejoined, &KGGZ::hookOpenCollector);
	kggzroom->removeHook(GGZCoreRoom::tablejoinfail, &KGGZ::hookOpenCollector);
	kggzroom->removeHook(GGZCoreRoom::tableleft, &KGGZ::hookOpenCollector);
	kggzroom->removeHook(GGZCoreRoom::tableleavefail, &KGGZ::hookOpenCollector);
	kggzroom->removeHook(GGZCoreRoom::tabledata, &KGGZ::hookOpenCollector);
	KGGZDEBUG("== done\n");
}

void KGGZ::attachServerCallbacks()
{
	KGGZDEBUG("=== attaching server hooks\n");
	kggzserver->addHook(GGZCoreServer::connected, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	kggzserver->addHook(GGZCoreServer::connectfail, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	kggzserver->addHook(GGZCoreServer::negotiated, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	kggzserver->addHook(GGZCoreServer::negotiatefail, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	kggzserver->addHook(GGZCoreServer::loggedin, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	kggzserver->addHook(GGZCoreServer::loginfail, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	kggzserver->addHook(GGZCoreServer::motdloaded, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	kggzserver->addHook(GGZCoreServer::roomlist, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	kggzserver->addHook(GGZCoreServer::typelist, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	kggzserver->addHook(GGZCoreServer::entered, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	kggzserver->addHook(GGZCoreServer::enterfail, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	kggzserver->addHook(GGZCoreServer::loggedout, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	kggzserver->addHook(GGZCoreServer::neterror, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	kggzserver->addHook(GGZCoreServer::protoerror, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	kggzserver->addHook(GGZCoreServer::chatfail, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	kggzserver->addHook(GGZCoreServer::statechange, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	KGGZDEBUG("=== done\n");
}

void KGGZ::detachServerCallbacks()
{
	KGGZDEBUG("=== detaching hooks from server\n");
	kggzserver->removeHook(GGZCoreServer::connected, &KGGZ::hookOpenCollector);
	kggzserver->removeHook(GGZCoreServer::connectfail, &KGGZ::hookOpenCollector);
	kggzserver->removeHook(GGZCoreServer::negotiated, &KGGZ::hookOpenCollector);
	kggzserver->removeHook(GGZCoreServer::negotiatefail, &KGGZ::hookOpenCollector);
	kggzserver->removeHook(GGZCoreServer::loggedin, &KGGZ::hookOpenCollector);
	kggzserver->removeHook(GGZCoreServer::loginfail, &KGGZ::hookOpenCollector);
	kggzserver->removeHook(GGZCoreServer::motdloaded, &KGGZ::hookOpenCollector);
	kggzserver->removeHook(GGZCoreServer::roomlist, &KGGZ::hookOpenCollector);
	kggzserver->removeHook(GGZCoreServer::typelist, &KGGZ::hookOpenCollector);
	kggzserver->removeHook(GGZCoreServer::entered, &KGGZ::hookOpenCollector);
	kggzserver->removeHook(GGZCoreServer::enterfail, &KGGZ::hookOpenCollector);
	kggzserver->removeHook(GGZCoreServer::loggedout, &KGGZ::hookOpenCollector);
	kggzserver->removeHook(GGZCoreServer::neterror, &KGGZ::hookOpenCollector);
	kggzserver->removeHook(GGZCoreServer::protoerror, &KGGZ::hookOpenCollector);
	kggzserver->removeHook(GGZCoreServer::chatfail, &KGGZ::hookOpenCollector);
	kggzserver->removeHook(GGZCoreServer::statechange, &KGGZ::hookOpenCollector);
	KGGZDEBUG("=== done\n");
}

void KGGZ::attachGameCallbacks()
{
	KGGZDEBUG("=== attaching game hooks\n");
	kggzgame->addHook(GGZCoreGame::launched, &KGGZ::hookOpenCollector, (void*)kggzgamecallback);
	kggzgame->addHook(GGZCoreGame::launchfail, &KGGZ::hookOpenCollector, (void*)kggzgamecallback);
	kggzgame->addHook(GGZCoreGame::negotiated, &KGGZ::hookOpenCollector, (void*)kggzgamecallback);
	kggzgame->addHook(GGZCoreGame::negotiatefail, &KGGZ::hookOpenCollector, (void*)kggzgamecallback);
	kggzgame->addHook(GGZCoreGame::data, &KGGZ::hookOpenCollector, (void*)kggzgamecallback);
	kggzgame->addHook(GGZCoreGame::over, &KGGZ::hookOpenCollector, (void*)kggzgamecallback);
	kggzgame->addHook(GGZCoreGame::ioerror, &KGGZ::hookOpenCollector, (void*)kggzgamecallback);
	kggzgame->addHook(GGZCoreGame::protoerror, &KGGZ::hookOpenCollector, (void*)kggzgamecallback);
	KGGZDEBUG("=== done\n");
}

void KGGZ::detachGameCallbacks()
{
	KGGZDEBUG("=== detaching hooks from game\n");
	kggzgame->removeHook(GGZCoreGame::launched, &KGGZ::hookOpenCollector);
	kggzgame->removeHook(GGZCoreGame::launchfail, &KGGZ::hookOpenCollector);
	kggzgame->removeHook(GGZCoreGame::negotiated, &KGGZ::hookOpenCollector);
	kggzgame->removeHook(GGZCoreGame::negotiatefail, &KGGZ::hookOpenCollector);
	kggzgame->removeHook(GGZCoreGame::data, &KGGZ::hookOpenCollector);
	kggzgame->removeHook(GGZCoreGame::over, &KGGZ::hookOpenCollector);
	kggzgame->removeHook(GGZCoreGame::ioerror, &KGGZ::hookOpenCollector);
	kggzgame->removeHook(GGZCoreGame::protoerror, &KGGZ::hookOpenCollector);
	KGGZDEBUG("=== done\n");
}

void KGGZ::slotLaunchGame(GGZCoreGametype *gametype)
{
	GGZCoreModule *module;
	int i;
	int ret;
	char *icon;

    KGGZDEBUG("Create module...\n");
    module = new GGZCoreModule();
    module->init(gametype->name(), gametype->protocol());
    i = module->count();
    KGGZDEBUG("Found: %i modules for this game\n", i);
    if(i == 0)
    {
        KMessageBox::information(this, "Sorry, no modules found for this game.", "Error!");
        delete module;
        return;
    }
    module->setActive(0);
    //module->launch();

	if(kggzgame)
    {
        KGGZDEBUG("Found game object alive; killing it now :-)\n");
        delete kggzgame;
    }
    kggzgame = new GGZCoreGame();
    kggzgame->init(module->module());
 
    attachGameCallbacks(); // don't forget detaching!

    ret = kggzgame->launch();
    if(ret < 0)
    {
        KMessageBox::information(this, "Couldn't launch game!", "Error!");
        return;
    }
 
    icon = module->pathIcon();
    KGGZDEBUG("Found module icon: %s\n", icon);
    if(!icon) icon = KGGZ_DIRECTORY "/images/icons/module.png";
    m_workspace->widgetLogo()->setBackgroundPixmap(QPixmap(icon));

	delete module;
}

void KGGZ::slotLaunch()
{
	GGZCoreTable *table;
	GGZCoreGametype *gametype;
	GGZCoreModule *module;
	int seats;
	char *description;
	int i;
	int ret;

	if(!kggzroom)
	{
		KGGZDEBUG("Baby I shrunk the rooms list!\n");
		return;
	}
	if(!m_launch)
	{
		KGGZDEBUG("Huh?\n");
		return;
	}

	gametype = kggzroom->gametype();
	if((!gametype) || (!gametype->gametype()))
	{
		KGGZDEBUG("Erm... would you mind to give me the game type?\n");
		return;
	}

	//slotLaunchGame(gametype);

	description = (char*)m_launch->description();
	seats = m_launch->seats();

	table = new GGZCoreTable();
	table->init(gametype->gametype(), description, seats);
	for(int i = 0; i < seats; i++)
	{
		switch(m_launch->seatType(i))
		{
			case KGGZLaunch::seatplayer:
				KGGZDEBUG("* %i: player\n", i);
				table->addPlayer(m_save_username, i);
				break;
			case KGGZLaunch::seatopen:
				KGGZDEBUG("* %i: open\n", i);
				break;
			case KGGZLaunch::seatbot:
				KGGZDEBUG("* %i: bot\n");
				table->addBot(NULL, i);
				break;
			case KGGZLaunch::seatreserved:
				KGGZDEBUG("* %i: reserved\n", i);
				table->addReserved("RESERVED", i);
				break;
			case KGGZLaunch::seatunused:
				KGGZDEBUG("* %i: unused.\n", i);
				break;
			case KGGZLaunch::seatunknown:
			default:
				KGGZDEBUG("* %i: unknown!\n", i);
		}
	}

	ret = kggzroom->launchTable(table->table());
	delete table;

	delete m_launch;
	m_launch = NULL;

	if(ret < 0)
	{
		KMessageBox::information(this, "Failed launching table!", "Error!");
		return;
	}
}

void KGGZ::slotAboutGGZ()
{
	KGGZDEBUGF("KGGZ::slotAboutGGZ()\n");
	KGGZDEBUG("About it... hm, it's pretty cool and open for everyone!\n");
	if(!m_about) m_about = new KGGZAbout(NULL, "KGGZAbout");
	m_about->show();
}
