// Player list includes
#include "playerlist.h"
#include "qrecursivesortfilterproxymodel.h"

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
	m_treeview = new QTreeView();

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

	QStandardItem *item01 = new QStandardItem();
	item01->setText("Friends");
	m_model->appendRow(item01);

	QStandardItem *item02 = new QStandardItem();
	item02->setText("Others");
	m_model->appendRow(item02);

	QStandardItem *item11 = new QStandardItem();
	item11->setIcon(QIcon("players/admin.png"));
	item11->setText("someadmin");
	QStandardItem *item12 = new QStandardItem();
	item12->setIcon(QIcon("players/lag5.png"));
	item12->setText("0-1-5");
	/*QList<QStandardItem*> list1;
	list1 << item11;
	list1 << item12;
	m_model->appendRow(list1);*/

	QStandardItem *item21 = new QStandardItem();
	item21->setIcon(QIcon("players/host.png"));
	item21->setText("somehost");
	QStandardItem *item22 = new QStandardItem();
	item22->setIcon(QIcon("players/lag3.png"));
	item22->setText("12-4-5");
	/*QList<QStandardItem*> list2;
	list2 << item21;
	list2 << item22;
	m_model->appendRow(list2);*/

	item01->setChild(0, 0, item21);
	item01->setChild(0, 1, item22);

	item02->setChild(0, 0, item11);
	item02->setChild(0, 1, item12);

	m_model->setHeaderData(0, Qt::Horizontal, QString("Player"), Qt::DisplayRole);
	m_model->setHeaderData(1, Qt::Horizontal, QString("Properties"), Qt::DisplayRole);

	m_proxymodel = new QRecursiveSortFilterProxyModel(this);
	m_proxymodel->setSourceModel(m_model);

	m_treeview->setModel(m_proxymodel);
	m_treeview->expandAll();

	connect(searchbox, SIGNAL(textChanged(const QString&)), SLOT(slotSearch(const QString&)));

	setWindowTitle("GGZ gets a more flexible player list!");
	resize(320, 300);
	show();
}

void PlayerList::slotSearch(const QString& text)
{
	m_proxymodel->setFilterRegExp(QRegExp(text, Qt::CaseInsensitive, QRegExp::FixedString));
	m_proxymodel->setFilterKeyColumn(0);
	m_treeview->expandAll();
}
