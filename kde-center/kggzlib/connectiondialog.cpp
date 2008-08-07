// Connection dialog includes
#include "connectiondialog.h"

// KGGZ includes
#include "serverlist.h"
#include "connectionprofiles.h"

#include <kggzcore/coreclient.h>

#include <kconfig.h>
#include <kconfiggroup.h>
#include <ksharedconfig.h>
#include <kglobal.h>

// Qt includes
#include <qlayout.h>
#include <qpushbutton.h>

ConnectionDialog::ConnectionDialog()
: QWidget()
{
	m_serverlist = new ServerList();

	QPushButton *manage_button = new QPushButton("Manage profiles...");
	m_connect_button = new QPushButton("Connect");
	m_connect_button->setEnabled(false);
	QPushButton *cancel_button = new QPushButton("Cancel");

	QHBoxLayout *hbox = new QHBoxLayout();
	hbox->addWidget(manage_button);
	hbox->addStretch();
	hbox->addWidget(m_connect_button);
	hbox->addWidget(cancel_button);

	QVBoxLayout *vbox = new QVBoxLayout();
	vbox->addWidget(m_serverlist);
	vbox->addLayout(hbox);
	setLayout(vbox);

	connect(cancel_button, SIGNAL(clicked()), SLOT(close()));
	connect(m_connect_button, SIGNAL(clicked()), SLOT(slotConnect()));
	connect(manage_button, SIGNAL(clicked()), SLOT(slotManage()));

	connect(m_serverlist, SIGNAL(signalSelected(const GGZServer&)), SLOT(slotSelected(const GGZServer&)));

	load();

	setWindowTitle("Connect to Gaming Zone");
	resize(320, 350);
	show();
}

void ConnectionDialog::load()
{
	KSharedConfig::Ptr conf = KGlobal::config();
	for(int i = 0; true; i++)
	{
		KConfigGroup cg = KConfigGroup(conf, "Profile" + QString::number(i));
		//if(!cg.isValid())
		if(cg.keyList().size() == 0)
			break;

		GGZServer server;
		server.setUri(cg.readEntry("Uri"));
		server.setApi(cg.readEntry("Api"));
		server.setName(cg.readEntry("Name"));
		server.setIcon(cg.readEntry("Icon"));
		addServer(server);
	}
}

void ConnectionDialog::addServer(const GGZServer& server)
{
	m_serverlist->addServer(server);
}

void ConnectionDialog::slotManage()
{
	ConnectionProfiles prof(this);
	prof.exec();

	m_serverlist->clear();
	QList<GGZServer> profiles = prof.profiles();
	KSharedConfig::Ptr conf = KGlobal::config();
	for(int i = 0; i < profiles.size(); i++)
	{
		GGZServer server = profiles.at(i);
		addServer(server);

		KConfigGroup cg = KConfigGroup(conf, "Profile" + QString::number(i));
		cg.writeEntry("Uri", server.uri());
		cg.writeEntry("Api", server.api());
		cg.writeEntry("Name", server.name());
		cg.writeEntry("Icon", server.icon());
		conf->sync();
	}
}

void ConnectionDialog::slotConnect()
{
	m_connect_button->setEnabled(false);

	KGGZCore::CoreClient *coreclient = new KGGZCore::CoreClient(this, true);
	coreclient->setUrl(m_uri);
	coreclient->initiateLogin();
	// FIXME: ...
}

void ConnectionDialog::slotSelected(const GGZServer& server)
{
	bool enabled = (!server.uri().isEmpty());
	m_connect_button->setEnabled(enabled);
	if(enabled)
		m_uri = server.uri();
}
