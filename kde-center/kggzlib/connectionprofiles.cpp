// Connection dialog includes
#include "connectionprofiles.h"

// KGGZ includes
#include "serverlist.h"
#include "configwidget.h"
#include "serverselector.h"

// Qt includes
#include <qlayout.h>
#include <qpushbutton.h>

ConnectionProfiles::ConnectionProfiles(QWidget *parent)
: QDialog(parent)
{
	m_serverlist = new ServerList();

	QPushButton *update_button = new QPushButton("Update sites...");
	QPushButton *close_button = new QPushButton("Close");

	QHBoxLayout *hbox = new QHBoxLayout();
	hbox->addWidget(update_button);
	hbox->addStretch();
	hbox->addWidget(close_button);

	m_configwidget = new ConfigWidget(this, false);

	QVBoxLayout *vbox = new QVBoxLayout();
	vbox->addWidget(m_serverlist);
	vbox->addWidget(m_configwidget);
	vbox->addLayout(hbox);
	setLayout(vbox);

	connect(close_button, SIGNAL(clicked()), SLOT(accept()));
	connect(update_button, SIGNAL(clicked()), SLOT(slotUpdate()));

	connect(m_serverlist, SIGNAL(signalSelected(const GGZServer&)), SLOT(slotSelected(const GGZServer&)));
	connect(m_configwidget, SIGNAL(signalChanged(const GGZServer&)), SLOT(slotChanged(const GGZServer&)));

	setWindowTitle("Manage GGZ profiles");
	resize(400, 350);
	show();

	// FIXME: use kconfig
	m_metaserver = "ggzmeta://meta.ggzgamingzone.org:15689";
}

void ConnectionProfiles::addServer(const GGZServer& server)
{
	m_serverlist->addServer(server);
}

void ConnectionProfiles::slotUpdate()
{
	ServerSelector selector(this);
	selector.setMetaUri(m_metaserver);
	int status = selector.exec();
	if(status == QDialog::Accepted)
	{
		addServer(selector.server());
	}
}

void ConnectionProfiles::setMetaserver(const QString &metaserver)
{
	m_metaserver = metaserver;
}

void ConnectionProfiles::slotSelected(const GGZServer& server)
{
	m_configwidget->setGGZServer(server);
}

QList<GGZServer> ConnectionProfiles::profiles()
{
	QList<GGZServer> servers = m_serverlist->servers();
	QList<GGZServer> profiles;
	for(int i = 0; i < servers.size(); i++)
	{
		GGZServer server = servers.at(i);
		//if(!server.logintype())
		// FIXME: ...
		profiles.append(server);
	}
	return profiles;
}

void ConnectionProfiles::slotChanged(const GGZServer& server)
{
	m_serverlist->updateServer(server);
}
