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
	view_servers->addColumn(i18n("Location"));
	view_servers->addColumn(i18n("Speed"));
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

	connect(remove, SIGNAL(clicked()), SIGNAL(signalChanged()));
	connect(remove_servers, SIGNAL(clicked()), SIGNAL(signalChanged()));
	connect(add, SIGNAL(clicked()), SIGNAL(signalChanged()));
	connect(add_servers, SIGNAL(clicked()), SIGNAL(signalChanged()));
	connect(autoconfig, SIGNAL(clicked()), SIGNAL(signalChanged()));
	connect(autoconfig_servers, SIGNAL(clicked()), SIGNAL(signalChanged()));
}

KCMGGZMetaserver::~KCMGGZMetaserver()
{
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
	MetaserverAdd dialog;
	connect(&dialog, SIGNAL(signalAdd(QString, QString)), SLOT(slotAdded(QString, QString)));
	dialog.exec();
}

void KCMGGZMetaserver::slotAddServer()
{
	MetaserverAddServer dialog_servers;
	connect(&dialog_servers, SIGNAL(signalAdd(QString, QString, QString, QString, QString)),
		SLOT(slotAddedServer(QString, QString, QString, QString, QString)));
	dialog_servers.exec();
}

void KCMGGZMetaserver::slotAuto()
{
	m_query = query;
	sock = new QSocket();
	connect(sock, SIGNAL(connected()), SLOT(slotAutoConnected()));
	connect(sock, SIGNAL(readyRead()), SLOT(slotAutoRead()));
	connect(sock, SIGNAL(error(int)), SLOT(slotAutoError(int)));
	sock->connectToHost("live.ggzgamingzone.org", 15689);
}

void KCMGGZMetaserver::slotAutoServer()
{
	m_query = query_server;
	sock = new QSocket();
	connect(sock, SIGNAL(connected()), SLOT(slotAutoConnected()));
	connect(sock, SIGNAL(readyRead()), SLOT(slotAutoRead()));
	connect(sock, SIGNAL(error(int)), SLOT(slotAutoError(int)));
	sock->connectToHost("live.ggzgamingzone.org", 15689);
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
				addServerURI(element.text(), "GGZ Gaming Zone", pref, QString::null, QString::null, QString::null);
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

void KCMGGZMetaserver::slotAddedServer(QString uri, QString type, QString location, QString speed, QString comment)
{
	addServerURI(uri, type, "100", QString::null, QString::null, comment);
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

void KCMGGZMetaserver::addServerURI(QString uri, QString type, QString preference, QString location, QString speed, QString comment)
{
	for(QListViewItem *item = view_servers->firstChild(); item; item = item->nextSibling())
		if(item->text(0) == uri)
		{
			delete item;
			break;
		}

	if(!location) location = i18n("(unknown)");
	if(!comment) comment = i18n("(none)");
	if(!speed) speed = i18n("(unknown)");
	if(!preference) preference = "100";

	(void)new KListViewItem(view_servers, uri, type, preference, location, speed, comment);
}

void KCMGGZMetaserver::slotSelected(QListViewItem *item, const QPoint& point, int column)
{
	if(!item) return;
	/*if(!popup)
	{
		popup = new QPopupMenu(this);
		popup->insertItem(i18n("Visit the project home page"), menuhomepage);
		popup->insertItem(i18n("Show all information"), menuinformation);
		connect(popup, SIGNAL(activated(int)), SLOT(slotActivated(int)));
	}

	popup->popup(point);*/
}

void KCMGGZMetaserver::load()
{
	int found;
	QStringList servers, metaservers;

	// Read directory;
	KConfig conf("ggzmetaserverrc");
	conf.setGroup("Directory");
	servers = conf.readListEntry("servers");
	metaservers = conf.readListEntry("metaservers");

	// Load local meta servers
	found = 0;
	conf.setGroup("Meta servers");
	for(QStringList::iterator s = metaservers.begin(); s != metaservers.end(); s++)
	{
		addURI((*s), conf.readEntry((*s)));
		found++;
	}

	// Load default meta servers
	if(!found)
	{
		addURI("ggzmeta://mindx.dyndns.org", "ggz");
		addURI("ggzmeta://live.ggzgamingzone.org", "ggz");
		addURI("http://www.freeciv.org/metaserver.html", "freeciv");
	}

	// Load local game servers
	found = 0;
	conf.setGroup("Game servers");
	for(QStringList::iterator s = servers.begin(); s != servers.end(); s++)
	{
		QStringList list;
		list = conf.readListEntry((*s));
		if(list.count() == 5)
		{
			found++;
			addServerURI((*s), *(list.at(0)), *(list.at(1)), *(list.at(2)), *(list.at(3)), *(list.at(4)));
		}
	}

	// Load default game servers
	if(!found)
	{
		addServerURI("ggz://ggz.jzaun.com:5689", "GGZ Gaming Zone", "10", "Pennsylvania/USA", QString::null, "Justin's Developer server");
		addServerURI("ggz://live.ggzgamingzone.org", "GGZ Gaming Zone", "10", "Berlin/Germany", QString::null, "GGZ Europe One server");
		addServerURI("kmonop://somewhere.org", "Atlantik", "10", "Whereever", QString::null, "Yet another host");
		addServerURI("freeciv://civserver.freeciv.org", "FreeCiv", "10", "Anywhere", QString::null, "SmallPox is cool");
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
		list << item->text(1) << item->text(2) << item->text(3) << item->text(4) << item->text(5);
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



