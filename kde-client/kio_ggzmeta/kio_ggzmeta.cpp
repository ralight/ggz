#include "kio_ggzmeta.h"
#include "helper.h"

#include <kinstance.h>
#include <kio/netaccess.h>
#include <kio/job.h>
// temp
#include <fstream>

#include <qfile.h>
#include <qtextstream.h>
#include <qregexp.h>
#include <qstringlist.h>
#include <qdom.h>
#include <qsocket.h>
#include <qwidget.h>
#include <qapplication.h>

GGZMetaProtocol *me;

GGZMetaProtocol::GGZMetaProtocol(const QCString& pool, const QCString& app)
: QObject(), SlaveBase("ggzmeta", pool, app)
{
	m_sock = NULL;
	me = this;

	debug("IO loaded.");
}

GGZMetaProtocol::~GGZMetaProtocol()
{
}

void GGZMetaProtocol::jobOperator(const KURL& url)
{
	KURL u;
	QString p, h;

	u = url;
	u.cleanPath();

	debug(">> " + u.url() + " :: host=" + u.host() + " path=" + u.path(0));

	if(u.host())
	{
		debug(QString("Class: ") + u.host());
		work(u.host(),  u.path());
	}
	else
	{
		debug("General information about ggzmeta://");
		QCString output;
		mimeType("text/plain");
		output.sprintf("This is the GGZ Gaming Zone Meta Server IO Slave.\n"
			"See http://ggz.sourceforge.net/metaserver/ for more details.\n");
		data(output);
		finished();
	}
}

void GGZMetaProtocol::slotResult(KIO::Job *job)
{
	debug("job is ready!!");
}

void GGZMetaProtocol::slotWrite()
{
	QString s;
	QStringList l;

	debug("ggz -> slotWrite()");
	l = l.split('/', m_query);
	if(l.count() == 2)
	{
		debug("Write...");
		s = QString("<?xml version=\"1.0\"><query class=\"%1\" type=\"%2\">%3</query>\n").arg(m_class).arg(*(l.at(0))).arg(*(l.at(1)));
		m_sock->writeBlock(s.latin1(), s.length());
		m_sock->flush();
	}
	else
	{
		error(0, "Wrong format!");
		qApp->exit_loop();
	}
}

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
			GGZMetaProtocolHelper::app_file(entry, QString("%1_%2").arg(host).arg(hostname), 1);
		}
	}
	else
	{
		rdata = m_sock->readLine();
		rdata.truncate(rdata.length() - 1);
	 
		dom.setContent(rdata);
		node = dom.documentElement().firstChild();
		while(!node.isNull())
		{
			element = node.toElement();
			if(!element.firstChild().isNull())
			{
				element = element.firstChild().toElement();
				pref = element.attribute("preference", "20");
				GGZMetaProtocolHelper::app_file(entry, QString("%1_%2").arg(element.text()).arg(pref), 1);
				listEntry(entry, false);
			}
			node = node.nextSibling();
		}
		listEntry(entry, true);
		finished();
	}

	delete m_sock;

	qApp->exit_loop();
}

void GGZMetaProtocol::work(QString queryclass, QString query)
{
	m_class = queryclass;
	m_query = query;

	if(queryclass == "freeciv")
	{
		QString tmp;
		/*xxx*/
		debug("running kio::job");
		KIO::Job *job = KIO::copy("http://meta.freeciv.org/metaserver/", "/tmp/freeciv.metaserver");
		connect(job, SIGNAL(result(KIO::Job)), SLOT(slotResult(KIO::Job)));
		loop();
		/*xxx*/
		debug("** start freeciv download **");
		if(KIO::NetAccess::download("http://meta.freeciv.org/metaserver/", tmp))
		{
			delegate(queryclass, tmp);
			KIO::NetAccess::removeTempFile(tmp);
		}
		else error(0, QString("Couldn't process freeciv query: %1").arg(query));
	}
	else if(queryclass == "atlantik")
	{
		debug("** start atlantik download **");

		QString tmp;
		if(KIO::NetAccess::download("http://gator.monopd.net", tmp))
		{
			delegate(queryclass, tmp);
			KIO::NetAccess::removeTempFile(tmp);
		}
		else error(0, QString("Couldn't process atlantik query: %1").arg(query));
	}
	else if(queryclass == "crossfire")
	{
		m_sock = new QSocket();
		connect(m_sock, SIGNAL(readyRead()), SLOT(slotRead()));
		m_sock->connectToHost("crossfire.real-time.com", 13326);
	}
	else
	{
		// ggz and other non-delegates supported by the GGZ Meta Server directly
		//error(QString("Unsupported class: %1").arg(queryclass));
		debug("** ggz meta server **");

		m_sock = new QSocket();
		connect(m_sock, SIGNAL(connected()), SLOT(slotWrite()));
		connect(m_sock, SIGNAL(readyRead()), SLOT(slotRead()));
		m_sock->connectToHost("localhost", 15689);

		loop();
	}
}

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
					//QListViewItem *item = new QListViewItem(this);
					//int i = 0;
					QString host = *(l.at(0));
					QString port = *(l.at(2));
					GGZMetaProtocolHelper::app_file(entry, QString("%1:%2").arg(host).arg(port), 1);
					listEntry(entry, false);
					//for(QStringList::Iterator it = l.begin(); it != l.end(); it++, i++);
						//item->setText(i, (*it));
				}
			}
			listEntry(entry, true);
			finished();
			file.close();
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
	qApp->exit_loop();
}

void GGZMetaProtocol::listDir(const KURL& url)
{
	jobOperator(url);
}

void GGZMetaProtocol::get(const KURL& url)
{
	jobOperator(url);
}

void GGZMetaProtocol::init(const KURL& url)
{
}

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

void qt_enter_modal(QWidget *widget);
void qt_leave_modal(QWidget *widget);

void GGZMetaProtocol::loop()
{
	debug("May the trolls be with you...");
	QWidget dummy(0, 0, WType_Modal);
	debug("1 troll");
	qt_enter_modal(&dummy);
	debug("2 trolls");
	qApp->enter_loop();
	debug("3 trolls");
	qt_leave_modal(&dummy);
	debug("4 trolls");
}

void GGZMetaProtocol::debug(QString s)
{
	ofstream dbg;
	dbg.open("/tmp/kio_ggzmeta.debug", ios::app);
	dbg << s.latin1() << endl;
	dbg.close();
}

