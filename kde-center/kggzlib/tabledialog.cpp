// Table configuration dialog includes
#include "tabledialog.h"

// KGGZ includes
#include "tablelist.h"
#include "tableconfiguration.h"
#include "util.h"

#include <kggzcore/coreclient.h>
#include <kggzcore/player.h>
#include <kggzcore/table.h>
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

	connect(m_tablelist, SIGNAL(signalSelected(const KGGZCore::Table&, int)), SLOT(slotSelected(const KGGZCore::Table&, int)));

	load();

	setWindowTitle("Table configuration");
	resize(400, 350);
	show();
}

void TableDialog::load()
{
	m_tablelist->clear();
	m_mods.clear();

	KSharedConfig::Ptr conf = KSharedConfig::openConfig("kggzlib.rc"); //KGlobal::config();
	for(int i = 0; true; i++)
	{
		KConfigGroup cg = KConfigGroup(conf, "Table" + QString::number(i));
		//if(!cg.isValid())
		if(cg.keyList().size() == 0)
			break;

		KGGZCore::Table table(cg.readEntry("description"));
		int open = cg.readEntry("open").toInt();
		int bots = cg.readEntry("bots").toInt();
		int reserved = cg.readEntry("reserved").toInt();
		for(int j = 0; j < reserved; j++)
		{
			KGGZCore::Player p(QString(), KGGZCore::Player::reserved);
			table.addPlayer(p);
		}
		for(int j = 0; j < open; j++)
		{
			KGGZCore::Player p(QString(), KGGZCore::Player::open);
			table.addPlayer(p);
		}
		for(int j = 0; j < bots; j++)
		{
			KGGZCore::Player p(QString(), KGGZCore::Player::bot);
			table.addPlayer(p);
		}
		m_tablelist->addConfiguration(table);

		m_mods.append(cg.readEntry("modifiable", true));
	}
}

void TableDialog::slotManage()
{
	TableConfiguration dlg(this);
	dlg.initLauncher(m_identity, m_gametype.maxPlayers(), m_gametype.maxBots());
	QMap<QString, QString> namedbots = m_gametype.namedBots();
	QMap<QString, QString>::const_iterator it = namedbots.begin();
	while(it != namedbots.end())
	{
		dlg.addBot(it.key(), it.value());
		it++;
	}
	if(dlg.exec() == QDialog::Accepted)
	{
		KGGZCore::Table table(dlg.description());

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
		seats.prepend(KGGZCore::Player(m_identity, KGGZCore::Player::player));
		// FIXME: replace prepend() with correct seats[0]

		table.setSeats(seats);
		m_tablelist->addConfiguration(table);
		//load();
	}
}

void TableDialog::slotUse()
{
	m_use_button->setEnabled(false);
	accept();
}

void TableDialog::slotSelected(const KGGZCore::Table& table, int pos)
{
	Q_UNUSED(pos);

	m_table = table;

	bool enabled = (table.players().size() > 0);
	m_use_button->setEnabled(enabled);
}

void TableDialog::setGameType(const KGGZCore::GameType& gametype)
{
	m_gametype = gametype;
}

void TableDialog::setIdentity(QString identity)
{
	m_identity = identity;
}

KGGZCore::Table TableDialog::table() const
{
	return m_table;
}

