#include "ggzap_handler.h"

#include "GGZCoreModule.h"
#include "GGZCoreGametype.h"
#include "GGZCore.h"
#include "GGZCoreConfio.h"

#include <string.h>
#include <stdlib.h>

#include <iostream>

using namespace std;

GGZapHandler::GGZapHandler()
: QObject()
{
	GGZCore *core;

	m_server = NULL;
	m_room = NULL;
	m_game = NULL;
	m_module = NULL;
	m_frontendtype = NULL;
	m_modulename = NULL;
	m_confserver = NULL;
	m_confusername = NULL;
	m_zapuser = NULL;

	core = new GGZCore();
	core->init(GGZCore::parser | GGZCore::modules, "/tmp/ggzap.debug", GGZCore::all);
}

GGZapHandler::~GGZapHandler()
{
	shutdown();
}

void GGZapHandler::init()
{
	int result;
	GGZCoreConfio *conf;
	char confdir[1024];
	char *user;

	m_zapuser = (char*)malloc(17);
	strcpy(confdir, getenv("HOME"));
	strcat(confdir, "/.ggz/ggzap.rc");
	conf = new GGZCoreConfio(confdir, GGZCoreConfio::readonly);
	m_confserver = conf->read("Global", "Server", "141.30.227.122");
	user = conf->read("Global", "Username", "");
	delete conf;

	strncpy(m_zapuser, user, 12);
	strcat(m_zapuser, "/zap");
	m_confusername = m_zapuser;

	if(!m_modulename) return;

	m_server = new GGZCoreServer();
	attachServerCallbacks();

	m_server->setHost(m_confserver, 5688, 0);
	result = m_server->connect();
	//if(result == -1) emit signalState(connectfail);
}

void GGZapHandler::shutdown()
{
	if(m_zapuser)
	{
		free(m_zapuser);
		m_zapuser = NULL;
	}
	if(m_server)
	{
		m_server->logout();
		process();
	}
	if(m_game)
	{
		detachGameCallbacks();
		delete m_game;
		m_game = NULL;
	}
	if(m_room)
	{
		detachRoomCallbacks();
		delete m_room;
		m_room = NULL;
	}
	if(m_server)
	{
		detachServerCallbacks();
		delete m_server;
		m_server = NULL;
	}
	m_modulename = NULL;
	m_frontendtype = NULL;
}

void GGZapHandler::setModule(const char *modulename)
{
	m_modulename = modulename;
}

void GGZapHandler::setFrontend(const char *frontendtype)
{
	m_frontendtype = frontendtype;
}

void GGZapHandler::process()
{
	if(!m_server) return;
	if(m_server->dataPending()) m_server->dataRead();
	if(!m_game) return;
	if(m_game->dataPending()) m_game->dataRead();
//cout << "." << endl;
}

GGZHookReturn GGZapHandler::hookServer(unsigned int id, void *event_data, void *user_data)
{
	GGZapHandler *handler;

cout << "server " << id << endl;

	handler = (GGZapHandler*)user_data;
	handler->hookServerActive(id);
	return GGZ_HOOK_OK;
}

GGZHookReturn GGZapHandler::hookRoom(unsigned int id, void *event_data, void *user_data)
{
	GGZapHandler *handler;

cout << "room " << id << endl;

	handler = (GGZapHandler*)user_data;
	handler->hookRoomActive(id, event_data);
	return GGZ_HOOK_OK;
}

GGZHookReturn GGZapHandler::hookGame(unsigned int id, void *event_data, void *user_data)
{
	GGZapHandler *handler;

cout << "game " << id << endl;

	handler = (GGZapHandler*)user_data;
	handler->hookGameActive(id, event_data);
	return GGZ_HOOK_OK;
}

void GGZapHandler::hookServerActive(unsigned int id)
{
	int join;

	switch(id)
	{
		case GGZCoreServer::connected:
			emit signalState(connected);
			while(!m_server->isOnline()) m_server->dataRead();
			m_server->setLogin(GGZCoreServer::guest, m_confusername, "GGZap Game");
			m_server->login();
			break;
		case GGZCoreServer::connectfail:
			emit signalState(connectfail);
			break;
		case GGZCoreServer::loggedin:
			emit signalState(loggedin);
			m_server->listRooms(-1, 0);
			m_server->listGames(1);
			break;
		case GGZCoreServer::loginfail:
			emit signalState(loginfail);
			break;
		case GGZCoreServer::roomlist:
			break;
		case GGZCoreServer::typelist:
			join = -1;
			for(int i = 0; i < m_server->countRooms(); i++)
			{
				if(strcmp(m_server->room(i)->gametype()->name(), m_modulename) == 0) join = i;
				else cout << "type: " << m_server->room(i)->gametype()->name() << endl;
			}
			if(join == -1) emit signalState(joinroomfail);
			else m_server->joinRoom(join);
			break;
		case GGZCoreServer::entered:
			m_room = m_server->room();
			attachRoomCallbacks();
			emit signalState(joinedroom);
			m_room->listPlayers();
			m_room->listTables(-1, 0);
			break;
		case GGZCoreServer::enterfail:
			emit signalState(joinroomfail);
			break;
		default:
			cout << "not handled: " << id << endl;
	}
}

void GGZapHandler::getModule()
{
	int count;
	GGZCoreGametype *gametype;

	m_module = new GGZCoreModule();
	gametype = m_room->gametype();
cout << "##### init with: " << gametype->name() << ", " << gametype->protocolVersion() << endl;
	m_module->init(gametype->name(), gametype->protocolVersion(), gametype->protocolEngine());
	count = m_module->count();
	if(!count)
	{
cout << "##### no module defined!!!" << endl;
		delete m_module;
		m_module = NULL;
		emit signalState(startfail);
	}
	else
	{
		m_module->setActive(0);
		if(m_frontendtype)
		{
			for(int i = 0; i < count; i++)
			{
				m_module->setActive(i);
				if(!strcmp(m_frontendtype, m_module->frontend())) break;
			}
		}
cout << "##### frontend: " << m_module->frontend() << endl;
	}
}

void GGZapHandler::hookRoomActive(unsigned int id, void *data)
{
	GGZPlayer *player;
	GGZCoreTable *table;
	GGZCoreGametype *gametype;
	char *name;
	int join;
	int ret;

	switch(id)
	{
		case GGZCoreRoom::playerlist:
			for(int i = 0; i < m_room->countPlayers(); i++)
			{
				player = m_room->player(i);
				name = ggzcore_player_get_name(player);
			}
			break;
		case GGZCoreRoom::tablelist:
			emit signalState(waiting);
			join = -1;
			for(int i = 0; i < m_room->countTables(); i++)
			{
				table = m_room->table(i);
				for(int j = 0; j < table->countSeats(); j++)
					if(table->playerType(j) == GGZ_SEAT_OPEN) join = i;
			}
			if(join != -1)
			{
				getModule();
				if(m_module)
				{
cout << "##### joinTable: " << join << endl;
					m_room->joinTable(join);
				}
			}
			break;
		case GGZCoreRoom::tablejoined:
			m_game = new GGZCoreGame();
			m_game->init(m_module->module());
			attachGameCallbacks();
			delete m_module;
			m_module = NULL;
			if(m_game->launch() < 0)
			{
cout << "##### game launch fail" << endl;
				emit signalState(startfail);
			}
			else emit signalState(started);
			break;
		case GGZCoreRoom::enter:
			// launch table and wait...
			// NO: better wait for race condition?
			getModule();
			if(!m_module) return;
			gametype = m_room->gametype();
			table = new GGZCoreTable();
			table->init(gametype->gametype(), "foobar", 2);
			ret = m_room->launchTable(table->table());
			delete table;
			if(ret >= 0) emit signalState(waiting);
			else emit signalState(startfail);
			// recognize victims here :-) But... is this important?
			break;
		case GGZCoreRoom::tablejoinfail:
			delete m_module;
			m_module = NULL;
cout << "##### tablejoinfail" << endl;
			emit signalState(startfail);
			break;
		case GGZCoreRoom::tabledata:
			if(m_game) m_game->dataSend((char*)data);
			break;
		case GGZCoreRoom::tableupdate:
			hookRoomActive(GGZCoreRoom::tablelist, NULL);
			break;
		default:
			cout << "not handled: " << id << endl;
	}
}

void GGZapHandler::hookGameActive(unsigned int id, void *data)
{
	switch(id)
	{
		case GGZCoreGame::data:
			m_room->sendData((char*)data);
			break;
		case GGZCoreGame::over:
			detachGameCallbacks();
			delete m_game;
			m_game = NULL;
			emit signalState(finish);
			break;
	}
}

///////////////////////////////////////////////////////////////////
// stolen from GGZapHandler
// todo: only use hooks which are essential!
///////////////////////////////////////////////////////////////////

void GGZapHandler::attachServerCallbacks()
{
    m_server->addHook(GGZCoreServer::connected, &GGZapHandler::hookServer, (void*)this);
    m_server->addHook(GGZCoreServer::connectfail, &GGZapHandler::hookServer, (void*)this);
    m_server->addHook(GGZCoreServer::negotiated, &GGZapHandler::hookServer, (void*)this);
    m_server->addHook(GGZCoreServer::negotiatefail, &GGZapHandler::hookServer, (void*)this);
    m_server->addHook(GGZCoreServer::loggedin, &GGZapHandler::hookServer, (void*)this);
    m_server->addHook(GGZCoreServer::loginfail, &GGZapHandler::hookServer, (void*)this);
    m_server->addHook(GGZCoreServer::motdloaded, &GGZapHandler::hookServer, (void*)this);
    m_server->addHook(GGZCoreServer::roomlist, &GGZapHandler::hookServer, (void*)this);
    m_server->addHook(GGZCoreServer::typelist, &GGZapHandler::hookServer, (void*)this);
    m_server->addHook(GGZCoreServer::entered, &GGZapHandler::hookServer, (void*)this);
    m_server->addHook(GGZCoreServer::enterfail, &GGZapHandler::hookServer, (void*)this);
    m_server->addHook(GGZCoreServer::loggedout, &GGZapHandler::hookServer, (void*)this);
    m_server->addHook(GGZCoreServer::neterror, &GGZapHandler::hookServer, (void*)this);
    m_server->addHook(GGZCoreServer::protoerror, &GGZapHandler::hookServer, (void*)this);
    m_server->addHook(GGZCoreServer::chatfail, &GGZapHandler::hookServer, (void*)this);
    m_server->addHook(GGZCoreServer::statechange, &GGZapHandler::hookServer, (void*)this);
}

void GGZapHandler::detachServerCallbacks()
{
    m_server->removeHook(GGZCoreServer::connected, &GGZapHandler::hookServer);
    m_server->removeHook(GGZCoreServer::connectfail, &GGZapHandler::hookServer);
    m_server->removeHook(GGZCoreServer::negotiated, &GGZapHandler::hookServer);
    m_server->removeHook(GGZCoreServer::negotiatefail, &GGZapHandler::hookServer);
    m_server->removeHook(GGZCoreServer::loggedin, &GGZapHandler::hookServer);
    m_server->removeHook(GGZCoreServer::loginfail, &GGZapHandler::hookServer);
    m_server->removeHook(GGZCoreServer::motdloaded, &GGZapHandler::hookServer);
    m_server->removeHook(GGZCoreServer::roomlist, &GGZapHandler::hookServer);
    m_server->removeHook(GGZCoreServer::typelist, &GGZapHandler::hookServer);
    m_server->removeHook(GGZCoreServer::entered, &GGZapHandler::hookServer);
    m_server->removeHook(GGZCoreServer::enterfail, &GGZapHandler::hookServer);
    m_server->removeHook(GGZCoreServer::loggedout, &GGZapHandler::hookServer);
    m_server->removeHook(GGZCoreServer::neterror, &GGZapHandler::hookServer);
    m_server->removeHook(GGZCoreServer::protoerror, &GGZapHandler::hookServer);
    m_server->removeHook(GGZCoreServer::chatfail, &GGZapHandler::hookServer);
    m_server->removeHook(GGZCoreServer::statechange, &GGZapHandler::hookServer);
}

void GGZapHandler::attachRoomCallbacks()
{
	m_room->addHook(GGZCoreRoom::playerlist, &GGZapHandler::hookRoom, (void*)this);
	m_room->addHook(GGZCoreRoom::tablelist, &GGZapHandler::hookRoom, (void*)this);
	m_room->addHook(GGZCoreRoom::chatnormal, &GGZapHandler::hookRoom, (void*)this);
	m_room->addHook(GGZCoreRoom::chatannounce, &GGZapHandler::hookRoom, (void*)this);
	m_room->addHook(GGZCoreRoom::chatprivate, &GGZapHandler::hookRoom, (void*)this);
	m_room->addHook(GGZCoreRoom::chatbeep, &GGZapHandler::hookRoom, (void*)this);
	m_room->addHook(GGZCoreRoom::enter, &GGZapHandler::hookRoom, (void*)this);
	m_room->addHook(GGZCoreRoom::leave, &GGZapHandler::hookRoom, (void*)this);
	m_room->addHook(GGZCoreRoom::tableupdate, &GGZapHandler::hookRoom, (void*)this);
	m_room->addHook(GGZCoreRoom::tablelaunched, &GGZapHandler::hookRoom, (void*)this);
	m_room->addHook(GGZCoreRoom::tablelaunchfail, &GGZapHandler::hookRoom, (void*)this);
	m_room->addHook(GGZCoreRoom::tablejoined, &GGZapHandler::hookRoom, (void*)this);
	m_room->addHook(GGZCoreRoom::tablejoinfail, &GGZapHandler::hookRoom, (void*)this);
	m_room->addHook(GGZCoreRoom::tableleft, &GGZapHandler::hookRoom, (void*)this);
	m_room->addHook(GGZCoreRoom::tableleavefail, &GGZapHandler::hookRoom, (void*)this);
	m_room->addHook(GGZCoreRoom::tabledata, &GGZapHandler::hookRoom, (void*)this);
}

void GGZapHandler::detachRoomCallbacks()
{
	m_room->removeHook(GGZCoreRoom::playerlist, &GGZapHandler::hookRoom);
	m_room->removeHook(GGZCoreRoom::tablelist, &GGZapHandler::hookRoom);
	m_room->removeHook(GGZCoreRoom::chatnormal, &GGZapHandler::hookRoom);
	m_room->removeHook(GGZCoreRoom::chatannounce, &GGZapHandler::hookRoom);
	m_room->removeHook(GGZCoreRoom::chatprivate, &GGZapHandler::hookRoom);
	m_room->removeHook(GGZCoreRoom::chatbeep, &GGZapHandler::hookRoom);
	m_room->removeHook(GGZCoreRoom::enter, &GGZapHandler::hookRoom);
	m_room->removeHook(GGZCoreRoom::leave, &GGZapHandler::hookRoom);
	m_room->removeHook(GGZCoreRoom::tableupdate, &GGZapHandler::hookRoom);
	m_room->removeHook(GGZCoreRoom::tablelaunched, &GGZapHandler::hookRoom);
	m_room->removeHook(GGZCoreRoom::tablelaunchfail, &GGZapHandler::hookRoom);
	m_room->removeHook(GGZCoreRoom::tablejoined, &GGZapHandler::hookRoom);
	m_room->removeHook(GGZCoreRoom::tablejoinfail, &GGZapHandler::hookRoom);
	m_room->removeHook(GGZCoreRoom::tableleft, &GGZapHandler::hookRoom);
	m_room->removeHook(GGZCoreRoom::tableleavefail, &GGZapHandler::hookRoom);
	m_room->removeHook(GGZCoreRoom::tabledata, &GGZapHandler::hookRoom);
}

void GGZapHandler::attachGameCallbacks()
{
	m_game->addHook(GGZCoreGame::launched, &GGZapHandler::hookGame, (void*)this);
	m_game->addHook(GGZCoreGame::launchfail, &GGZapHandler::hookGame, (void*)this);
	m_game->addHook(GGZCoreGame::negotiated, &GGZapHandler::hookGame, (void*)this);
	m_game->addHook(GGZCoreGame::negotiatefail, &GGZapHandler::hookGame, (void*)this);
	m_game->addHook(GGZCoreGame::data, &GGZapHandler::hookGame, (void*)this);
	m_game->addHook(GGZCoreGame::over, &GGZapHandler::hookGame, (void*)this);
	m_game->addHook(GGZCoreGame::ioerror, &GGZapHandler::hookGame, (void*)this);
	m_game->addHook(GGZCoreGame::protoerror, &GGZapHandler::hookGame, (void*)this);
}

void GGZapHandler::detachGameCallbacks()
{
	m_game->removeHook(GGZCoreGame::launched, &GGZapHandler::hookGame);
	m_game->removeHook(GGZCoreGame::launchfail, &GGZapHandler::hookGame);
	m_game->removeHook(GGZCoreGame::negotiated, &GGZapHandler::hookGame);
	m_game->removeHook(GGZCoreGame::negotiatefail, &GGZapHandler::hookGame);
	m_game->removeHook(GGZCoreGame::data, &GGZapHandler::hookGame);
	m_game->removeHook(GGZCoreGame::over, &GGZapHandler::hookGame);
	m_game->removeHook(GGZCoreGame::ioerror, &GGZapHandler::hookGame);
	m_game->removeHook(GGZCoreGame::protoerror, &GGZapHandler::hookGame);
}

