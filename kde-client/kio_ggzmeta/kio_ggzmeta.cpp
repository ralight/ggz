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

GGZMetaProtocol *me;

GGZMetaProtocol::GGZMetaProtocol(const QCString& pool, const QCString& app)
: SlaveBase("ggzmeta", pool, app)
{
	debug("IO loaded.");

	me = this;
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

void GGZMetaProtocol::work(QString queryclass, QString query)
{
	m_class = queryclass;

	if(queryclass == "freeciv")
	{
		QString tmp;
		/*xxx*/
		debug("running kio::job");
		KIO::Job *job = KIO::copy("http://meta.freeciv.org/metaserver/", "/tmp/freeciv.metaserver");
		connect(job, SIGNAL(result(KIO::Job)), SLOT(slotResult(KIO::Job)));
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
	}
	else
	{
		// ggz and other non-delegates supported by the GGZ Meta Server directly
		//error(QString("Unsupported class: %1").arg(queryclass));
		debug("** ggz meta server **");
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
	} else error(0, QString("Unknown query class: %1").arg(queryclass));
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

void GGZMetaProtocol::debug(QString s)
{
	ofstream dbg;
	dbg.open("/tmp/kio_ggzmeta.debug", ios::app);
	dbg << s.latin1() << endl;
	dbg.close();
}

