// Table configuration dialog includes
#include "tabledialog.h"

// KGGZ includes
#include "tablelist.h"
#include "tableconfiguration.h"
#include "util.h"

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

QList<KGGZCore::Player> TableDialog::seats()
{
	QList<KGGZCore::Player> m_seats;
	// FIXME: !!!
	return m_seats;
}
