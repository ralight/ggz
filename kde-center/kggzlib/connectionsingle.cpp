// Connection dialog includes
#include "connectionsingle.h"

#include "util.h"

// KGGZ includes
#include "serverlist.h"
#include "configwidget.h"
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

ConnectionSingle::ConnectionSingle(QWidget *parent)
: QDialog(parent)
{
	m_serverlist = new ServerList();

	QPushButton *connect_button = new QPushButton(i18n("Connect"));
	QPushButton *cancel_button = new QPushButton(i18n("Cancel"));

	QHBoxLayout *hbox = new QHBoxLayout();
	hbox->addStretch();
	hbox->addWidget(connect_button);
	hbox->addWidget(cancel_button);

	m_configwidget = new ConfigWidget(this, false);

	QVBoxLayout *vbox = new QVBoxLayout();
	vbox->addWidget(m_serverlist);
	vbox->addWidget(m_configwidget);
	vbox->addLayout(hbox);
	setLayout(vbox);

	connect(connect_button, SIGNAL(clicked()), SLOT(slotAccept()));
	connect(cancel_button, SIGNAL(clicked()), SLOT(reject()));

	connect(m_configwidget, SIGNAL(signalChanged(const GGZProfile&)), SLOT(slotChanged(const GGZProfile&)));

	setWindowTitle(i18n("Establish new GGZ profile"));
	resize(400, 550);
	show();
}

// FIXME: Duplicate in ConnectionProfiles; move to Util?
void ConnectionSingle::presetServer(const QString& uri)
{
	GGZProfile profile = Util::uriToProfile(uri);
	addProfile(profile, true);

	m_configwidget->setGGZProfile(profile);
}

void ConnectionSingle::addProfile(const GGZProfile& profile, bool selected)
{
	m_serverlist->addProfile(profile);
	if(selected)
		m_serverlist->selectProfile(profile);
}

GGZProfile ConnectionSingle::profile()
{
	QList<GGZProfile> profiles = m_serverlist->profiles();
	return profiles.at(0);
}

void ConnectionSingle::slotChanged(const GGZProfile& profile)
{
	m_serverlist->updateProfile(profile, 0);
}

void ConnectionSingle::slotAccept()
{
	m_serverlist->updateProfile(m_configwidget->ggzProfile(), 0);

	accept();
}
