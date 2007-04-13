// Player list includes
#include "playerlist.h"

// Qt includes
#include <qtreeview.h>
#include <qstandarditemmodel.h>
#include <qheaderview.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qpushbutton.h>

PlayerList::PlayerList()
: QWidget()
{
	QTreeView *treeview = new QTreeView();

	QVBoxLayout *vbox = new QVBoxLayout();
	vbox->addWidget(treeview);
	setLayout(vbox);

	QStandardItemModel *model = new QStandardItemModel();
	model->setColumnCount(2);

	QStandardItem *item11 = new QStandardItem();
	item11->setText("foo");
	QStandardItem *item12 = new QStandardItem();
	item12->setText("foo2");
	QList<QStandardItem*> list1;
	list1 << item11;
	list1 << item12;
	model->appendRow(list1);

	QStandardItem *item21 = new QStandardItem();
	item21->setText("bar");
	QStandardItem *item22 = new QStandardItem();
	item22->setText("bar2");
	QList<QStandardItem*> list2;
	list2 << item21;
	list2 << item22;
	model->appendRow(list2);

	//QHeaderView *header = new QHeaderView();
	model->setHeaderData(0, Qt::Horizontal, QString("arg"), Qt::DisplayRole);
	model->setHeaderData(1, Qt::Horizontal, QString("haha"), Qt::DisplayRole);

	treeview->setModel(model);
	//treeview->setEditTriggers(QAbstractItemView::NoEditTriggers);
	//treeview->setHeader(header);

	setWindowTitle("GGZ gets a more flexible player list!");
	resize(320, 300);
	show();
}

