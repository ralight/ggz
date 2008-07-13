#include "plugin.h"

#include <klocale.h>
#include <kdebug.h>

#include <qlayout.h>
#include <qpushbutton.h>
#include <qtablewidget.h>
#include <qheaderview.h>

Plugin::Plugin(QWidget *parent)
: KDialog(parent)
{
	QWidget *frame;
	QVBoxLayout *vbox;

	setCaption(i18n("Plugin configuration"));
	setButtons(KDialog::Ok | KDialog::Cancel);

	frame = new QWidget();
	setMainWidget(frame);

	m_settings = new QTableWidget();
	m_settings->setRowCount(1);
	m_settings->setColumnCount(0);

	vbox = new QVBoxLayout();
	vbox->addWidget(m_settings);
	frame->setLayout(vbox);

	connect(m_settings, SIGNAL(cellChanged(int, int)), SLOT(slotChanged(int, int)));
}

Plugin::~Plugin()
{
}

void Plugin::addColumn(QString caption)
{
	QTableWidgetItem *item = new QTableWidgetItem();
	item->setText(caption);
	m_settings->setColumnCount(m_settings->columnCount() + 1);
	m_settings->setHorizontalHeaderItem(m_settings->columnCount() - 1, item);
}

void Plugin::addRow(QStringList row)
{
	int i = 0;
	for(QStringList::Iterator it = row.begin(); it != row.end(); it++)
	{
		QTableWidgetItem *item = new QTableWidgetItem();
		item->setText((*it));
		m_settings->setItem(m_settings->rowCount() - 1, i, item);
		i++;
	}
	m_settings->setRowCount(m_settings->rowCount() + 1);
}

int Plugin::numRows()
{
	return m_settings->rowCount() - 1;
}

QStringList Plugin::getRow(int row)
{
	QStringList l;
	for(int i = 0; i < m_settings->columnCount(); i++)
	{
		QTableWidgetItem *item = m_settings->item(row, i);
		if(item)
			l << item->text();
		else
			l << QString();
	}
	return l;
}

void Plugin::slotChanged(int row, int col)
{
	bool empty = true;

	Q_UNUSED(col);

	for(int i = 0; i < m_settings->columnCount(); i++)
	{
		QTableWidgetItem *item = m_settings->item(row, i);
		if((item) && (!(item->text().isEmpty())))
			empty = false;
	}
	if(empty)
	{
		m_settings->removeRow(row);
		row -= 1;
	}
	if(row == m_settings->rowCount() - 1)
		m_settings->setRowCount(row + 2);
}

