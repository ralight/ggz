// Player list includes
#include "playerlist.h"
#include "qrecursivesortfilterproxymodel.h"
#include "player.h"

// Qt includes
#include <qtreeview.h>
#include <qstandarditemmodel.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qmenu.h>

static Qt::ItemFlags ROFLAGS =
	Qt::ItemIsSelectable |
	Qt::ItemIsDragEnabled |
	Qt::ItemIsDropEnabled |
	Qt::ItemIsUserCheckable |
	Qt::ItemIsEnabled;

PlayerList::PlayerList()
: QWidget()
{
	m_treeview = new QTreeView();
	m_treeview->setContextMenuPolicy(Qt::CustomContextMenu);

	QLineEdit *searchbox = new QLineEdit();
	QLabel *searchlabel = new QLabel("Search for:");

	QHBoxLayout *hbox = new QHBoxLayout();
	hbox->addWidget(searchlabel);
	hbox->addWidget(searchbox);

	QVBoxLayout *vbox = new QVBoxLayout();
	vbox->addWidget(m_treeview);
	vbox->addLayout(hbox);
	setLayout(vbox);

	m_model = new QStandardItemModel();
	m_model->setColumnCount(2);

	m_itemfriends = new QStandardItem();
	m_itemfriends->setText("Friends");
	m_itemfriends->setFlags(ROFLAGS);
	m_model->appendRow(m_itemfriends);

	m_itemignored = new QStandardItem();
	m_itemignored->setText("Ignored");
	m_itemignored->setFlags(ROFLAGS);
	m_model->appendRow(m_itemignored);

	m_itemothers = new QStandardItem();
	m_itemothers->setText("Others");
	m_itemothers->setFlags(ROFLAGS);
	m_model->appendRow(m_itemothers);

	m_model->setHeaderData(0, Qt::Horizontal, QString("Player"), Qt::DisplayRole);
	m_model->setHeaderData(1, Qt::Horizontal, QString("Properties"), Qt::DisplayRole);

	m_proxymodel = new QRecursiveSortFilterProxyModel(this);
	m_proxymodel->setSourceModel(m_model);
	m_proxymodel->setDynamicSortFilter(true);

	m_treeview->setModel(m_proxymodel);
	m_treeview->expandAll();

	connect(searchbox, SIGNAL(textChanged(const QString&)), SLOT(slotSearch(const QString&)));
	connect(m_treeview, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(slotSelected(const QPoint&)));

	setWindowTitle("GGZ gets a more flexible player list!");
	resize(320, 300);
	show();

	Player *player1 = new Player("someadmin");
	player1->setRole(Player::Admin);
	player1->setStatistics("0-1-5");
	player1->setLag(25);
	player1->setRelation(Player::Buddy);
	addPlayer(player1);

	Player *player2 = new Player("somehost");
	player2->setRole(Player::Host);
	player2->setStatistics("12-4-5");
	player2->setLag(69);
	addPlayer(player2);
}

void PlayerList::addPlayer(Player *player)
{
	m_players[player->name()] = player;

	QString pixmap = "guest.png";
	if(player->role() == Player::Admin)
		pixmap = "admin.png";
	else if(player->role() == Player::Host)
		pixmap = "host.png";
	else if(player->role() == Player::Registered)
		pixmap = "registered.png";
	else if(player->role() == Player::Bot)
		pixmap = "bot.png";

	QString lagpixmap = "lag0.png";
	if(player->lag() == 100)
		lagpixmap = "lag5.png";
	else if(player->lag() > 80)
		lagpixmap = "lag4.png";
	else if(player->lag() > 60)
		lagpixmap = "lag3.png";
	else if(player->lag() > 40)
		lagpixmap = "lag2.png";
	else if(player->lag() > 20)
		lagpixmap = "lag1.png";

	QStandardItem *itemname = new QStandardItem();
	itemname->setFlags(ROFLAGS);
	itemname->setIcon(QIcon("players/" + pixmap));
	itemname->setText(player->name());

	QStandardItem *itemlagstats = new QStandardItem();
	itemlagstats->setFlags(ROFLAGS);
	itemlagstats->setIcon(QIcon("players/" + lagpixmap));
	itemlagstats->setText(player->statistics());

	QStandardItem *item = m_itemothers;
	if(player->relation() == Player::Buddy)
		item = m_itemfriends;
	else if(player->relation() == Player::Ignored)
		item = m_itemignored;

	QList<QStandardItem*> childitems;
	childitems << itemname;
	childitems << itemlagstats;
	item->appendRow(childitems);
}

void PlayerList::slotSearch(const QString& text)
{
	m_proxymodel->setFilterRegExp(QRegExp(text, Qt::CaseInsensitive, QRegExp::FixedString));
	m_proxymodel->setFilterKeyColumn(0);
	m_treeview->expandAll();
}

void PlayerList::slotSelected(const QPoint& pos)
{
	QModelIndex index = m_treeview->indexAt(pos);
	if(!index.isValid())
		return;

	QString name = m_proxymodel->data(index).toString();
	if(m_players.contains(name))
	{
		Player *player = m_players[name];

		QMenu menu;
		if(player->relation() == Player::Buddy)
		{
			menu.addAction("Remove from buddy list");
		}
		else if(player->relation() == Player::Ignored)
		{
			menu.addAction("Remove from list of ignored players");
		}
		else
		{
			menu.addAction("Add to list of ignored players.");
			menu.addAction("Add to buddy list");
		}
		menu.addSeparator();
		menu.addAction("Chat...");
		menu.exec(mapToGlobal(pos));
	}
}
