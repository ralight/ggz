#include "ggzap_handler.h"

GGZapHandler::GGZapHandler()
: QObject()
{
	m_server = new GGZCoreServer();
	m_room = NULL;

	attachServerCallbacks();
}

GGZapHandler::~GGZapHandler()
{
	if(m_room) delete m_room;

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

GGZHookReturn GGZapHandler::hookServer(unsigned int id, void *event_data, void *user_data)
{
	GGZapHandler *handler;

	handler = (GGZapHandler*)user_data;
	handler->hook(id);
}

void GGZapHandler::hook(unsigned int id)
{
	switch(id)
	{
		case GGZCoreServer::connected:
			emit signalState(connected);
			break;
		case GGZCoreServer::connectfail:
			emit signalState(connectfail);
			break;
	}
}

///////////////////////////////////////////////////////////////////
// stolen from kggz
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

