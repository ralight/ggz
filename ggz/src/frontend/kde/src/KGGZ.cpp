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
// KGGZ: The main GGZ Gaming Zone object. Controls servers, rooms and games/rooms. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

// Header file
#include "KGGZ.h"

// KGGZ includes
#include "KGGZWorkspace.h"
#include "KGGZSplash.h"
#ifdef KGGZ_BROWSER
#ifndef __STRICT_ANSI__
#warning You are building the internal browser which sloooows things down :)
#endif
#include "KGGZBrowser.h"
#endif
#include "KGGZGrubby.h"
#include "KGGZPrefEnv.h"
#include "KGGZSelector.h"

// KDE includes
#include <kmessagebox.h>
#include <klocale.h>

// Qt includes
#include <qiconview.h>
#include <qlayout.h>

// System includes
#include <stdio.h>
#include <iostream.h>
#include <stdlib.h>

// GGZCore++ includes
#include "GGZCoreConfio.h"

KGGZ::KGGZ(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	int result;
	QLayout *vbox;

	KGGZDEBUGF("KGGZ::KGGZ()\n");
	KGGZDEBUG("Starting...\n");

	kggzroom = NULL;
	kggzserver = NULL;
	m_save_username = NULL;
	m_save_password = NULL;
	m_save_host = NULL;
	m_lock = 0;
	m_launch = NULL;
	kggzgame = NULL;
#ifdef KGGZ_BROWSER
	m_browser = NULL;
#endif
	m_motd = NULL;
	m_grubby = NULL;
	m_prefenv = NULL;
	m_selector = NULL;
	m_module = NULL;
	m_table = NULL;
	m_gameinfo = NULL;
	m_connect = NULL;
	m_killserver = 0;
	m_dns = NULL;

	setBackgroundColor(QColor(0, 0, 0));

	m_splash = new KGGZSplash(this, "splash");

#ifdef KGGZ_BROWSER
	m_browser = new KGGZBrowser(this, "browser");
#endif

	m_workspace = new KGGZWorkspace(this, "workspace");

	connect(m_workspace->widgetChat(), SIGNAL(signalChat(const char *, char *, int)), SLOT(slotChat(const char *, char *, int)));
	connect(m_workspace->widgetLogo(), SIGNAL(signalInfo()), SLOT(menuGameInfo()));

	KGGZDEBUG("Initializing GGZCore...\n");
	m_core = new GGZCore();
	result = m_core->init(GGZCore::parser | GGZCore::modules, /*"/tmp/kggz.debug"*/NULL, GGZCore::all); // make a debug log configurable!
	KGGZCommon::clear();
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
			KGGZDEBUG("- load connection dialog\n");
			menuConnect();
		}
		if(m_config->read("Preferences", "Chatlog", 0))
		{
			KGGZDEBUG("- enable chat logging\n");
			m_workspace->widgetChat()->setLogging(1);
		}
		if(m_config->read("Preferences", "Speech", 0))
		{
			m_workspace->widgetChat()->setSpeech(1);
		}
	}

	kggzroomcallback = new KGGZCallback(this, COLLECTOR_ROOM);
	kggzservercallback = new KGGZCallback(this, COLLECTOR_SERVER);
	kggzgamecallback = new KGGZCallback(this, COLLECTOR_GAME);

	vbox = new QVBoxLayout(this);
	vbox->add(m_splash);
#ifdef KGGZ_BROWSER
	vbox->add(m_browser);
#endif
	vbox->add(m_workspace);

	// VERY DANGEROUS !!!
	startTimer(40);

	KGGZDEBUGF("KGGZ::KGGZ() ready\n");
}

KGGZ::~KGGZ()
{
	KGGZDEBUGF("KGGZ::~KGGZ()\n");

	dispatcher();

	KGGZDEBUGF("KGGZ::~KGGZ() ready\n");
	exit(0);
}

void KGGZ::resizeEvent(QResizeEvent *e)
{
	m_workspace->resize(e->size());
	//resize(e->size());
	//m_workspace->setGeometry(0, 30, e->size().width(), e->size().height() - 30);
	//m_menu->setGeometry(0, 0, e->size().width(), 20);
}

void KGGZ::slotConnected(const char *host, int port, const char *username, const char *password, int mode)
{
	KGGZDEBUGF("KGGZ::slotConnected()\n");
	/*delete m_connect;
	m_connect = NULL;*/

	if(m_connect->optionServer())
	{
		KGGZDEBUG("Start server\n");
		host = "localhost";
		menuServerLaunch();
	}

	if(m_connect->optionSecure())
	{
		KMessageBox::information(this, i18n("Please note that secure connections are not implemented yet!"), i18n("Connection security"));
	}

	KGGZDEBUG("Connect with: host=%s port=%i username=%s password=%s mode=%i\n", host, port, username, password, mode);

	if(m_dns) delete m_dns;
	m_dns = new QDns(host);
	connect(m_dns, SIGNAL(resultsReady()), SLOT(slotConnectedStart()));
	// FIXME: and now?

	m_save_loginmode = mode;
	m_save_username = strdup(username);
	m_save_password = strdup(password);
	m_save_host = strdup(host);
	m_save_port = port;
	KGGZDEBUG("Values are: username=%s password=%s mode=%i\n", m_save_username, m_save_password, m_save_loginmode);
}

void KGGZ::slotConnectedStart()
{
	int result;
	QValueList<QHostAddress> list;
	QHostAddress addr;

	KGGZDEBUGF("KGGZ::slotConnectedStart()\n");
	if(m_dns->addresses().at(0) == m_dns->addresses().end()) return;
	list = m_dns->addresses();
	KGGZDEBUG("Pre-Resolve: %s\n", (*list.at(0)).toString().latin1());
	addr = (*list.at(0));
	if(m_save_host) free(m_save_host);
	m_save_host = strdup(addr.toString().latin1());
	KGGZDEBUG("Host resolved to: %s\n", m_save_host);

	kggzserver = new GGZCoreServer();
	attachServerCallbacks();
	//ggzcore_server_log_session(kggzserver->server(), KGGZCommon::append(getenv("HOME"), "/.ggz/kggz.xml-log"));
	kggzserver->logSession(KGGZCommon::append(getenv("HOME"), "/.ggz/kggz.xml-log"));
	KGGZCommon::clear();

	kggzserver->setHost(m_save_host, m_save_port);
	KGGZDEBUG("connect now!\n");
	result = kggzserver->connect();
	if(m_killserver)
	{
		KGGZDEBUG("Delayed server kill now!\n");
		detachServerCallbacks();
		delete kggzserver;
		kggzserver = NULL;
		m_killserver = 0;
	}
	if(result == -1)
	{
		if(kggzserver)
		{
			detachServerCallbacks();
			delete kggzserver;
			kggzserver = NULL;
			KMessageBox::error(this, i18n("Attempt to connect refused!"), "Error!");
			menuConnect();
		}
		return;
	}
}

void KGGZ::menuDisconnect()
{
	KGGZDEBUGF("KGGZ::menuDisconnect()\n");

	eventLeaveGame();
    eventLeaveRoom();

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

	kggzserver->logout();
}

void KGGZ::menuServerLaunch()
{
	int result;
	GGZCoreConfio *config;
	char *process;

	config = new GGZCoreConfio(KGGZCommon::append(getenv("HOME"), "/.ggz/kggz.rc"), GGZCoreConfio::readwrite | GGZCoreConfio::create);
	KGGZCommon::clear();
	process = config->read("Environment", "Server", "/usr/bin/ggzd");
	delete config;

	if((!process) && (!strlen(process)))
	{
		KMessageBox::information(this, i18n("GGZ Gaming Zone server not found.\n"
			"Please configure it in the settings dialog.\n"), i18n("Note"));
		return;
	}

	result = KGGZCommon::launchProcess("ggzd", process);

	switch(result)
	{
		case -1:
			KMessageBox::error(this, i18n("Could not start the server!"), "Error!");
			break;
		case -2:
			KMessageBox::error(this, i18n("The GGZ server is already running!"), "Error!");
			break;
		default:
			emit signalMenu(MENUSIG_SERVERSTART);
			break;
	}
}

void KGGZ::menuServerKill()
{
	int result;

	result = KGGZCommon::killProcess("ggzd");
	switch(result)
	{
		case -3:
			KMessageBox::error(this, i18n("The server isn't running anymore."), i18n("Error!"));
			emit signalMenu(MENUSIG_SERVERSTOP);
			break;
		case -2:
			KMessageBox::error(this, i18n("You don't have the privileges to stop the server."), i18n("Error!"));
			break;
		case -1:
			KMessageBox::error(this, i18n("An unknown error occured."), i18n("Error!"));
			break;
		case 0:
			KMessageBox::error(this, i18n("Could only kill server via sigkill!"), i18n("Error!"));
			emit signalMenu(MENUSIG_SERVERSTOP);
			break;
		default:
			emit signalMenu(MENUSIG_SERVERSTOP);
			break;
	}
}

void KGGZ::menuConnect()
{
KGGZDEBUG("releasecritical: menuconnect is: %i\n", m_connect);
	if(!m_connect)
	{
		m_connect = new KGGZConnect(NULL, "connect");
		connect(m_connect, SIGNAL(signalConnect(const char*, int, const char*, const char*, int)),
			SLOT(slotConnected(const char*, int, const char*, const char*, int)));
	}
	m_connect->show();
}

void KGGZ::dispatch_free(char *var, const char *description)
{
	if(var)
	{
		KGGZDEBUG("- %s\n", description);
		free(var);
		var = NULL;
	}
}

void KGGZ::dispatch_delete(void *object, const char *description)
{
	if(object)
	{
		KGGZDEBUG("- %s\n", description);
		delete object;
		object = NULL;
	}
}

// fake: need base class to do destruction
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
	dispatch_delete((void*)m_table, "GGZCore++ table object");
	dispatch_delete((void*)m_module, "GGZCore++ module object");
	dispatch_delete((void*)m_config, "GGZCore++ configuration object");
	dispatch_delete((void*)m_core, "GGZCore++ core object");
	dispatch_free(m_save_username, "stored user name");
	dispatch_free(m_save_password, "stored password");
	dispatch_free(m_save_host, "stored host name");
	KGGZDEBUGF("KGGZ::Dispatcher() ready\n");
}

void KGGZ::timerEvent(QTimerEvent *e)
{
	if(m_killserver) return;
	if(m_lock) return;
	if(!kggzserver) return;

	if(kggzgame)
	{
		if(kggzgame->dataPending()) kggzgame->dataRead();
	}

	if((!m_lock) && (kggzserver))
	{
		if(kggzserver->dataPending()) kggzserver->dataRead();
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
	KGGZDEBUG("## Found %i tables\n", number);
	for(int i = 0; i < number; i++)
	{
		m_workspace->widgetUsers()->addTable(i);
		table = kggzroom->table(i);
		tabledescription = table->description();
		tableseats = table->countSeats();
		tableseatsopen = 0;
		for(int j = 0; j < tableseats; j++)
		{
			KGGZDEBUG(" ** table %i, seat %i: player type is %i\n", i, j, table->playerType(j));
			KGGZDEBUG(" ** table %i: state is %i\n", i, table->state());

			switch(table->playerType(j))
			{
				case GGZ_SEAT_OPEN:
					KGGZDEBUG("GGZ_SEAT_OPEN (%i)\n", GGZ_SEAT_OPEN);
					break;
				case GGZ_SEAT_BOT:
					KGGZDEBUG("GGZ_SEAT_BOT (%i)\n", GGZ_SEAT_BOT);
					break;
				case GGZ_SEAT_NONE:
					KGGZDEBUG("GGZ_SEAT_NONE (%i)\n", GGZ_SEAT_NONE);
					break;
				case GGZ_SEAT_RESERVED:
					KGGZDEBUG("GGZ_SEAT_RESERVED (%i)\n", GGZ_SEAT_RESERVED);
					break;
				case GGZ_SEAT_PLAYER:
					KGGZDEBUG("GGZ_SEAT_PLAYER (%i)\n", GGZ_SEAT_PLAYER);
					break;
				default:
					KGGZDEBUG("UNKNOWN (%i)\n", table->playerType(i));
			}

			if(table->playerType(j) == GGZ_SEAT_OPEN) tableseatsopen++;
			else
			{
				if(table->playerType(j) == GGZ_SEAT_PLAYER)
				{
					playername = table->playerName(j);
					if(strlen(playername) == 0)
					{
						playername = (char*)"(unknown)";
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
	if(!kggzroom) return;
	m_workspace->widgetUsers()->removeall();
	m_workspace->widgetChat()->chatline()->removeAll();
	if(m_grubby) m_grubby->removeAll();
	KGGZDEBUG("* Removed all, now adding anew...\n");
	number = kggzroom->countPlayers();
	for(int i = 0; i < number; i++)
	{
		player = kggzroom->player(i);
		playername = ggzcore_player_get_name(player);
		KGGZDEBUG(" # player: %s\n", playername);
		m_workspace->widgetUsers()->add(playername);
		m_workspace->widgetChat()->chatline()->addPlayer(playername);
		if(m_grubby) m_grubby->addPlayer(playername);
	}
	m_workspace->widgetUsers()->assignSelf(m_save_username);
}

void KGGZ::gameCollector(unsigned int id, void* data)
{
	switch(id)
	{
		case GGZCoreGame::launched:
			KGGZDEBUG("launched\n");
			m_workspace->widgetChat()->receive(NULL, i18n("Launched game"), KGGZChat::RECEIVE_ADMIN);
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
			eventLeaveGame();
			break;
		case GGZCoreGame::ioerror:
			KGGZDEBUG("ioerror\n");
			m_workspace->widgetChat()->receive(NULL, i18n("ERROR: Network error!"), KGGZChat::RECEIVE_ADMIN);
			break;
		case GGZCoreGame::protoerror:
			KGGZDEBUG("protoerror\n");
			m_workspace->widgetChat()->receive(NULL, i18n("ERROR: Protocol error!"), KGGZChat::RECEIVE_ADMIN);
			break;
		default:
			KGGZDEBUG("unknown!\n");
			break;
	}
}

void KGGZ::roomCollector(unsigned int id, void* data)
{
	char *chatsender = NULL, *chatmessage = NULL;
	//char buffer[1024];
	QString buffer;

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
			if(strncmp(chatmessage, "/me ", 4) == 0)
			{
				m_workspace->widgetChat()->receive(chatsender, chatmessage, KGGZChat::RECEIVE_ME);
			}
			else
			{
				if(strcmp(chatsender, m_save_username) == 0)
					m_workspace->widgetChat()->receive(chatsender, chatmessage, KGGZChat::RECEIVE_OWN);
				else
				{
					KGGZDEBUG("%s != %s\n", chatsender, m_save_username);
					m_workspace->widgetChat()->receive(chatsender, chatmessage, KGGZChat::RECEIVE_CHAT);
				}
			}
			break;
		case GGZCoreRoom::chatannounce:
			KGGZDEBUG("chatannounce\n");
			m_workspace->widgetChat()->receive(chatsender, chatmessage, KGGZChat::RECEIVE_ANNOUNCE);
			break;
		case GGZCoreRoom::chatprivate:
			KGGZDEBUG("chatprivate\n");
			m_workspace->widgetChat()->receive(chatsender, chatmessage, KGGZChat::RECEIVE_PERSONAL);
			break;
		case GGZCoreRoom::chatbeep:
			KGGZDEBUG("chatbeep\n");
			m_workspace->widgetChat()->beep();
			m_workspace->widgetChat()->receive(NULL, i18n("You have been beeped by %1.").arg(chatsender), KGGZChat::RECEIVE_PERSONAL);
			break;
		case GGZCoreRoom::enter:
			KGGZDEBUG("enter\n");
			buffer.append((char*)data);
			buffer.append(i18n(" enters the room."));
			m_workspace->widgetChat()->receive(NULL, buffer.latin1(), KGGZChat::RECEIVE_ADMIN);
			m_workspace->widgetUsers()->add((char*)data);
			m_workspace->widgetChat()->chatline()->addPlayer((char*)data);
			if(m_grubby) m_grubby->addPlayer((char*)data);
			break;
		case GGZCoreRoom::leave:
			KGGZDEBUG("leave\n");
			buffer.append((char*)data);
			buffer.append(i18n(" has left the room."));
			//strcpy(buffer, (char*)data);
			//strcat(buffer, i18n(" has left the room."));
			m_workspace->widgetChat()->receive(NULL, buffer.latin1(), KGGZChat::RECEIVE_ADMIN);
			m_workspace->widgetUsers()->remove((char*)data);
			KGGZDEBUG("remove from chatline\n");
			m_workspace->widgetChat()->chatline()->removePlayer((char*)data);
			KGGZDEBUG("Leave room event: ready\n");
			break;
		case GGZCoreRoom::tableupdate:
			KGGZDEBUG("tableupdate\n");
			//m_workspace->widgetUsers()->removeall();
			listPlayers();
			listTables();
			break;
		case GGZCoreRoom::tablelaunched:
			KGGZDEBUG("tablelaunched\n");
			//m_workspace->widgetUsers()->removeall();
			m_workspace->widgetChat()->receive(NULL, i18n("Launched table"), KGGZChat::RECEIVE_ADMIN);
			listPlayers();
			listTables();
			break;
		case GGZCoreRoom::tablelaunchfail:
			KGGZDEBUG("tablelaunchfail\n");
			KMessageBox::information(this, i18n("ERROR: Couldn't launch table!"), "Error!");
			listPlayers();
			listTables();
			break;
		case GGZCoreRoom::tablejoined:
			KGGZDEBUG("tablejoined\n");
			//m_workspace->widgetUsers()->removeall();
			m_workspace->widgetChat()->receive(NULL, i18n("Joined table"), KGGZChat::RECEIVE_ADMIN);
			//listPlayers();
			//listTables();
			//slotLaunchGame(kggzroom->gametype());
			slotGameFire();
			break;
		case GGZCoreRoom::tablejoinfail:
			KGGZDEBUG("tablejoinfail\n");
			KMessageBox::information(this, i18n("ERROR: Couldn't join table!"), "Error");
			listPlayers();
			listTables();
			break;
		case GGZCoreRoom::tableleft:
			KGGZDEBUG("tableleft\n");
			//m_workspace->widgetUsers()->removeall();
			KGGZDEBUG("SHALL I QUIT THE GAME HERE???\n");
			eventLeaveGame();
			m_workspace->widgetChat()->receive(NULL, i18n("Left table"), KGGZChat::RECEIVE_ADMIN);
			emit signalMenu(MENUSIG_GAMEOVER);
			break;
		case GGZCoreRoom::tableleavefail:
			KGGZDEBUG("tableleavefail\n");
			m_workspace->widgetChat()->receive(NULL, i18n("ERROR: Couldn't leave table!"), KGGZChat::RECEIVE_ADMIN);
			break;
		case GGZCoreRoom::tabledata:
			KGGZDEBUG("tabledata\n");
			if(!kggzgame)
			{
				KGGZDEBUG("Don't wanna bother, but... we don't have any game running!!\n");
				//return;
				//kggzroom->sendData((char*)data);
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
	//char buffer[1024];
	QString buffer;

	switch(id)
	{
		case GGZCoreServer::connected:
			KGGZDEBUG("connected\n");
			// possibly ggzcore bug:
			// state has not been updated yet here
			while((kggzserver) && (!kggzserver->isOnline())) kggzserver->dataRead();
			if(!kggzserver) return;
			kggzserver->setLogin(m_save_loginmode, m_save_username, m_save_password);
			result = kggzserver->login();
			if(result == -1)
			{
				detachServerCallbacks();
				delete kggzserver;
				kggzserver = NULL;
				KMessageBox::error(this, i18n("Attempt to login refused!"), "Error!");
				menuConnect();
				return;
			}
			break;
		case GGZCoreServer::connectfail:
			KGGZDEBUG("connectfail\n");
			detachServerCallbacks();
			delete kggzserver;
			kggzserver = NULL;
			KMessageBox::error(this, i18n("Couldn't connect to server!"), "Error!");
			menuConnect();
			break;
		case GGZCoreServer::negotiated:
			KGGZDEBUG("negotiated\n");
			break;
		case GGZCoreServer::negotiatefail:
			KGGZDEBUG("negotiatefail\n");
			m_killserver = 1;
			KMessageBox::error(this, i18n("Could not connect, maybe the server version is incompatible."), "Error!");
			menuConnect();
			// quick hack!
			KGGZDEBUG("Free ggzcore server object\n");
			kggzserver->rescue();
			KGGZDEBUG("freed\n");
			break;
		case GGZCoreServer::loggedin:
			KGGZDEBUG("loggedin\n");
			emit signalMenu(MENUSIG_LOGIN);
			buffer.sprintf(i18n("KGGZ - [logged in as %s@%s:%i]"), m_save_username, m_save_host, m_save_port);
			emit signalCaption(buffer);
			menuView(VIEW_CHAT);
			if(m_save_loginmode == GGZCoreServer::firsttime)
			{
				KGGZDEBUG("First time login!\n");
				buffer.sprintf(i18n("You are welcome as a new GGZ Gaming Zone player.\n"
					"Your personal password is: %s"), kggzserver->password());
				KMessageBox::information(this, buffer, "Information");
				//if(m_motd) m_motd->show();
			}
			buffer.sprintf(i18n("Logged in as %s"), m_save_username);
			m_workspace->widgetChat()->receive(NULL, buffer, KGGZChat::RECEIVE_ADMIN);
			m_workspace->widgetChat()->receive(NULL, i18n("Please join a room to start!"), KGGZChat::RECEIVE_ADMIN);
			if((m_save_loginmode == GGZCoreServer::firsttime) && (m_motd)) m_motd->raise();
			kggzserver->listRooms(-1, 1);
			if(kggzserver->listGames(1) != 0) // NEVER use 0 here, it will hang the client !!!
			{
				KGGZDEBUG("HUH? Don't give me game type list?!\n");
				return;
			}
			break;
		case GGZCoreServer::loginfail:
			KGGZDEBUG("loginfail\n");
			KMessageBox::error(this, i18n("Login refused!"), "Error!");
			// TODO: do something here !? 2001-07-17 josef
			menuConnect();
			break;
		case GGZCoreServer::motdloaded:
			KGGZDEBUG("motdloaded\n");
			if(!m_motd) m_motd = new KGGZMotd(NULL, "KGGZMotd");
			m_motd->setSource(data);
			m_motd->show();
			break;
		case GGZCoreServer::roomlist:
			KGGZDEBUG("roomlist\n");
			emit signalMenu(MENUSIG_ROOMLIST);
			for(int i = 0; i < kggzserver->countRooms(); i++)
			{
				KGGZDEBUG("Found: %s (%s :: %s)\n", kggzserver->room(i)->name(), kggzserver->room(i)->category(), kggzserver->room(i)->description());
				emit signalRoom(kggzserver->room(i)->name(), kggzserver->room(i)->category());
			}
			break;
		case GGZCoreServer::typelist:
			KGGZDEBUG("typelist\n");
			break;
		case GGZCoreServer::entered:
			KGGZDEBUG("entered\n");
			KGGZDEBUG("==> creating room object\n");
			m_lock = 1;
			eventLeaveRoom();
			kggzroom = kggzserver->room();
			//kggzroom = new GGZCoreRoom(ggzcore_server_get_cur_room(kggzserver->server()));
			attachRoomCallbacks();
			m_lock = 0;
			kggzroom->listPlayers();
			kggzroom->listTables(-1, 0);
			slotLoadLogo();
			buffer.sprintf(i18n("Entered room %s"), kggzroom->name());
			m_workspace->widgetChat()->init();
			m_workspace->widgetChat()->receive(NULL, buffer, KGGZChat::RECEIVE_ADMIN);
			emit signalLocation(i18n("  Room: ") + kggzroom->name() + "  ");
			emit signalMenu(MENUSIG_ROOMENTER);
			break;
		case GGZCoreServer::enterfail:
			KGGZDEBUG("enterfail\n");
			KMessageBox::information(this, i18n("Sorry, you cannot change the room while playing!"), i18n("Room join error"));
			break;
		case GGZCoreServer::loggedout:
			KGGZDEBUG("loggedout\n");
			m_workspace->widgetChat()->receive(NULL, i18n("Logged out"), KGGZChat::RECEIVE_ADMIN);
			m_lock = 1;
			detachServerCallbacks();
			delete kggzserver;
			kggzserver = NULL;
			m_lock = 0;
			KGGZDEBUG("Disconnection successful.\n");
			emit signalMenu(MENUSIG_DISCONNECT);
			break;
		case GGZCoreServer::neterror:
			KGGZDEBUG("neterror\n");
			m_workspace->widgetChat()->receive(NULL, i18n("ERROR: Network error detected!"), KGGZChat::RECEIVE_ADMIN);
			//emit signalMenu(MENUSIG_DISCONNECT);
//?
			menuDisconnect();
			// This is a quickhack, but for the user's sake
			serverCollector(GGZCoreServer::loggedout, NULL);
			break;
		case GGZCoreServer::protoerror:
			KGGZDEBUG("protoerror\n");
			m_workspace->widgetChat()->receive(NULL, i18n("ERROR: Protocol error detected!"), KGGZChat::RECEIVE_ADMIN);
			detachServerCallbacks();
			delete kggzserver;
			kggzserver = NULL;
			KMessageBox::error(this, "A protocol error occured!", "Error!");
			menuConnect();
			break;
		case GGZCoreServer::chatfail:
			KGGZDEBUG("chatfail\n");
			m_workspace->widgetChat()->receive(NULL, i18n(QString("ERROR: Chat error detected: ").append((char*)data)), KGGZChat::RECEIVE_ADMIN);
			break;
		case GGZCoreServer::statechange:
			KGGZDEBUG("statechange\n");
			KGGZDEBUG("State is now: %s\n", KGGZCommon::state(kggzserver->state()));
			// Spades-type Workaround
			//if(kggzserver->state() == GGZ_STATE_IN_ROOM)
			//{
				//KGGZDEBUG(" -- Do we have a game? %i\n", kggzgame);
				//KGGZDEBUG(" -- Do we have a table? %i\n", kggzserver->isAtTable());
				//if(kggzroom) kggzroom->leaveTable();
			//}
			//if(kggzserver->state() == GGZ_STATE_IN_ROOM)
			//{
				//m_workspace->widgetLogo()->setBackgroundPixmap(QPixmap(NULL));
			//}
			emit signalState(kggzserver->state());
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
	KGGZDEBUG("Return: GGZ_HOOK_OK\n");
	return GGZ_HOOK_OK;
}

void KGGZ::slotChat(const char *text, char *player, int mode)
{
	char *sendtext;

	if((kggzserver) && (kggzroom))
	{
		KGGZDEBUG("Chat sends: %s\n", text);
		sendtext = strdup(text);
		switch(mode)
		{
			case KGGZChat::RECEIVE_CHAT:
				kggzroom->chat(GGZ_CHAT_NORMAL, player, sendtext);
				break;
			case KGGZChat::RECEIVE_ANNOUNCE:
				if(kggzroom->chat(GGZ_CHAT_ANNOUNCE, NULL, sendtext) < 0)
				{
					m_workspace->widgetChat()->receive(NULL, "Only administrators are allowed to broadcast messages.", KGGZChat::RECEIVE_ADMIN);
				}
				break;
			case KGGZChat::RECEIVE_BEEP:
				kggzroom->chat(GGZ_CHAT_BEEP, player, sendtext);
				break;
			case KGGZChat::RECEIVE_PERSONAL:
				kggzroom->chat(GGZ_CHAT_PERSONAL, player, sendtext);
				break;
		}
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

void KGGZ::slotGameFire()
{
	int ret;

	KGGZDEBUG("KGGZ::slotGameFire() with frontend %i\n", m_gameinfo->frontend());
	if(!m_module)
	{
		KGGZDEBUG("Critical: Eeeeak! No module found.\n");
		return;
	}

	m_module->setActive(m_gameinfo->frontend());
	//m_module->launch();

	if(kggzgame)
	{
		KGGZDEBUG("Found game object alive; killing it now :-)\n");
		delete kggzgame;
	}
	kggzgame = new GGZCoreGame();
	kggzgame->init(m_module->module());

	attachGameCallbacks(); // don't forget detaching!

	delete m_module;
	m_module = NULL;

	ret = kggzgame->launch();
	if(ret < 0)
	{
		KGGZDEBUG("Red Alert! Game launching failed immedeately!\n");
		eventLeaveGame();
		KMessageBox::information(this, i18n("Couldn't launch game!"), "Error!");
		KGGZDEBUG("Phew.. the crisis is over, let's continue with normal work.\n");
		return;
	}

	slotLoadLogo();

	// not already at slotLaunchGame!
	emit signalMenu(MENUSIG_GAMESTART);

	//delete module;
}

void KGGZ::menuView(int viewtype)
{
	m_workspace->show();
	m_splash->hide();
#ifdef KGGZ_BROWSER
	m_browser->hide();
#endif
	switch(viewtype)
	{
		case VIEW_CHAT:
			m_workspace->hideBar(0);
			m_workspace->widgetChat()->show();
			m_workspace->widgetLogo()->show();
			m_workspace->widgetTables()->show();
			m_workspace->widgetUsers()->show();
			break;
		case VIEW_USERS:
			//m_workspace->hideBar(1);
			m_workspace->widgetChat()->hide();
			m_workspace->widgetUsers()->show();
			m_workspace->widgetLogo()->hide();
			m_workspace->widgetTables()->hide();
			//m_workspace->widgetUsers()->resize(width(), height());
			//m_workspace->widgetLogo()->setFixedHeight(1);
			//m_workspace->widgetTables()->setFixedHeight(1);
			//m_workspace->widgetUsers()->resize(width(), height());
			//m_workspace->updateGeometry();
			m_workspace->hideBar(1);
			break;
		case VIEW_TABLES:
			m_workspace->hideBar(0);
			m_workspace->widgetChat()->hide();
			m_workspace->widgetLogo()->hide();
			m_workspace->widgetTables()->show();
			m_workspace->widgetUsers()->hide();
			break;
		case VIEW_SPLASH:
			m_splash->show();
			m_workspace->hide();
			break;
		case VIEW_BROWSER:
#ifdef KGGZ_BROWSER
			m_browser->show();
#endif
			m_workspace->hide();
			break;
		default:
			KGGZDEBUG("viewtype: unknown (%i)!\n", viewtype);
	}
	// Qt is crappy here... so we must do something for the chicks (animation)
	KGGZDEBUG("resize from menuView()...\n");
	m_workspace->resize(width() - 1, height() - 1);
	m_workspace->resize(width(), height());

	// Don't hide connection dialog
	if(m_connect) m_connect->raise();
}

void KGGZ::menuGameLaunch()
{
	GGZCoreGametype *gametype;

	if(!kggzroom)
	{
		KMessageBox::information(this, i18n("You cannot launch a game outside a room!"), "Error!");
		return;
	}
	gametype = kggzroom->gametype();

	if(m_gameinfo) delete m_gameinfo;
	m_gameinfo = new KGGZGameInfo();
	m_gameinfo->setType(KGGZGameInfo::typelaunch);

	slotGameFrontend();
}

void KGGZ::menuGameJoin()
{
	int number;

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
			KMessageBox::information(this, i18n("Please select a table to join!"), "Error!");
			return;
		}
	}

	if(m_gameinfo) delete m_gameinfo;
	m_gameinfo = new KGGZGameInfo();
	m_gameinfo->setType(KGGZGameInfo::typejoin);
	m_gameinfo->setTable(number);

	slotGameFrontend();
	//kggzroom->joinTable(number);
}

void KGGZ::slotGameFrontend()
{
	GGZCoreGametype *gametype;
	int modules;

	if((!kggzroom) || (!m_gameinfo))
	{
		KGGZDEBUG("Critical: Frontend selection without prior assignments\n");
		return;
	}

	gametype = kggzroom->gametype();

	KGGZDEBUG("Create module...\n");
	m_module = new GGZCoreModule();
	m_module->init(gametype->name(), gametype->protocolVersion(), gametype->protocolEngine());
	modules = m_module->count();

	KGGZDEBUG("Found: %i modules for this game\n", modules);
	if(modules == 0)
	{
		KMessageBox::information(this, i18n("Sorry, no modules found for this game."), "Error!");
		delete m_module;
		m_module = NULL;
		//eventLeaveGame();
		return;
	}

	//emit signalMenu(MENUSIG_GAMESTART);

	if(modules == 1) slotGamePrepare(0);
	else
	{
		if(m_selector) delete m_selector;
		m_selector = new KGGZSelector(NULL, "KGGZSelector");
		for(int i = 0; i < modules; i++)
		{
			m_module->setActive(i);
			m_selector->addFrontend(m_module->frontend());
		}
		m_selector->show();
		connect(m_selector, SIGNAL(signalFrontend(int)), SLOT(slotGamePrepare(int)));
	}
}

void KGGZ::slotGamePrepare(int frontend)
{
	GGZCoreGametype *gametype;

	if(!kggzroom) return;
	gametype = kggzroom->gametype();

	KGGZDEBUG("Receive Frontend: %i *******************************************************\n", frontend);
	m_gameinfo->setFrontend(frontend);
	if(m_gameinfo->type() == KGGZGameInfo::typejoin) slotGameStart();
	else
	{
		if(m_launch)
		{
			delete m_launch;
			m_launch = NULL;
		}
		if(!m_launch) m_launch = new KGGZLaunch(NULL, "KGGZLaunch");
		m_launch->initLauncher(m_save_username, gametype->maxPlayers(), gametype->maxBots());
		for(int i = 0; i < gametype->maxPlayers(); i++)
		{
			KGGZDEBUG("Assignment: %i is %i\n", i, gametype->isPlayersValid(i + 1));
			m_launch->setSeatAssignment(i, gametype->isPlayersValid(i + 1));
		}
		m_launch->show();
		connect(m_launch, SIGNAL(signalLaunch()), SLOT(slotGameStart()));
	}
}

void KGGZ::slotGameStart()
{
	GGZCoreGametype *gametype;
	int seats;
	char *description;
	int ret;

	if(!kggzroom)
	{
		KGGZDEBUG("Baby I shrunk the rooms list!\n");
		return;
	}
	gametype = kggzroom->gametype();
	if((!gametype) || (!gametype->gametype()) || (!m_gameinfo))
	{
		KGGZDEBUG("Erm... would you mind to give me the game type?\n");
		return;
	}

	if(m_gameinfo->type() == KGGZGameInfo::typelaunch)
	{
		if(!m_launch)
		{
			KGGZDEBUG("Huh?\n");
			return;
		}
		description = (char*)m_launch->description();
		seats = m_launch->seats();

		m_table = new GGZCoreTable();
		m_table->init(gametype->gametype(), description, seats);
		for(int i = 0; i < seats; i++)
		{
			switch(m_launch->seatType(i))
			{
				case KGGZLaunch::seatplayer:
					KGGZDEBUG("* %i: player\n", i);
					/*table->addPlayer(m_save_username, i);*/ // don't :-)
					break;
				case KGGZLaunch::seatopen:
					KGGZDEBUG("* %i: open\n", i);
					break;
				case KGGZLaunch::seatbot:
					KGGZDEBUG("* %i: bot\n", i);
					m_table->addBot(NULL, i);
					break;
				case KGGZLaunch::seatreserved:
					KGGZDEBUG("* %i: reserved\n", i);
					m_table->addReserved((char*)"RESERVED", i);
					break;
				case KGGZLaunch::seatunused:
					KGGZDEBUG("* %i: unused.\n", i);
					break;
				case KGGZLaunch::seatunknown:
					default:
					KGGZDEBUG("* %i: unknown!\n", i);
			}
		}
		ret = kggzroom->launchTable(m_table->table());
		delete m_launch;
		m_launch = NULL;
		if(ret < 0)
		{
			delete m_module;
			m_module = NULL;
			delete m_table;
			m_table = NULL;
			KMessageBox::error(this, i18n("Failed launching the requested table!"), "Error!");
			return;
		}
	}
	else
	{
		ret = kggzroom->joinTable(m_gameinfo->table());
		if(ret < 0)
		{
			delete m_module;
			m_module = NULL;
			KMessageBox::error(this, i18n("Failed joining the requested table!"), "Error!");
			return;
		}
	}

	if(m_table)
	{
		delete m_table;
		m_table = NULL;
	}

	//slotGameFire();
}

void KGGZ::menuGameInfo()
{
	GGZCoreGametype *gametype;
	//char buffer[2048];
	QString buffer;

	if(!kggzserver)
	{
		KGGZDEBUG("Critical! No server found.\n");
		return;
	}
	if(!kggzroom)
	{
		//KGGZDEBUG("Critical! No room found.\n");
		KMessageBox::information(this, i18n("Please join a room first."), "Info:");
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
	KGGZDEBUG("ProtocolVersion: %s\n", gametype->protocolVersion());
	KGGZDEBUG("ProtocolEngine: %s\n", gametype->protocolEngine());
	KGGZDEBUG("Category: %s\n", gametype->category());
	KGGZDEBUG("Rating: %s\n", gametype->rating());
	buffer.append(i18n("Name: "));
	buffer.append(gametype->name());
	buffer.append("\n");
	buffer.append(i18n("Description: "));
	buffer.append(gametype->description());
	buffer.append("\n");
	buffer.append(i18n("Author: "));
	buffer.append(gametype->author());
	buffer.append("\n");
	buffer.append(i18n("Version: "));
	buffer.append(gametype->version());
	buffer.append("\n");
	buffer.append(i18n("Protocol: "));
	buffer.append(gametype->protocolVersion());
	buffer.append("\n");
	buffer.append(i18n("Engine: "));
	buffer.append(gametype->protocolEngine());
	buffer.append("\n");
	buffer.append(i18n("URL: "));
	buffer.append(gametype->url());
	buffer.append("\n");
	buffer.append(i18n("Game Category: "));
	buffer.append(gametype->category());
	buffer.append("\n");
	buffer.append(i18n("Rating: "));
	buffer.append(gametype->rating());
	//sprintf(buffer, "Name: %s\nDescription: %s\nAuthor: %s\nVersion: %s\nProtocol: %s\nURL: %s",
	//	gametype->name(), gametype->description(), gametype->author(), gametype->version(), gametype->protocol(), gametype->url());
	KMessageBox::information(this, buffer, i18n("Game Type Information"));
}

void KGGZ::menuRoom(int room)
{
	if(room >= 0)
	{
		if(kggzserver)
		{
			kggzserver->joinRoom(room);
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

void KGGZ::slotLoadLogo()
{
	GGZCoreModule *module;
	GGZCoreGametype *gametype;
	char *icon;

	KGGZDEBUG("__ loading logo __\n");
	if((!kggzroom) || (!(gametype = kggzroom->gametype())))
	{
		KGGZDEBUG("Critical! slotLoadLogo is broken!\n");
		return;
	}

	module = new GGZCoreModule();
	module->init(gametype->name(), gametype->protocolVersion(), gametype->protocolEngine());
	if(module->count() == 0)
	{
		KGGZDEBUG("Critical! No modules found!\n");
		return;
	}
	module->setActive(0);

	icon = module->pathIcon();
	m_workspace->widgetLogo()->setLogo(icon, gametype->name());

	delete module;
}

void KGGZ::menuGrubby()
{
	if(!kggzroom)
	{
		KMessageBox::information(this, i18n("Please join a room first."), "Error!");
		return;
	}
	if(m_grubby) delete m_grubby;
	m_grubby = new KGGZGrubby(NULL, "KGGZGrubby");
	m_grubby->show();
	listPlayers();
	listTables();
	connect(m_grubby, SIGNAL(signalAction(const char*, const char*, int)), SLOT(slotGrubby(const char*, const char*, int)));
}

void KGGZ::slotGrubby(const char *grubby, const char *argument, int id)
{
	switch(id)
	{
		case KGGZGrubby::actionabout:
			slotChat(QString("%1 about").arg(grubby).latin1(), NULL, KGGZChat::RECEIVE_CHAT);
			break;
		case KGGZGrubby::actionhelp:
			slotChat(QString("%1 help").arg(grubby).latin1(), NULL, KGGZChat::RECEIVE_CHAT);
			break;
		case KGGZGrubby::actionseen:
			slotChat(QString("%1 have you seen %2").arg(grubby).arg(argument).latin1(), NULL, KGGZChat::RECEIVE_CHAT);
			break;
		case KGGZGrubby::actionalertadd:
			slotChat(QString("%1 alert %2").arg(grubby).arg(argument).latin1(), NULL, KGGZChat::RECEIVE_CHAT);
			break;
		case KGGZGrubby::actionmessages:
			slotChat(QString("%1 do i have any messages").arg(grubby).latin1(), NULL, KGGZChat::RECEIVE_CHAT);
			break;
		case KGGZGrubby::actionbye:
			KMessageBox::information(m_grubby, i18n("The pleasure has been on my side :)"), "Grubby");
			break;
		case KGGZGrubby::actionteach:
			slotChat(QString("%1 %2").arg(grubby).arg(argument), NULL, KGGZChat::RECEIVE_CHAT);
			break;
		case KGGZGrubby::actionwhois:
			slotChat(QString("%1 who is %2").arg(grubby).arg(argument), NULL, KGGZChat::RECEIVE_CHAT);
			break;
		default:
			KMessageBox::information(m_grubby, i18n("I don't know that command, sorry."), "Error!");
	}
}

void KGGZ::menuPreferencesSettings()
{
	KGGZDEBUGF("showing preferences dialog\n");
	if(m_prefenv) delete m_prefenv;
	m_prefenv = new KGGZPrefEnv(NULL, "KGGZPrefEnv");
	m_prefenv->show();
}

void KGGZ::eventLeaveGame()
{
	if(!kggzgame)
	{
		KGGZDEBUG("Hrmpf, someone deleted kggzgame...\n");
	}
	else
	{
		detachGameCallbacks();
		// FIXME: Don't delete kggzgame here! It will terminate KGGZ!!!!!
		//delete kggzgame;
		kggzgame = NULL;
		m_workspace->widgetChat()->receive(NULL, i18n("Game over"), KGGZChat::RECEIVE_ADMIN);
	}
	KGGZDEBUG("Leave table?\n");
	if(kggzserver->state() == GGZ_STATE_AT_TABLE)
	{
		KGGZDEBUG("**** Still at table (alert) -> leaving now!\n");
		kggzroom->leaveTable();
	}
	listPlayers();
	listTables();
	//emit signalMenu(MENUSIG_GAMEOVER);
}

void KGGZ::eventLeaveRoom()
{
	if(!kggzroom)
	{
		KGGZDEBUG("Pffft... no room here.\n");
		return;
	}
	detachRoomCallbacks();
	delete kggzroom;
	kggzroom = NULL;
	m_workspace->widgetChat()->chatline()->removeAll();
	m_workspace->widgetUsers()->removeall();
	m_workspace->widgetTables()->reset();
	m_workspace->widgetChat()->shutdown();
	m_workspace->widgetLogo()->shutdown();
	emit signalLocation(i18n("  No room selected  "));
}

void KGGZ::menuGameCancel()
{
	KGGZDEBUGF("KGGZ::menuGameCancel()\n");
	eventLeaveGame();
}

