#include "ggzap_handler.h"

#include "GGZCoreModule.h"
#include "GGZCoreGametype.h"

#include <iostream>

GGZapHandler::GGZapHandler()
: QObject()
{
	m_server = new GGZCoreServer();
	m_room = NULL;
	m_game = NULL;
	m_frontendtype = NULL;
	m_modulename = NULL;

	attachServerCallbacks();
}

GGZapHandler::~GGZapHandler()
{
	if(m_game)
	{
		detachGameCallbacks();
		delete m_game;
	}
	if(m_room)
	{
		detachRoomCallbacks();
		delete m_room;
	}

	detachServerCallbacks();
	delete m_server;
}

void GGZapHandler::init(const char *modulename)
{
	int result;

	m_modulename = modulename;

	m_server->setHost("localhost", 5688);
	result = m_server->connect();
	if(result == -1) emit signalState(connectfail);
}

void GGZapHandler::setFrontend(const char *frontendtype)
{
	m_frontendtype = frontendtype;
}

void GGZapHandler::process()
{
	if(m_server->dataPending()) m_server->dataRead();
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
	handler->hookRoomActive(id);
	return GGZ_HOOK_OK;
}

GGZHookReturn GGZapHandler::hookGame(unsigned int id, void *event_data, void *user_data)
{
	GGZapHandler *handler;

cout << "room " << id << endl;

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
			m_server->setLogin(GGZCoreServer::guest, "randomguest", "foo");
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

void GGZapHandler::hookRoomActive(unsigned int id)
{
	GGZPlayer *player;
	GGZCoreTable *table;
	GGZCoreModule *module;
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
			join = -1;
			for(int i = 0; i < m_room->countTables(); i++)
			{
				table = m_room->table(i);
				for(int j = 0; j < table->countSeats(); j++)
					if(table->playerType(j) == GGZ_SEAT_OPEN) join = i;
			}
			if(join != -1) m_room->joinTable(join);
			else
			{
				// launch table and wait...
				gametype = m_room->gametype();
				table = new GGZCoreTable();
				table->init(gametype->gametype(), "foobar", 2);
				ret = m_room->launchTable(table->table());
				delete table;
				if(ret >= 0) emit signalState(waiting);
				else emit signalState(startfail);
			}
			break;
		case GGZCoreRoom::enter:
			// recognize victims here :-) But... is this important?
			break;
		case GGZCoreRoom::tablejoined:
			module = new GGZCoreModule();
			gametype = m_room->gametype();
			module->init(gametype->name(), gametype->protocol());
			if(!module->count()) emit signalState(startfail);
			else
			{
				module->setActive(0);
				m_game = new GGZCoreGame();
				m_game->init(module->module());
				attachGameCallbacks();
				if(!m_game->launch()) emit signalState(startfail);
				else emit signalState(started);
			}
			break;
		case GGZCoreRoom::tablejoinfail:
			emit signalState(startfail);
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
