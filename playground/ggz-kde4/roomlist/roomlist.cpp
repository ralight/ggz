// Player list includes
#include "roomlist.h"
#include "room.h"
#include "roomdelegate.h"
#include "qrecursivesortfilterproxymodel.h"

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

RoomList::RoomList()
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

	m_itemgame = new QStandardItem();
	m_itemgame->setText("Gaming");
	m_itemgame->setFlags(ROFLAGS);
	m_model->appendRow(m_itemgame);

	m_itemchat = new QStandardItem();
	m_itemchat->setText("Chat only");
	m_itemchat->setFlags(ROFLAGS);
	m_model->appendRow(m_itemchat);

	m_model->setHeaderData(0, Qt::Horizontal, QString("Room"), Qt::DisplayRole);
	m_model->setHeaderData(1, Qt::Horizontal, QString("Players"), Qt::DisplayRole);

	m_proxymodel = new QRecursiveSortFilterProxyModel(this);
	m_proxymodel->setSourceModel(m_model);
	m_proxymodel->setDynamicSortFilter(true);

	RoomDelegate *delegate = new RoomDelegate(this);

	m_treeview->setModel(m_proxymodel);
	m_treeview->setItemDelegate(delegate);
	m_treeview->expandAll();

	connect(searchbox, SIGNAL(textChanged(const QString&)), SLOT(slotSearch(const QString&)));
	connect(m_treeview, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(slotSelected(const QPoint&)));

	setWindowTitle("GGZ gets a more flexible room list!");
	resize(320, 300);
	show();

	Room *room1 = new Room("Tic-Tac-Toe");
	room1->setLogo("tictactoe.png");
	addRoom(room1);

	Room *room2 = new Room("Chess");
	room2->setLogo("chess.png");
	room2->setModule(true);
	addRoom(room2);

	Room *room3 = new Room("Connect The Dots");
	room3->setLogo("dots.png");
	room3->setAccess(Room::Locked);
	addRoom(room3);
}

void RoomList::addRoom(Room *room)
{
	m_rooms[room->name()] = room;

	QStandardItem *itemname = new QStandardItem();
	itemname->setFlags(ROFLAGS);
	itemname->setIcon(QIcon("games/" + room->logo()));
	itemname->setText(room->name());

	QStandardItem *itemcount = new QStandardItem();
	itemcount->setFlags(ROFLAGS);
	itemcount->setText("99");

	QStandardItem *item = m_itemgame;
	if(!room->module())
		item = m_itemchat;

	QList<QStandardItem*> childitems;
	childitems << itemname;
	childitems << itemcount;
	item->appendRow(childitems);
}

void RoomList::slotSearch(const QString& text)
{
	m_proxymodel->setFilterRegExp(QRegExp(text, Qt::CaseInsensitive, QRegExp::FixedString));
	m_proxymodel->setFilterKeyColumn(0);
	m_treeview->expandAll();
}

void RoomList::slotSelected(const QPoint& pos)
{
	QModelIndex index = m_treeview->indexAt(pos);
	if(!index.isValid())
		return;

	QString name = m_proxymodel->data(index).toString();
	if(m_rooms.contains(name))
	{
		Room *room = m_rooms[name];
		m_action_name = name;

		QMenu menu;

		QAction *action_favourites;
		if(room->favourite())
			action_favourites = menu.addAction("Remove from favourites");
		else
			action_favourites = menu.addAction("Add to favourites");
		connect(action_favourites, SIGNAL(triggered()), SLOT(slotFavourites()));

		menu.exec(mapToGlobal(pos));
	}
}

void RoomList::slotFavourites()
{
	if(m_rooms.contains(m_action_name))
	{
		Room *room = m_rooms[m_action_name];
		room->setFavourite(!room->favourite());
	}
}
