// Player list includes
#include "playerlist.h"

// Qt includes
#include <qtreeview.h>
#include <qstandarditemmodel.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlabel.h>

PlayerList::PlayerList()
: QWidget()
{
	QTreeView *treeview = new QTreeView();

	QLineEdit *searchbox = new QLineEdit();
	QLabel *searchlabel = new QLabel("Search for:");

	QHBoxLayout *hbox = new QHBoxLayout();
	hbox->addWidget(searchlabel);
	hbox->addWidget(searchbox);

	QVBoxLayout *vbox = new QVBoxLayout();
	vbox->addWidget(treeview);
	vbox->addLayout(hbox);
	setLayout(vbox);

	QStandardItemModel *model = new QStandardItemModel();
	model->setColumnCount(2);

	QStandardItem *item01 = new QStandardItem();
	item01->setText("Friends");
	model->appendRow(item01);

	QStandardItem *item02 = new QStandardItem();
	item02->setText("Others");
	model->appendRow(item02);

	QStandardItem *item11 = new QStandardItem();
	item11->setIcon(QIcon("players/admin.png"));
	item11->setText("someadmin");
	QStandardItem *item12 = new QStandardItem();
	item12->setIcon(QIcon("players/lag5.png"));
	item12->setText("0-1-5");
	/*QList<QStandardItem*> list1;
	list1 << item11;
	list1 << item12;
	model->appendRow(list1);*/

	QStandardItem *item21 = new QStandardItem();
	item21->setIcon(QIcon("players/host.png"));
	item21->setText("somehost");
	QStandardItem *item22 = new QStandardItem();
	item22->setIcon(QIcon("players/lag3.png"));
	item22->setText("12-4-5");
	/*QList<QStandardItem*> list2;
	list2 << item21;
	list2 << item22;
	model->appendRow(list2);*/

	item01->setChild(0, 0, item21);
	item01->setChild(0, 1, item22);

	item02->setChild(0, 0, item11);
	item02->setChild(0, 1, item12);

	model->setHeaderData(0, Qt::Horizontal, QString("Player"), Qt::DisplayRole);
	model->setHeaderData(1, Qt::Horizontal, QString("Properties"), Qt::DisplayRole);

	treeview->setModel(model);

	setWindowTitle("GGZ gets a more flexible player list!");
	resize(320, 300);
	show();
}

