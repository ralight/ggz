// Connection dialog includes
#include "connectionprofiles.h"

#include "util.h"

// KGGZ includes
#include "serverlist.h"
#include "configwidget.h"
#include "serverselector.h"
#include "ggzprofile.h"

// KDE includes
#include <kconfig.h>
#include <kconfiggroup.h>
#include <ksharedconfig.h>
#include <kglobal.h>
#include <kinputdialog.h>
#include <klocale.h>

// Qt includes
#include <qlayout.h>
#include <qpushbutton.h>

ConnectionProfiles::ConnectionProfiles(QWidget *parent)
: QDialog(parent), m_pos(-1)
{
	m_serverlist = new ServerList();

	QPushButton *add_button = new QPushButton("Add");
	m_remove_button = new QPushButton("Remove");
	QPushButton *update_button = new QPushButton("Update sites...");
	QPushButton *close_button = new QPushButton("Close");
	m_remove_button->setEnabled(false);

	QHBoxLayout *hbox = new QHBoxLayout();
	hbox->addWidget(add_button);
	hbox->addWidget(m_remove_button);
	hbox->addWidget(update_button);
	hbox->addStretch();
	hbox->addWidget(close_button);

	m_configwidget = new ConfigWidget(this, false);

	QVBoxLayout *vbox = new QVBoxLayout();
	vbox->addWidget(m_serverlist);
	vbox->addWidget(m_configwidget);
	vbox->addLayout(hbox);
	setLayout(vbox);

	connect(add_button, SIGNAL(clicked()), SLOT(slotAdd()));
	connect(m_remove_button, SIGNAL(clicked()), SLOT(slotRemove()));
	connect(close_button, SIGNAL(clicked()), SLOT(slotAccept()));
	connect(update_button, SIGNAL(clicked()), SLOT(slotUpdate()));

	connect(m_serverlist, SIGNAL(signalSelected(const GGZProfile&, int)), SLOT(slotSelected(const GGZProfile&, int)));
	connect(m_configwidget, SIGNAL(signalChanged(const GGZProfile&)), SLOT(slotChanged(const GGZProfile&)));

	load();

	setWindowTitle("Manage GGZ profiles");
	resize(400, 550);
	show();
}

void ConnectionProfiles::load()
{
	KSharedConfig::Ptr conf = KGlobal::config();
	for(int i = 0; true; i++)
	{
		KConfigGroup cg = KConfigGroup(conf, "Profile" + QString::number(i));
		//if(!cg.isValid())
		if(cg.keyList().size() == 0)
			break;

		Util util;
		addProfile(util.loadprofile(cg));
	}

	KConfigGroup cg = KConfigGroup(conf, "Settings");
	m_metaserver = cg.readEntry("Metaserver", "ggzmeta://meta.ggzgamingzone.org:15689");
}

void ConnectionProfiles::addProfile(const GGZProfile& profile)
{
	m_serverlist->addProfile(profile);
}

void ConnectionProfiles::slotAdd()
{
	QString uri = KInputDialog::getText(i18n("Profile addition"), i18n("GGZ server location:"));

	if(!uri.isNull())
	{
		GGZProfile profile;
		GGZServer server;
		server.setUri(uri);
		profile.setGGZServer(server);
		addProfile(profile);
	}
}

void ConnectionProfiles::slotRemove()
{
	m_serverlist->removeProfile(m_pos);
}

void ConnectionProfiles::slotUpdate()
{
	ServerSelector selector(this);
	selector.setMetaUri(m_metaserver);
	int status = selector.exec();
	if(status == QDialog::Accepted)
	{
		GGZProfile profile;
		profile.setGGZServer(selector.server());
		addProfile(profile);

		save(profile);
	}
}

void ConnectionProfiles::setMetaserver(const QString &metaserver)
{
	m_metaserver = metaserver;
}

void ConnectionProfiles::slotSelected(const GGZProfile& profile, int pos)
{
	m_serverlist->updateProfile(m_configwidget->ggzProfile(), m_pos);

	m_pos = pos;
	save(profile);

	m_configwidget->setGGZProfile(profile);

	m_remove_button->setEnabled((pos != -1));
}

QList<GGZProfile> ConnectionProfiles::profiles()
{
	QList<GGZProfile> profiles = m_serverlist->profiles();
	QList<GGZProfile> configuredprofiles;
	for(int i = 0; i < profiles.size(); i++)
	{
		GGZProfile profile = profiles.at(i);
		if(profile.configured())
			configuredprofiles.append(profile);
	}
	return configuredprofiles;
}

void ConnectionProfiles::slotChanged(const GGZProfile& profile)
{
	m_serverlist->updateProfile(profile, m_pos);

	save(profile);
}

void ConnectionProfiles::save(const GGZProfile& profile)
{
	// FIXME: would be better to just save this profile
	// but would need unique identifier
	Q_UNUSED(profile);

	KSharedConfig::Ptr conf = KGlobal::config();
	QList<GGZProfile> profiles = m_serverlist->profiles();
	for(int i = 0; i < m_serverlist->profiles().size(); i++)
	{
		GGZProfile profile = profiles.at(i);

		KConfigGroup cg = KConfigGroup(conf, "Profile" + QString::number(i));
		Util util;
		util.saveprofile(profile, cg, conf);
	}

	int max = m_serverlist->profiles().size();
	conf->deleteGroup("Profile" + QString::number(max));
	conf->sync();
}

void ConnectionProfiles::slotAccept()
{
	m_serverlist->updateProfile(m_configwidget->ggzProfile(), m_pos);
	if((m_pos != -1) && (m_pos < m_serverlist->profiles().size()))
		save(m_serverlist->profiles().at(m_pos));

	accept();
}
