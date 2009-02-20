// Player list includes
#include "roomlist.h"
#include "room.h"
#include "roomdelegate.h"
#include "roomtreeview.h"
#include "qrecursivesortfilterproxymodel.h"
#include "modelview.h"

// KDE includes
#include <kstandarddirs.h>

// Qt includes
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
	m_treeview = new RoomTreeView();
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
	m_model->setHeaderData(1, Qt::Horizontal, QString("Information"), Qt::DisplayRole);

	m_proxymodel = new QRecursiveSortFilterProxyModel(this);
	m_proxymodel->setSourceModel(m_model);
	m_proxymodel->setDynamicSortFilter(true);

	RoomDelegate *delegate = new RoomDelegate(m_treeview);

	m_treeview->setModel(m_proxymodel);
	m_treeview->setItemDelegate(delegate);
	m_treeview->expandAll();
	m_treeview->resizeColumnToContents(0);
	m_treeview->resizeColumnToContents(1);

	connect(searchbox, SIGNAL(textChanged(const QString&)), SLOT(slotSearch(const QString&)));
	connect(m_treeview, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(slotSelected(const QPoint&)));
	connect(m_treeview, SIGNAL(signalToolTip(QPoint)), delegate, SLOT(slotToolTip(QPoint)));
	connect(m_treeview, SIGNAL(doubleClicked(const QModelIndex&)), SLOT(slotSelectedLeft(const QModelIndex&)));

	setWindowTitle("GGZ gets a more flexible room list!");
	resize(500, 400);
	//show();
}

void RoomList::addRoom(Room *room)
{
	KStandardDirs d;

	m_rooms[room->name()] = room;

	QStandardItem *itemname = new QStandardItem();
	itemname->setFlags(ROFLAGS);
	itemname->setIcon(QIcon(d.findResource("data", "kggzlib/games/" + room->logo())));
	itemname->setText(room->name());
	itemname->setData(QVariant::fromValue((void*)room), ROOM_ROLE);

	QStandardItem *itemcount = new QStandardItem();
	itemcount->setFlags(ROFLAGS);
	itemcount->setText(room->description());
	itemcount->setData(QVariant::fromValue((void*)room), ROOM_ROLE);
	// FIXME: why is ROOM_ROLE necessary? apparently Qt::UserRole+1 isn't used
	// either here or in data() in RoomDelegate

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

	void *roomptr = index.model()->data(index, ROOM_ROLE).value<void*>();
	Room *room = static_cast<Room*>(roomptr);

	QString name = room->name();
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

		emit signalFavourite(room->name(), room->favourite());
	}
}

void RoomList::slotSelectedLeft(const QModelIndex& index)
{
	if(!index.isValid())
		return;

	QString name = m_proxymodel->data(index).toString();
	if(m_rooms.contains(name))
	{
		emit signalSelected(name);
	}
}
