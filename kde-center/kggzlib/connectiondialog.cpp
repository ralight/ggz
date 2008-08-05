// Connection dialog includes
#include "connectiondialog.h"

// KGGZ includes
#include "serverlist.h"
#include "connectionprofiles.h"

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

	setWindowTitle("Connect to Gaming Zone");
	resize(320, 350);
	show();
}

void ConnectionDialog::addServer(const GGZServer& server)
{
	m_serverlist->addServer(server);
}

void ConnectionDialog::slotManage()
{
	ConnectionProfiles *prof = new ConnectionProfiles();
	// FIXME: ...
}

void ConnectionDialog::slotConnect()
{
	// FIXME: ...
}

void ConnectionDialog::slotSelected(const GGZServer& server)
{
	bool enabled = (!server.uri().isEmpty());
	m_connect_button->setEnabled(enabled);
}
