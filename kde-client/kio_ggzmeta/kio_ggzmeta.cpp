// Header files
#include "kio_ggzmeta.h"
#include "helper.h"

// KDE includes
#include <kinstance.h>
#include <kio/netaccess.h>
#include <kio/job.h>
// temp
#include <fstream>

// Qt includes
#include <qfile.h>
#include <qtextstream.h>
#include <qregexp.h>
#include <qstringlist.h>
#include <qdom.h>
#include <qsocket.h>
#include <qapplication.h>

// Global QApplication object to be used for events
QApplication *a;

// Constructor
GGZMetaProtocol::GGZMetaProtocol(const QCString& pool, const QCString& app)
: QObject(), SlaveBase("ggzmeta", pool, app)
{
	m_sock = NULL;

	// Official quick hack (tm)
	int argc = 1;
	char *argv[2] = {"ggzmeta", NULL};
	a = new QApplication(argc, argv);

	debug("IO loaded.");
}

// Destructor
GGZMetaProtocol::~GGZMetaProtocol()
{
}

// Process URI
void GGZMetaProtocol::jobOperator(const KURL& url)
{
	KURL u;
	QString p, h;

	u = url;
	u.cleanPath();

	debug(">> " + u.url() + " :: host=" + u.host() + " path=" + u.path(1));

	if(u.host())
	{
		// When class is specified, go for it...
		debug(QString("Class: ") + u.host());
		work(u.host(), u.path());
	}
	else
	{
		// ...else just output some information FIXME! Use HTML
		debug("General information about ggzmeta://");
		QCString output;
		mimeType("text/plain");
		output.sprintf("This is the GGZ Gaming Zone Meta Server IO Slave.\n"
			"See http://ggz.sourceforge.net/metaserver/ for more details.\n");
		data(output);
		finished();
	}
}

// Result slot for async copy operations
void GGZMetaProtocol::slotResult(KIO::Job *job)
{
	debug(QString("ggz -> slotResult: %1").arg(m_class));

	delegate(m_class, m_temp);
	// rm(temp); FIXME! KTempFile
	a->quit();
}

// Result slot for interactive meta servers
void GGZMetaProtocol::slotWrite()
{
	QString s;
	QStringList l;

	debug("ggz -> slotWrite()");
	l = l.split('/', m_query);
	if(l.count() == 2)
	{
		// Recognized valid GGZ meta server URI-style query
		debug("Write...");
		s = QString("<?xml version=\"1.0\"><query class=\"%1\" type=\"%2\">%3</query>\n").arg(m_class).arg(*(l.at(0))).arg(*(l.at(1)));
		debug(QString("Write: %1").arg(s));
		m_sock->writeBlock(s.latin1(), s.length());
		m_sock->flush();
	}
	else error(0, "Wrong format!");
}

// Result slot for meta server answers
void GGZMetaProtocol::slotRead()
{
	QString rdata;
	QDomDocument dom;
	QDomNode node;
	QDomElement element;
	QString pref;
	QStringList list;
	QString host, hostname;

	debug("ggz -> slotRead");

	if(m_class == "crossfire")
	{
// 131.246.89.16|38|bratwurst.unix-ag.uni-kl.de|0|1.0.0-07-30|Running CVS of 2001-09-06|151725|3503971|51698
		while(m_sock->bytesAvailable())
		{
			rdata = m_sock->readLine();
			list = list.split('|', rdata);
			host = *(list.at(0));
			hostname = *(list.at(1));
			GGZMetaProtocolHelper::app_file(entry, QString("%1_%2").arg(host).arg(hostname), 1, "application/x-desktop");
			listEntry(entry, false);
		}
		listEntry(entry, true);
		finished();
	}
	else if(m_class == "netrek")
	{
//Server Host                             Port     Ago  Status            Flags
//--------------------------------------- -------- ---- ----------------- -------
//-h kirk.hal-pc.org                      -p 2592   17  * Timed out        DR   B
		while(m_sock->bytesAvailable())
		{
			rdata = m_sock->readLine();
			list = list.split(' ', rdata);
			if(*(list.at(0)) == "-h")
			{
				host = *(list.at(1));
				GGZMetaProtocolHelper::app_file(entry, QString("%1").arg(host), 1, "application/x-desktop");
				listEntry(entry, false);
			}
		}
		listEntry(entry, true);
		finished();
	}
	else if(m_class == "ggz")
	{
		rdata = m_sock->readLine();
		rdata.truncate(rdata.length() - 1);
		debug(QString("Read raw: %1").arg(rdata));
	 
		dom.setContent(rdata);
		node = dom.documentElement().firstChild();
		while(!node.isNull())
		{
			element = node.toElement();
			if(!element.firstChild().isNull())
			{
				element = element.firstChild().toElement();
				pref = element.attribute("preference", "20");
				GGZMetaProtocolHelper::app_file(entry, QString("%1_%2").arg(element.text()).arg(pref), 1, "application/x-desktop");
				listEntry(entry, false);
				debug(QString("-> entry: %1_%2").arg(element.text()).arg(pref));
			}
			node = node.nextSibling();
		}
		listEntry(entry, true);
		finished();
	}
	else error(0, "No such class!");

	delete m_sock;
	a->quit();
}

// Dispatcher: Select method based on query information
void GGZMetaProtocol::work(QString queryclass, QString query)
{
	m_class = queryclass;
	m_query = query;

	if(queryclass == "freeciv")
	{
		debug("** start freeciv download **");

		m_temp = "/tmp/freeciv.metaserver";
		KIO::Job *job = KIO::copy("http://meta.freeciv.org/metaserver/", m_temp);
		connect(job, SIGNAL(result(KIO::Job)), SLOT(slotResult(KIO::Job)));

		a->exec();

		/*QString tmp;
		if(KIO::NetAccess::download("http://meta.freeciv.org/metaserver/", tmp))
		{
			delegate(queryclass, tmp);
			KIO::NetAccess::removeTempFile(tmp);
		}
		else error(0, QString("Couldn't process freeciv query: %1").arg(query));*/
	}
	else if(queryclass == "atlantik")
	{
		debug("** start atlantik download **");

		m_temp = "/tmp/atlantik.metaserver";
		KIO::Job *job = KIO::copy("http://gator.monopd.net", m_temp);
		connect(job, SIGNAL(result(KIO::Job)), SLOT(slotResult(KIO::Job)));

		a->exec();

		/*QString tmp;
		if(KIO::NetAccess::download("http://gator.monopd.net", tmp))
		{
			delegate(queryclass, tmp);
			KIO::NetAccess::removeTempFile(tmp);
		}
		else error(0, QString("Couldn't process atlantik query: %1").arg(query));*/
	}
	else if(queryclass == "dopewars")
	{
		debug("** start dopewars download **");

		m_temp = "/tmp/dopewars.metaserver";
		KIO::Job *job = KIO::copy("http://dopewars.sourceforge.net/metaserver.php?getlist=2", m_temp);
		connect(job, SIGNAL(result(KIO::Job)), SLOT(slotResult(KIO::Job)));

		a->exec();
	}
	else if(queryclass == "crossfire")
	{
		if((m_query.isEmpty()) || (m_query == "/"))
		{
			m_sock = new QSocket();
			connect(m_sock, SIGNAL(readyRead()), SLOT(slotRead()));
			m_sock->connectToHost("crossfire.real-time.com", 13326);

			a->exec();
		}
		else
		{
			QCString output;
			mimeType("application/x-desktop");
			output.sprintf("[Desktop Entry]\n"
				"Comment=Crossfire Game\n"
				"Exec=cfclient\n"
				"Icon=crossfire\n"
				"MimeType=Application\n"
				"Name=Crossfire\n"
				"Type=Application\n");
			data(output);
			finished();
		}
	}
	else if(queryclass == "netrek")
	{
		if((m_query.isEmpty()) || (m_query == "/"))
		{
			m_sock = new QSocket();
			connect(m_sock, SIGNAL(readyRead()), SLOT(slotRead()));
			m_sock->connectToHost("metaserver.netrek.org", 3521);

			a->exec();
		}
		else
		{
			QCString output;
			mimeType("application/x-desktop");
			output.sprintf("[Desktop Entry]\n"
				"Comment=Netrek Game\n"
				"Exec=netrek\n"
				"Icon=netrek\n"
				"MimeType=Application\n"
				"Name=Netrek\n"
				"Type=Application\n");
			data(output);
			finished();
		}
	}
	else
	{
		// ggz and other non-delegates supported by the GGZ Meta Server directly
		debug("** ggz meta server **");

		m_sock = new QSocket();
		connect(m_sock, SIGNAL(connected()), SLOT(slotWrite()));
		connect(m_sock, SIGNAL(readyRead()), SLOT(slotRead()));
		m_sock->connectToHost("localhost", 15689);

		a->exec();
	}
}

// Special delegation for some classes
void GGZMetaProtocol::delegate(QString queryclass, QString url)
{
	if(queryclass == "freeciv")
	{
		debug("*** delegation: freeciv ***");
		QFile file(url);
		if(file.open(IO_ReadOnly))
		{
			QTextStream t(&file);
			int counter = 0;
			while(!t.eof())
			{
				QString s = t.readLine();
				if((s.startsWith("<TR")) && (counter++))
				{
					QStringList l;
					l = l.split(QRegExp("<[^>]+>"), s);
					QString host = *(l.at(0));
					QString port = *(l.at(2));
					GGZMetaProtocolHelper::app_file(entry, QString("%1:%2").arg(host).arg(port), 1, "application/x-desktop");
					listEntry(entry, false);
				}
			}
			listEntry(entry, true);
			finished();
			file.close();
		}
	}
	else if(queryclass == "dopewars")
	{
		debug("*** delegation: dopewars ***");
		QFile file(url);
		if(file.open(IO_ReadOnly))
		{
			QTextStream t(&file);
			int counter = 0;
			while(!t.eof())
			{
				QString s = t.readLine();
				QString q;
				if((s.startsWith("<tr")) && (counter++))
				{
					int i = 0;
					//QListViewItem *item = new QListViewItem(this);
					while(!s.contains("</tr>"))
					{
						QStringList l;
						l = l.split(QRegExp("<[^>]+>"), s);
						//item->setText(i, *(l.at(0)));
						if(!q.isEmpty()) q.append("_");
						q.append(*(l.at(0)));
						i++;
						s = t.readLine();
					}
					GGZMetaProtocolHelper::app_file(entry, q, 1, "application/x-desktop");
					listEntry(entry, false);
				}
			}
			file.close();
			listEntry(entry, true);
			finished();
		}
	}
	else if(queryclass == "atlantik")
	{
		QString rdata;
		QDomDocument dom;
		QDomNode node;
		QDomElement element;

		debug("*** delegation: atlantik ***");
		// ... (there is nothing yet except <monopigator></monopigator>)
		finished();
	}
	else error(0, QString("Unknown query class: %1").arg(queryclass));
}

// Request directory listing
void GGZMetaProtocol::listDir(const KURL& url)
{
	jobOperator(url);
}

// Request any URI
void GGZMetaProtocol::get(const KURL& url)
{
	jobOperator(url);
}

// Request file statistics
void GGZMetaProtocol::stat(const KURL& url)
{
	GGZMetaProtocolHelper::app_file(entry, QString("Whatever"), 1, "application/x-desktop");

	statEntry(entry);
	finished();
}

// Request file type
void GGZMetaProtocol::mimetype(const KURL& url)
{
	mimeType("application/x-desktop");
}

// Initialization
void GGZMetaProtocol::init(const KURL& url)
{
}

// Entry point
extern "C"
{
	int kdemain(int argc, char **argv)
	{
		KInstance instance("kio_ggzmeta");
		GGZMetaProtocol slave(argv[2], argv[3]);
		slave.dispatchLoop();
		return 0;
	}
}

// Temporary debug function
void GGZMetaProtocol::debug(QString s)
{
	ofstream dbg;
	dbg.open("/tmp/kio_ggzmeta.debug", ios::app);
	dbg << s.latin1() << endl;
	dbg.close();
}

