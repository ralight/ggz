// Debugging (0 or 1)
#define SLAVE_DEBUG 1

// Header files
#include "kio_ggz.h"
#include "helper.h"

// KDE includes
#include <kinstance.h>
#include <kmessagebox.h>
#include <klocale.h>

// GGZCore++ includes
#include <GGZCore.h>
#include <GGZCoreServer.h>
#include <GGZCoreRoom.h>

// Miscellaneous includes
#if SLAVE_DEBUG
#include <fstream>
#include <kmessagebox.h>
#endif

// Static protocol handler
GGZProtocol *me;

GGZProtocol::GGZProtocol(const QCString& pool, const QCString& app)
: SlaveBase("ggz", pool, app)
{
	m_core = NULL;
	m_server = NULL;

	debug("IO slave loaded.");

	me = this;
	m_finished = false;
}

GGZProtocol::~GGZProtocol()
{
	if(m_server) delete m_server;
	if(m_core) delete m_core;
}

void GGZProtocol::jobOperator(const KURL& url)
{
	KURL u;
	QString h, vh, p;

	u = url;
	u.cleanPath(true);
	p = u.path(1);

	if(u.host())
	{
		debug("Host given, start processing...");
		h = u.host();
	}
	else
	{
		errormessage(i18n("No host given!"));
		return;
	}

	if(url.fileName() == "MOTD")
	{
		debug("action: show motd");
		showMotd();
		return;
	}
	else if(!m_server)
	{
		debug("action: init url");
		init(url);
	}
	else
	{
		debug("action: nothing (we already have a server)");
		return;
	}

	while(!m_finished)
	{
		if(m_server->dataPending()) m_server->dataRead();
	}
}

void GGZProtocol::listDir(const KURL& url)
{
	me->debug(":: listDir");
	jobOperator(url);
}

void GGZProtocol::get(const KURL& url)
{
	me->debug(":: get");
	jobOperator(url);
}

void GGZProtocol::stat(const KURL& url)
{
	me->debug(":: stat");

	KIO::UDSEntry entry;

	if(url.fileName() == "MOTD")
	{
		GGZProtocolHelper::app_file(entry, QString::null, 0);
		statEntry(entry);
	}
	else
	{
		GGZProtocolHelper::app_dir(entry, QString::null, 0);
		statEntry(entry);
	}

	finished();
}

GGZHookReturn GGZProtocol::hook_server_connect(unsigned int id, const void *event, const void *data)
{
	Q_UNUSED(id);
	Q_UNUSED(event);
	Q_UNUSED(data);

	me->debug("=> connect");

	while(!(me->server()->isOnline())) me->server()->dataRead(); // still that bug? oh my :)

	me->server()->setLogin(GGZCoreServer::guest, "kio_guest", "");

	me->debug("=> going to login soon...");
	me->server()->login();

	return GGZ_HOOK_OK;
}

GGZHookReturn GGZProtocol::hook_server_negotiated(unsigned int id, const void *event, const void *data)
{
	Q_UNUSED(id);
	Q_UNUSED(event);
	Q_UNUSED(data);

	me->debug("=> negotiated");

	return GGZ_HOOK_OK;
}

GGZHookReturn GGZProtocol::hook_server_login(unsigned int id, const void *event, const void *data)
{
	Q_UNUSED(id);
	Q_UNUSED(event);
	Q_UNUSED(data);

	me->debug("=> login");
	me->server()->listRooms(-1, 0);

	return GGZ_HOOK_OK;
}

GGZHookReturn GGZProtocol::hook_server_roomlist(unsigned int id, const void *event, const void *data)
{
	Q_UNUSED(id);
	Q_UNUSED(event);
	Q_UNUSED(data);

	me->debug("=> roomlist");
	for(int i = 0; i < me->server()->countRooms(); i++)
	{
		GGZCoreRoom *room = me->server()->room(i);
		GGZProtocolHelper::app_dir(me->entry, room->name(), 1);
		me->listEntry(me->entry, false);
		me->debug(QString("=> room: %1").arg(room->name()));
	}

	GGZProtocolHelper::app_file(me->entry, "MOTD", 1);
	me->listEntry(me->entry, false);

	me->listEntry(me->entry, true);
	me->finished();

	me->m_finished = 1;

	return GGZ_HOOK_OK;
}

GGZHookReturn GGZProtocol::hook_server_motd(unsigned int id, const void *event, const void *data)
{
	const GGZMotdEventData *motd;

	Q_UNUSED(id);
	Q_UNUSED(data);

	me->debug("=> motd");

	motd = (GGZMotdEventData*)event;
	me->savemotd = motd->motd;

	return GGZ_HOOK_OK;
}

GGZHookReturn GGZProtocol::hook_server_error(unsigned int id, const void *event, const void *data)
{
	Q_UNUSED(event);
	Q_UNUSED(data);

	me->errormessage(i18n("Server error: %1").arg(id));

	return GGZ_HOOK_OK;
}

void GGZProtocol::showMotd()
{
	QCString output;

	mimeType("text/html");

	if(!savemotd) output.sprintf(i18n("No MOTD found.\n"));
	else output.sprintf(i18n("This is the MOTD:\n\n%1\n").arg(savemotd).local8Bit());
	data(output);
	finished();
}

void GGZProtocol::init(const KURL& url)
{
	int port;

	debug(QString("Set host: %1").arg(url.host()));
	debug(QString("Port is: %1").arg(url.port()));

	if(url.port()) port = url.port();
	else port = 5688;

	m_core = new GGZCore();
	m_core->init(GGZCore::parser | GGZCore::modules);

	m_server = new GGZCoreServer();
	m_server->addHook(GGZCoreServer::connected, hook_server_connect);
	m_server->addHook(GGZCoreServer::negotiated, hook_server_negotiated);
	m_server->addHook(GGZCoreServer::loggedin, hook_server_login);
	m_server->addHook(GGZCoreServer::roomlist, hook_server_roomlist);
	m_server->addHook(GGZCoreServer::motdloaded, hook_server_motd);

	m_server->addHook(GGZCoreServer::connectfail, hook_server_error);
	m_server->addHook(GGZCoreServer::negotiatefail, hook_server_error);
	m_server->addHook(GGZCoreServer::negotiatefail, hook_server_error);
	m_server->addHook(GGZCoreServer::loginfail, hook_server_error);

	m_server->setHost(url.host().latin1(), port, 0);

	debug("Connecting...");
	m_server->connect();
}

extern "C"
{
	int kdemain(int argc, char **argv)
	{
		KInstance instance("kio_ggz");
		GGZProtocol slave(argv[2], argv[3]);
		slave.dispatchLoop();
		return 0;
	}
}

void GGZProtocol::debug(QString s)
{
#if SLAVE_DEBUG
	std::ofstream dbg;
	dbg.open("/tmp/kio_ggz.debug", std::ios::app);
	dbg << s.latin1() << std::endl;
	dbg.close();
#endif
}

void GGZProtocol::errormessage(QString s)
{
	debug("ERROR: " + s);
	error(KIO::ERR_SLAVE_DEFINED, s);
}

