#include "kio_ggz.h"
#include "helper.h"

#include <kinstance.h>

// temp
#include <fstream>
#include <kmessagebox.h>

GGZProtocol::GGZProtocol(const QCString& pool, const QCString& app)
: SlaveBase("ggz", pool, app)
{
}

GGZProtocol::~GGZProtocol()
{
}

void GGZProtocol::jobOperator(const KURL& url)
{
	KURL u;
	QString h, vh, p;

ofstream cdbg;

cdbg.open("/tmp/csdb.messages");

cdbg << "*** PATH BEFORE *** " << url.host() + " | " + url.path() << endl;

	u = url;
	u.cleanPath();
	p = u.path(1);

	if(u.host())
	{
		h = u.host();
	}
	else
	{
		vh = u.host();
		h = "[unknown]";
	}

cdbg << "*** PATH BETWEEN *** " << vh + " | " + h + " | " + p << endl;

	if((p.left(0) == "/") && (p.left(1) == "/")) p = p.right(p.length() - 1);

cdbg << "*** PATH AFTER *** " << vh + " | " + h + " | " + p << endl;
cdbg << "Slashes: " << p.contains("/") << endl;

	switch(p.contains("/"))
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

void GGZProtocol::do_get(const KURL& url)
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


