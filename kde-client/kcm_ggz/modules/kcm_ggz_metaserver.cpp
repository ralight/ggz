#include "kcm_ggz_metaserver.h"
#include "kcm_ggz_metaserver.moc"

#include "metaserver_add.h"
#include "metaserver_add_server.h"

#include <klistview.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kconfig.h>
#include <kdebug.h>

#include <qlayout.h>
#include <qgroupbox.h>
#include <qsocket.h>
#include <qdom.h>
#include <qpushbutton.h>

#include "config.h"

KCMGGZMetaserver::KCMGGZMetaserver(QWidget *parent, const char *name)
: KCMGGZPane(parent, name)
{
	QVBoxLayout *vbox, *vbox2, *vbox3, *vbox4;
	QHBoxLayout *hbox, *hbox2;
	QPushButton *add, *remove, *autoconfig;
	QPushButton *add_servers, *remove_servers, *autoconfig_servers;
	QGroupBox *box, *box_servers;
	QVBoxLayout *vboxroot;

	dialog = NULL;
	dialog_servers = NULL;
	m_query = query_invalid;

	box = new QGroupBox(i18n("Available meta servers"), this);
	box_servers = new QGroupBox(i18n("Available game servers"), this);

	view = new KListView(box);
	view->addColumn(i18n("URI"));
	view->addColumn(i18n("Supported classes"));

	add = new QPushButton(i18n("Add a metaserver"), box);
	remove = new QPushButton(i18n("Remove server"), box);
	autoconfig = new QPushButton(i18n("Autoconfigure"), box);

	add_servers = new QPushButton(i18n("Add a server"), box_servers);
	remove_servers = new QPushButton(i18n("Remove server"), box_servers);
	autoconfig_servers = new QPushButton(i18n("Autoconfigure"), box_servers);

	view_servers = new KListView(box_servers);
	view_servers->addColumn(i18n("URI"));
	view_servers->addColumn(i18n("Type"));
	view_servers->addColumn(i18n("Priority"));
	view_servers->addColumn(i18n("Description"));

	vboxroot = new QVBoxLayout(this, 5);

	vboxroot->add(box);
	vbox = new QVBoxLayout(box, 15);
	hbox = new QHBoxLayout(vbox, 5);
	hbox->add(view);
	vbox2 = new QVBoxLayout(hbox, 5);
	vbox2->add(add);
	vbox2->add(remove);
	vbox2->add(autoconfig);

	vboxroot->add(box_servers);
	vbox3 = new QVBoxLayout(box_servers, 15);
	hbox2 = new QHBoxLayout(vbox3, 5);
	hbox2->add(view_servers);
	vbox4 = new QVBoxLayout(hbox2, 5);
	vbox4->add(add_servers);
	vbox4->add(remove_servers);
	vbox4->add(autoconfig_servers);

	connect(remove, SIGNAL(clicked()), SLOT(slotDelete()));
	connect(remove_servers, SIGNAL(clicked()), SLOT(slotDeleteServer()));
	connect(add, SIGNAL(clicked()), SLOT(slotAdd()));
	connect(add_servers, SIGNAL(clicked()), SLOT(slotAddServer()));
	connect(autoconfig, SIGNAL(clicked()), SLOT(slotAuto()));
	connect(autoconfig_servers, SIGNAL(clicked()), SLOT(slotAutoServer()));
}

KCMGGZMetaserver::~KCMGGZMetaserver()
{
	if(dialog) delete dialog;
	if(dialog_servers) delete dialog_servers;
}

void KCMGGZMetaserver::slotDelete()
{
	if(view->selectedItem()) delete view->selectedItem();
}

void KCMGGZMetaserver::slotDeleteServer()
{
	if(view_servers->selectedItem()) delete view_servers->selectedItem();
}

void KCMGGZMetaserver::slotAdd()
{
	if(!dialog)
	{
		dialog = new MetaserverAdd();
		connect(dialog, SIGNAL(signalAdd(QString, QString)), SLOT(slotAdded(QString, QString)));
	}
	dialog->show();
}

void KCMGGZMetaserver::slotAddServer()
{
	if(!dialog_servers)
	{
		dialog_servers = new MetaserverAddServer();
		connect(dialog_servers, SIGNAL(signalAdd(QString, QString, QString)), SLOT(slotAddedServer(QString, QString, QString)));
	}
	dialog_servers->show();
}

void KCMGGZMetaserver::slotAuto()
{
	m_query = query;
	sock = new QSocket();
	connect(sock, SIGNAL(connected()), SLOT(slotAutoConnected()));
	connect(sock, SIGNAL(readyRead()), SLOT(slotAutoRead()));
	connect(sock, SIGNAL(error(int)), SLOT(slotAutoError(int)));
	sock->connectToHost("ggz.snafu.de", 15689);
}

void KCMGGZMetaserver::slotAutoServer()
{
	m_query = query_server;
	sock = new QSocket();
	connect(sock, SIGNAL(connected()), SLOT(slotAutoConnected()));
	connect(sock, SIGNAL(readyRead()), SLOT(slotAutoRead()));
	connect(sock, SIGNAL(error(int)), SLOT(slotAutoError(int)));
	sock->connectToHost("ggz.snafu.de", 15689);
}

void KCMGGZMetaserver::slotAutoError(int error)
{
	QString errstr = i18n("Unknown error");

	switch(error)
	{
		case QSocket::ErrConnectionRefused:
			errstr = i18n("Connection refused");
			break;
		case QSocket::ErrHostNotFound:
			errstr = i18n("Host not found");
			break;
		case QSocket::ErrSocketRead:
			errstr = i18n("Read error");
			break;
	}

	KMessageBox::error(this, i18n("An error occured: %1").arg(errstr), i18n("Autoconfiguration failed"));
}

void KCMGGZMetaserver::slotAutoConnected()
{
	QString s;

	if(m_query == query) s = "<?xml version=\"1.0\"><query class=\"ggz\" type=\"meta\">0.1</query>\n";
	else s = QString("<?xml version=\"1.0\"><query class=\"ggz\" type=\"connection\">%1</query>\n").arg(VERSION);
	sock->writeBlock(s.latin1(), s.length());
    sock->flush();
}

void KCMGGZMetaserver::slotAutoRead()
{
	QString rdata;
	QDomDocument dom;
	QDomNode node;
	QDomElement element;
	QString pref;

	rdata = sock->readLine();
	rdata.truncate(rdata.length() - 1);

	dom.setContent(rdata);
	node = dom.documentElement().firstChild();
	while(!node.isNull())
	{
		element = node.toElement();
		if(!element.firstChild().isNull())
		{
			element = element.firstChild().toElement();
			if(m_query == query)
			{
				addURI(element.text(), "ggz");
			}
			else
			{
				pref = element.attribute("preference", "20");
				addServerURI(element.text(), "GGZ Gaming Zone", pref, "(none)");
			}
		}
		node = node.nextSibling();
	}

	delete sock;
}

void KCMGGZMetaserver::slotAdded(QString uri, QString proto)
{
	addURI(uri, proto);
}

void KCMGGZMetaserver::slotAddedServer(QString uri, QString type, QString comment)
{
	addServerURI(uri, type, "100", comment);
}

void KCMGGZMetaserver::addURI(QString uri, QString proto)
{
	for(QListViewItem *item = view->firstChild(); item; item = item->nextSibling())
		if(item->text(0) == uri)
		{
			delete item;
			break;
		}
	(void)new KListViewItem(view, uri, proto);
}

void KCMGGZMetaserver::addServerURI(QString uri, QString type, QString preference, QString comment)
{
	for(QListViewItem *item = view_servers->firstChild(); item; item = item->nextSibling())
		if(item->text(0) == uri)
		{
			delete item;
			break;
		}
	(void)new KListViewItem(view_servers, uri, type, preference, comment);
}

void KCMGGZMetaserver::load()
{
	QStringList servers, metaservers;

	// Load default game servers
	addServerURI("ggz://ggz.jzaun.com:5689", "GGZ Gaming Zone", "10", "Justin's Developer server");
	addServerURI("ggz://ggz.snafu.de", "GGZ Gaming Zone", "10", "GGZ Europe One server");
	addServerURI("kmonop://somewhere.org", "Atlantik", "10", "Yet another host");
	addServerURI("freeciv://civserver.freeciv.org", "FreeCiv", "10", "SmallPox is cool");

	// Load default meta servers
	addURI("ggzmeta://mindx.dyndns.org", "ggz");
	addURI("ggzmeta://ggz.snafu.de", "ggz");
	addURI("http://www.freeciv.org/metaserver.html", "freeciv");

	// Read directory;
	KConfig conf("ggzmetaserverrc");
	conf.setGroup("Directory");
	servers = conf.readListEntry("servers");
	metaservers = conf.readListEntry("metaservers");

	// Load local meta servers
	conf.setGroup("Meta servers");
	for(QStringList::iterator s = metaservers.begin(); s != metaservers.end(); s++)
	{
		addURI((*s), conf.readEntry((*s)));
	}

	// Load local game servers
	conf.setGroup("Game servers");
	for(QStringList::iterator s = servers.begin(); s != servers.end(); s++)
	{
		QStringList list;
		list = conf.readListEntry((*s));
		if(list.count() == 3)
			addServerURI((*s), *(list.at(0)), *(list.at(1)), *(list.at(2)));
	}
}

void KCMGGZMetaserver::save()
{
	QStringList servers, metaservers;
	KConfig conf("ggzmetaserverrc");

	// Save all meta servers
	conf.setGroup("Meta servers");
	for(QListViewItem *item = view->firstChild(); item; item = item->nextSibling())
	{
		conf.writeEntry(item->text(0), item->text(1));
		metaservers << item->text(0);
	}

	// Save all game servers
	conf.setGroup("Game servers");
	for(QListViewItem *item = view_servers->firstChild(); item; item = item->nextSibling())
	{
		QStringList list;
		list << item->text(1) << item->text(2) << item->text(3);
		conf.writeEntry(item->text(0), list);
		servers << item->text(0);
	}

	// Save directory
	conf.setGroup("Directory");
	conf.writeEntry("servers", servers);
	conf.writeEntry("metaservers", metaservers);
}

QString KCMGGZMetaserver::caption()
{
	return i18n("Servers");
}

extern "C"
{
	KCMGGZPane *kcmggz_init(QWidget *parent, const char *name)
	{
		return new KCMGGZMetaserver(parent, name);
	}
}



