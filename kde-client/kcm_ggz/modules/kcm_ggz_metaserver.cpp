#include "kcm_ggz_metaserver.h"
#include "kcm_ggz_metaserver.moc"

#include "metaserver_add.h"
#include "metaserver_add_server.h"

#include <klistview.h>
#include <qlayout.h>
#include <kpushbutton.h>
#include <qgroupbox.h>
#include <qsocket.h>
#include <qdom.h>

KCMGGZMetaserver::KCMGGZMetaserver(QWidget *parent, const char *name)
: KCMGGZPane(parent, name)
{
	QVBoxLayout *vbox, *vbox2, *vbox3, *vbox4;
	QHBoxLayout *hbox, *hbox2;
	KPushButton *add, *remove, *autoconfig;
	KPushButton *add_servers, *remove_servers, *autoconfig_servers;
	QGroupBox *box, *box_servers;
	QVBoxLayout *vboxroot;

	dialog = NULL;
	dialog_servers = NULL;
	m_query = query_invalid;

	box = new QGroupBox("Available meta servers", this);
	box_servers = new QGroupBox("Available game servers", this);

	view = new KListView(box);
	view->addColumn("URI");
	view->addColumn("Supported classes");

	add = new KPushButton("Add a metaserver", box);
	remove = new KPushButton("Remove server", box);
	autoconfig = new KPushButton("Autoconfigure", box);

	add_servers = new KPushButton("Add a server", box_servers);
	remove_servers = new KPushButton("Remove server", box_servers);
	autoconfig_servers = new KPushButton("Autoconfigure", box_servers);

	view_servers = new KListView(box_servers);
	view_servers->addColumn("URI");
	view_servers->addColumn("Type");
	view_servers->addColumn("Description");

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

	load();
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
	sock->connectToHost("localhost", 15689);
}

void KCMGGZMetaserver::slotAutoServer()
{
	m_query = query_server;
	sock = new QSocket();
	connect(sock, SIGNAL(connected()), SLOT(slotAutoConnected()));
	connect(sock, SIGNAL(readyRead()), SLOT(slotAutoRead()));
	sock->connectToHost("localhost", 15689);
}

void KCMGGZMetaserver::slotAutoConnected()
{
	QString s;

	if(m_query == query) s = "<?xml version=\"1.0\"><query class=\"ggz\" type=\"meta\">0.1</query>\n";
	else s = "<?xml version=\"1.0\"><query class=\"ggz\" type=\"connection\">0.0.5pre</query>\n";
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
				(void)new QListViewItem(view, element.text(), "ggz");
			}
			else
			{
				pref = element.attribute("preference", "20");
				(void)new QListViewItem(view_servers, element.text(), pref, "???");
			}
		}
		node = node.nextSibling();
	}

	delete sock;
}

void KCMGGZMetaserver::slotAdded(QString uri, QString proto)
{
	(void)new QListViewItem(view, uri, proto);
}

void KCMGGZMetaserver::slotAddedServer(QString uri, QString type, QString comment)
{
	(void)new QListViewItem(view_servers, uri, type, comment);
}

void KCMGGZMetaserver::load()
{
	(void)new QListViewItem(view_servers, "ggz://jzaun.com", "GGZ Gaming Zone", "Justin's Developer server");
	(void)new QListViewItem(view_servers, "ggz://ggz.snafu.de", "GGZ Gaming Zone", "GGZ Europe One server");
	(void)new QListViewItem(view_servers, "kmonop://somewhere.org", "Atlantik", "Yet another host");
	(void)new QListViewItem(view_servers, "freeciv://civserver.freeciv.org", "FreeCiv", "SmallPox is cool");

	(void)new QListViewItem(view, "ggzmeta://jzaun.com", "ggz");
	(void)new QListViewItem(view, "ggzmeta://ggz.snafu.de", "ggz");
	(void)new QListViewItem(view, "http://www.freeciv.org/metaserver.html", "freeciv");
}

void KCMGGZMetaserver::save()
{
}

const char *KCMGGZMetaserver::caption()
{
	return "Servers";
}

extern "C"
{
	KCMGGZPane *kcmggz_init(QWidget *parent, const char *name)
	{
		return new KCMGGZMetaserver(parent, name);
	}
}

