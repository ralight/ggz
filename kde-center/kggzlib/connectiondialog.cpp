// Connection dialog includes
#include "connectiondialog.h"

// KGGZ includes
#include "serverlist.h"
#include "connectionprofiles.h"
#include "util.h"
#include "ggzprofile.h"
#include "kggzcorelayer.h"
#include "tableconfiguration.h"

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
: QDialog(parent), m_corelayer(NULL)
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

	setWindowTitle("Connect to Gaming Zone");
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
	m_corelayer->ggzcore(uri());
}

void ConnectionDialog::slotReady(bool ready)
{
	m_connect_button->setEnabled(true);
	m_indicator->setEnabled(false);
	m_indicator->setMaximum(1);

	if(ready)
	{
		TableConfiguration dlg(this);
		// FIXME: use real numbers from ggzcore
		dlg.initLauncher(m_username, 2, 1);
		if(dlg.exec() == QDialog::Accepted)
		{
			QList<KGGZCore::Player> seats;
			for(int i = 1; i < dlg.seats(); i++)
			{
				TableConfiguration::SeatTypes seattype = dlg.seatType(i);
				if(seattype == TableConfiguration::seatopen)
					seats << KGGZCore::Player(dlg.reservation(i), KGGZCore::Player::open);
				else if(seattype == TableConfiguration::seatbot)
					seats << KGGZCore::Player(dlg.reservation(i), KGGZCore::Player::bot);
				else if(seattype == TableConfiguration::seatplayer)
					seats << KGGZCore::Player(dlg.reservation(i), KGGZCore::Player::player);
			}
			m_corelayer->configureTable(seats);

			accept();
		}
		else
		{
			reject();
		}
	}
	else
	{
		KMessageBox::error(this,
			i18n("The connection cannot be established."),
			i18n("Connection failed"));
	}
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
	}
}

QString ConnectionDialog::uri()
{
	// FIXME: return fully-qualified GGZ URI
	return m_uri;
}

KGGZCoreLayer *ConnectionDialog::layer() const
{
	return m_corelayer;
}
