// Connection dialog includes
#include "connectiondialog.h"

// KGGZ includes
#include "serverlist.h"
#include "connectionprofiles.h"
#include "util.h"
#include "ggzprofile.h"
#include "kggzcorelayer.h"
#include "tabledialog.h"
#include "embeddedcoreclient.h"

#include <kggzcore/coreclient.h>
#include <kggzcore/player.h>

// KDE includes
#include <kconfig.h>
#include <kconfiggroup.h>
#include <ksharedconfig.h>
#include <kglobal.h>
#include <klocale.h>
#include <kmessagebox.h>

// Qt includes
#include <qlayout.h>
#include <qpushbutton.h>
#include <qprogressbar.h>

ConnectionDialog::ConnectionDialog(QWidget *parent)
: QDialog(parent), m_corelayer(NULL), m_tabledlg(NULL), m_coremode(false)
{
	m_serverlist = new ServerList();

	QPushButton *manage_button = new QPushButton(i18n("Manage profiles..."));
	m_connect_button = new QPushButton(i18n("Connect"));
	m_connect_button->setEnabled(false);
	QPushButton *cancel_button = new QPushButton(i18n("Cancel"));

	QHBoxLayout *hbox = new QHBoxLayout();
	hbox->addWidget(manage_button);
	hbox->addStretch();
	hbox->addWidget(m_connect_button);
	hbox->addWidget(cancel_button);

	m_indicator = new QProgressBar();
	m_indicator->setEnabled(false);
	m_indicator->setMinimum(0);
	m_indicator->setMaximum(1);

	QVBoxLayout *vbox = new QVBoxLayout();
	vbox->addWidget(m_serverlist);
	vbox->addWidget(m_indicator);
	vbox->addLayout(hbox);
	setLayout(vbox);

	connect(cancel_button, SIGNAL(clicked()), SLOT(reject()));
	connect(m_connect_button, SIGNAL(clicked()), SLOT(slotConnect()));
	connect(manage_button, SIGNAL(clicked()), SLOT(slotManage()));

	connect(m_serverlist, SIGNAL(signalSelected(const GGZProfile&, int)), SLOT(slotSelected(const GGZProfile&, int)));

	load();

	setWindowTitle(i18n("Connect to Gaming Zone"));
	resize(400, 350);
	show();
}

void ConnectionDialog::setGame(QString engine, QString version)
{
	m_engine = engine;
	m_version = version;
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
	m_indicator->setEnabled(true);
	m_indicator->setMaximum(0);

	// FIXME: core layer doesn't have parent and will leak happily
	m_corelayer = new KGGZCoreLayer(NULL, m_engine, m_version);
	connect(m_corelayer, SIGNAL(signalReady(bool)), SLOT(slotReady(bool)));
	connect(m_corelayer, SIGNAL(signalRoomReady(bool)), SLOT(slotRoomReady(bool)));
	m_corelayer->ggzcore(uri(), !m_coremode);
}

void ConnectionDialog::slotReady(bool ready)
{
	m_connect_button->setEnabled(true);
	m_indicator->setEnabled(false);
	m_indicator->setMaximum(1);

	if(ready)
	{
		// FIXME: support for join/spectate in addition to launch goes here...
		// FIXME: right now, this leads to a crash when closing the connection dialogue
		//EmbeddedCoreClient *ecc = new EmbeddedCoreClient();

		m_tabledlg = new TableDialog(this);
		// FIXME: use real gametype from ggzcore
		KGGZCore::GameType gametype;
		gametype.setMaxPlayers(2);
		gametype.setMaxBots(1);
		m_tabledlg->setGameType(gametype);
		m_tabledlg->setIdentity(m_username);
		m_tabledlg->setModal(true);
		m_tabledlg->show();

		connect(m_tabledlg, SIGNAL(finished(int)), SLOT(slotTable(int)));
	}
	else
	{
		KMessageBox::error(this,
			i18n("The connection cannot be established."),
			i18n("Connection failed"));
	}
}

void ConnectionDialog::slotTable(int result)
{
	if(result == QDialog::Accepted)
		m_corelayer->configureTable(m_tabledlg->table().players());

	delete m_tabledlg;
	m_tabledlg = NULL;

	done(result);
}

void ConnectionDialog::slotSelected(const GGZProfile& profile, int pos)
{
	Q_UNUSED(pos);

	bool enabled = (!profile.ggzServer().uri().isEmpty());
	m_connect_button->setEnabled(enabled);
	if(enabled)
	{
		m_uri = profile.ggzServer().uri();
		m_username = profile.username();

		// FIXME: This cannot capture first-time vs. anonymous
		QUrl qurl(m_uri);
		qurl.setUserName(profile.username());
		qurl.setPassword(profile.password());
		m_uri = qurl.toString();
	}
}

QString ConnectionDialog::uri()
{
	return m_uri;
}

KGGZCoreLayer *ConnectionDialog::layer() const
{
	return m_corelayer;
}

void ConnectionDialog::setCoreMode(bool coremode)
{
	m_coremode = coremode;
}

void ConnectionDialog::slotRoomReady(bool ready)
{
	if((ready) && (!m_coremode))
		m_corelayer->launch();

	if(m_coremode)
	{
		accept();
	}
}

KGGZCore::CoreClient *ConnectionDialog::core()
{
	if(!m_corelayer)
		return NULL;

	return m_corelayer->core();
}

