#include "kio_ggz.h"
#include "helper.h"

#include <kinstance.h>
#include <kmessagebox.h>

#include <GGZCore.h>
#include <GGZCoreServer.h>
#include <GGZCoreRoom.h>

// temp
#include <fstream>
#include <kmessagebox.h>

using namespace std;

GGZProtocol *me;

GGZProtocol::GGZProtocol(const QCString& pool, const QCString& app)
: SlaveBase("ggz", pool, app)
{
	m_core = NULL;
	m_server = NULL;

	debug("IO loaded.");

	me = this;
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
		debug("IO starting...");
		h = u.host();
	}
	else
	{
		error("No host given!");
		return;
	}

	/*switch(p.contains("/"))
	{
		case 0:
		case 1:
			do_listServers(u);
			break;
		case 2:
			do_listRooms(u);
			break;
		default:
			do_get(u);
			break;
	}*/

	/*do_listRooms(u);*/

	if(url.fileName() == "MOTD")
	{
		showMotd();
		return;
	}
	else if(!m_server)
	{
		init(url);
	}
	else return;

	// FIXME!!!
	while(1)
	{
		if(m_server->dataPending()) m_server->dataRead();
	}
}

void GGZProtocol::listDir(const KURL& url)
{
	jobOperator(url);
}

void GGZProtocol::get(const KURL& url)
{
	jobOperator(url);
}

GGZHookReturn GGZProtocol::hook_server_connect(unsigned int id, const void *event, const void *data)
{
	me->debug("=> connect");

	while(!(me->server()->isOnline())) me->server()->dataRead(); // still that bug? oh my :)

	me->server()->setLogin(GGZCoreServer::guest, "kio_guest", "");

	me->debug("=> going to login soon...");
	me->server()->login();

	return GGZ_HOOK_OK;
}

GGZHookReturn GGZProtocol::hook_server_negotiated(unsigned int id, const void *event, const void *data)
{
	me->debug("=> negotiated");

	return GGZ_HOOK_OK;
}

GGZHookReturn GGZProtocol::hook_server_login(unsigned int id, const void *event, const void *data)
{
	me->debug("=> login");
	me->server()->listRooms(-1, 0);

	return GGZ_HOOK_OK;
}

GGZHookReturn GGZProtocol::hook_server_roomlist(unsigned int id, const void *event, const void *data)
{
	me->debug("=> roomlist");
	for(int i = 0; i < me->server()->countRooms(); i++)
	{
		GGZCoreRoom *room = me->server()->room(i);
		GGZProtocolHelper::app_dir(me->entry, room->name(), 1);
		me->listEntry(me->entry, false);
	}

	GGZProtocolHelper::app_file(me->entry, "MOTD", 1);
	me->listEntry(me->entry, false);

	me->listEntry(me->entry, true);
	me->finished();

	me->debug("=> request motd soon...");
	me->server()->motd();

	return GGZ_HOOK_OK;
}

GGZHookReturn GGZProtocol::hook_server_motd(unsigned int id, const void *event, const void *data)
{
	me->debug("=> motd");
	//GGZProtocolHelper::app_file(me->entry, "MOTD", 1);
	//me->listEntry(me->entry, false);

	//me->listEntry(me->entry, true);
	//me->finished();

	me->savemotd = ((char**)event)[0];

	return GGZ_HOOK_OK;
}

GGZHookReturn GGZProtocol::hook_server_error(unsigned int id, const void *event, const void *data)
{
	me->error(QString("Server error: %1").arg(id));

	return GGZ_HOOK_OK;
}

void GGZProtocol::showMotd()
{
	QCString output;

	mimeType("text/plain");

	if(!savemotd) output.sprintf("No MOTD found.\n");
	else output.sprintf("This is the MOTD:\n\n%1\n", savemotd.latin1());
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
	//m_server->setLogin(GGZCoreServer::guest, "guest", NULL);

	debug("Connecting...");
	m_server->connect();
}

/*void GGZProtocol::do_get(const KURL& url)
{
	QString name;
	QCString output;

	mimeType("text/html");

	name = url.path();
	if((name.isEmpty()) || (name == "/"))
	{
		name = "GGZ Welcome";
	}
	else
	{
		name = name.mid(1);
	}
	output.sprintf("<HTML><BODY>* Joining %s...</BODY></HTML>\n", name.latin1());

	data(output);
	finished();
}

void GGZProtocol::do_listServers(const KURL& url)
{
	KIO::UDSEntry entry;

	GGZProtocolHelper::app_dir(entry, "GGZ Europe One", 1);
	listEntry(entry, false);
	GGZProtocolHelper::app_dir(entry, "GGZ Morat.net", 1);
	listEntry(entry, false);

	listEntry(entry, true);
	finished();
}

void GGZProtocol::do_listRooms(const KURL& url)
{
	KIO::UDSEntry entry;

	GGZProtocolHelper::app_file(entry, "NetSpades.ggzroom", 1);
	listEntry(entry, false);
	GGZProtocolHelper::app_file(entry, "Hastings1066.ggzroom", 1);
	listEntry(entry, false);
 
	listEntry(entry, true);
	finished();
}*/

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
	ofstream dbg;
	dbg.open("/tmp/kio_ggz.debug", ios::app);
	dbg << s.latin1() << endl;
	dbg.close();
}

void GGZProtocol::error(QString s)
{
	//KMessageBox::error(NULL, s, "GGZ (ioslave) error");
	debug("ERROR: " + s);
}

