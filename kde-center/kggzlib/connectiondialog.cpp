// Connection dialog includes
#include "connectiondialog.h"

// KGGZ includes
#include "serverlist.h"
#include "connectionprofiles.h"
#include "util.h"
#include "ggzprofile.h"

#include <kggzcore/coreclient.h>

// KDE includes
#include <kconfig.h>
#include <kconfiggroup.h>
#include <ksharedconfig.h>
#include <kglobal.h>

// Qt includes
#include <qlayout.h>
#include <qpushbutton.h>

ConnectionDialog::ConnectionDialog(QWidget *parent)
: QDialog(parent)
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

	connect(cancel_button, SIGNAL(clicked()), SLOT(reject()));
	connect(m_connect_button, SIGNAL(clicked()), SLOT(slotConnect()));
	connect(manage_button, SIGNAL(clicked()), SLOT(slotManage()));

	connect(m_serverlist, SIGNAL(signalSelected(const GGZProfile&, int)), SLOT(slotSelected(const GGZProfile&, int)));

	load();

	setWindowTitle("Connect to Gaming Zone");
	resize(320, 350);
	show();
}

void ConnectionDialog::load()
{
	m_serverlist->clear();

	KSharedConfig::Ptr conf = KGlobal::config();
	for(int i = 0; true; i++)
	{
		KConfigGroup cg = KConfigGroup(conf, "Profile" + QString::number(i));
		//if(!cg.isValid())
		if(cg.keyList().size() == 0)
			break;

		Util util;
		GGZProfile profile = util.loadprofile(cg);
		if(profile.configured())
			addProfile(profile);
	}
}

void ConnectionDialog::addProfile(const GGZProfile& profile)
{
	m_serverlist->addProfile(profile);
}

void ConnectionDialog::slotManage()
{
	ConnectionProfiles prof(this);
	prof.exec();

	load();
}

void ConnectionDialog::slotConnect()
{
	m_connect_button->setEnabled(false);

	KGGZCore::CoreClient *coreclient = new KGGZCore::CoreClient(this, true);
	coreclient->setUrl(m_uri);
	coreclient->initiateLogin();
	// FIXME: see if it works and then call accept() ...
}

void ConnectionDialog::slotSelected(const GGZProfile& profile, int pos)
{
	Q_UNUSED(pos);

qDebug("** %s", qPrintable(profile.ggzServer().uri()));
	bool enabled = (!profile.ggzServer().uri().isEmpty());
	m_connect_button->setEnabled(enabled);
	if(enabled)
		m_uri = profile.ggzServer().uri();
}
