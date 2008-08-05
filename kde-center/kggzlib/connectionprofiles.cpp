// Connection dialog includes
#include "connectionprofiles.h"

// KGGZ includes
#include "serverlist.h"
#include "configwidget.h"
#include "serverselector.h"

// Qt includes
#include <qlayout.h>
#include <qpushbutton.h>

ConnectionProfiles::ConnectionProfiles()
: QWidget()
{
	m_serverlist = new ServerList();

	QPushButton *update_button = new QPushButton("Update sites...");
	QPushButton *close_button = new QPushButton("Close");

	QHBoxLayout *hbox = new QHBoxLayout();
	hbox->addWidget(update_button);
	hbox->addStretch();
	hbox->addWidget(close_button);

	ConfigWidget *configwidget = new ConfigWidget(this, false);

	QVBoxLayout *vbox = new QVBoxLayout();
	vbox->addWidget(m_serverlist);
	vbox->addWidget(configwidget);
	vbox->addLayout(hbox);
	setLayout(vbox);

	connect(close_button, SIGNAL(clicked()), SLOT(close()));
	connect(update_button, SIGNAL(clicked()), SLOT(slotUpdate()));

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
		//setGGZServer(selector.server());
		// FIXME: add selector.server() to server list,
		// but not to profile list yet!
	}
}

void ConnectionProfiles::setMetaserver(const QString &metaserver)
{
	m_metaserver = metaserver;
}

