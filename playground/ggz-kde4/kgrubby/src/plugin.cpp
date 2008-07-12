#include "plugin.h"

//#include <k3listview.h>
#include <klocale.h>

#include <qlayout.h>
#include <qpushbutton.h>
#include <qtablewidget.h>
#include <qheaderview.h>

Plugin::Plugin(QWidget *parent)
: KDialog(parent)
{
	QWidget *frame;
//	QHBoxLayout *hbox;
	QVBoxLayout *vbox;
//	QPushButton *add, *modify, *remove;

	setCaption(i18n("Plugin configuration"));
	setButtons(KDialog::Ok | KDialog::Cancel);

	frame = new QWidget(this);
	setMainWidget(frame);

	//m_settings = new K3ListView(frame);
	m_settings = new QTableWidget(frame);
	m_settings->setRowCount(1);

//	add = new QPushButton(i18n("Add"), frame);
//	modify = new QPushButton(i18n("Modify"), frame);
//	remove = new QPushButton(i18n("Remove"), frame);

	vbox = new QVBoxLayout();
	vbox->addWidget(m_settings);
//	hbox = new QHBoxLayout(vbox, 5);
//	hbox->add(add);
//	hbox->add(modify);
//	hbox->add(remove);
	frame->setLayout(vbox);

//	vbox = new QVBoxLayout(plainPage(), 5);
//	vbox->add(m_settings);

	connect(m_settings, SIGNAL(valueChanged(int, int)), SLOT(slotChanged(int, int)));
}

Plugin::~Plugin()
{
}

void Plugin::addColumn(QString caption)
{
	//m_settings->addColumn(caption);
	m_settings->setColumnCount(m_settings->columnCount() + 1);
	m_settings->horizontalHeaderItem(m_settings->columnCount() - 1)->setText(caption);
}

void Plugin::addRow(QStringList row)
{
	int i = 0;
	for(QStringList::Iterator it = row.begin(); it != row.end(); it++)
	{
		m_settings->item(m_settings->rowCount() - 1, i)->setText((*it));
		i++;
	}
	m_settings->setRowCount(m_settings->rowCount() + 1);
}

int Plugin::numRows()
{
	return m_settings->rowCount() - 1;
}

QStringList Plugin::getRow(int number)
{
	QStringList l;
	for(int i = 0; i < m_settings->columnCount(); i++)
		l << m_settings->item(number, i)->text();
	return l;
}

void Plugin::slotChanged(int row, int col)
{
	bool empty = true;

	Q_UNUSED(col);

	for(int i = 0; i < m_settings->columnCount(); i++)
		if(!(m_settings->item(row, i)->text().isEmpty()))
			empty = false;
	if(empty)
	{
		m_settings->removeRow(row);
		row -= 1;
	}
	if(row == m_settings->rowCount() - 1)
		m_settings->setRowCount(row + 2);
}

