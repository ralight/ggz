// Table configuration dialog includes
#include "tabledialog.h"

// KGGZ includes
#include "tablelist.h"
#include "tableconfiguration.h"
#include "util.h"

#include <kggzcore/coreclient.h>
#include <kggzcore/player.h>
#include <kggzcore/gametype.h>

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

TableDialog::TableDialog(QWidget *parent)
: QDialog(parent)
{
	m_tablelist = new TableList();

	QPushButton *manage_button = new QPushButton("Manage tables...");
	m_use_button = new QPushButton("Play on this table");
	m_use_button->setEnabled(false);
	QPushButton *cancel_button = new QPushButton("Cancel");

	QHBoxLayout *hbox = new QHBoxLayout();
	hbox->addWidget(manage_button);
	hbox->addStretch();
	hbox->addWidget(m_use_button);
	hbox->addWidget(cancel_button);

	QVBoxLayout *vbox = new QVBoxLayout();
	vbox->addWidget(m_tablelist);
	vbox->addLayout(hbox);
	setLayout(vbox);

	connect(cancel_button, SIGNAL(clicked()), SLOT(reject()));
	connect(m_use_button, SIGNAL(clicked()), SLOT(slotUse()));
	connect(manage_button, SIGNAL(clicked()), SLOT(slotManage()));

	//connect(m_serverlist, SIGNAL(signalSelected(const GGZProfile&, int)), SLOT(slotSelected(const GGZProfile&, int)));

	load();

	setWindowTitle("Table configuration");
	resize(400, 350);
	show();
}

void TableDialog::load()
{
	m_tablelist->clear();

	KSharedConfig::Ptr conf = KGlobal::config();
	for(int i = 0; true; i++)
	{
		KConfigGroup cg = KConfigGroup(conf, "Table" + QString::number(i));
		//if(!cg.isValid())
		if(cg.keyList().size() == 0)
			break;

		//Util util;
		//GGZProfile profile = util.loadprofile(cg);
		//if(profile.configured())
		//	addProfile(profile);
	}
}

void TableDialog::slotManage()
{
	TableConfiguration tableconf(this);
	tableconf.initLauncher(m_identity, m_gametype.maxPlayers(), m_gametype.maxBots());
	QMap<QString, QString> namedbots = m_gametype.namedBots();
	QMap<QString, QString>::const_iterator it = namedbots.begin();
	while(it != namedbots.end())
	{
		tableconf.addBot(it.key(), it.value());
		it++;
	}
	tableconf.exec();

	load();
}

void TableDialog::slotUse()
{
	m_use_button->setEnabled(false);
	accept();
}

/*void TableDialog::slotSelected(const GGZProfile& profile, int pos)
{
	Q_UNUSED(pos);

	bool enabled = (!profile.ggzServer().uri().isEmpty());
	m_connect_button->setEnabled(enabled);
	if(enabled)
	{
		m_uri = profile.ggzServer().uri();
		m_username = profile.username();
	}
}*/

void TableDialog::setGameType(const KGGZCore::GameType& gametype)
{
	m_gametype = gametype;
}

void TableDialog::setIdentity(QString identity)
{
	m_identity = identity;
}

QList<KGGZCore::Player> TableDialog::seats()
{
	QList<KGGZCore::Player> m_seats;
	// FIXME: !!!
	return m_seats;
}
