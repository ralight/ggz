#include "plugin.h"

//#include <klistview.h>
#include <klocale.h>

#include <qlayout.h>
#include <qpushbutton.h>
#include <qvbox.h>
#include <qtable.h>

Plugin::Plugin(QWidget *parent, const char *name)
: KDialogBase(parent, name, true, i18n("Plugin configuration"),
	KDialogBase::Ok | KDialogBase::Cancel, KDialogBase::Ok, true)
{
	QWidget *frame;
//	QHBoxLayout *hbox;
	QVBoxLayout *vbox;
//	QPushButton *add, *modify, *remove;

	frame = new QWidget(this);
	setMainWidget(frame);

	//m_settings = new KListView(frame);
	m_settings = new QTable(frame);
	m_settings->setNumRows(1);

//	add = new QPushButton(i18n("Add"), frame);
//	modify = new QPushButton(i18n("Modify"), frame);
//	remove = new QPushButton(i18n("Remove"), frame);

	vbox = new QVBoxLayout(frame, 5);
	vbox->add(m_settings);
//	hbox = new QHBoxLayout(vbox, 5);
//	hbox->add(add);
//	hbox->add(modify);
//	hbox->add(remove);

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
	m_settings->setNumCols(m_settings->numCols() + 1);
	m_settings->horizontalHeader()->setLabel(m_settings->numCols() - 1, caption);
}

void Plugin::addRow(QStringList row)
{
	int i = 0;
	for(QStringList::Iterator it = row.begin(); it != row.end(); it++)
	{
		m_settings->setText(m_settings->numRows() - 1, i, (*it));
		i++;
	}
	m_settings->setNumRows(m_settings->numRows() + 1);
}

int Plugin::numRows()
{
	return m_settings->numRows() - 1;
}

QStringList Plugin::getRow(int number)
{
	QStringList l;
	for(int i = 0; i < m_settings->numCols(); i++)
		l << m_settings->text(number, i);
	return l;
}

void Plugin::slotChanged(int row, int col)
{
	bool empty = true;
	for(int i = 0; i < m_settings->numCols(); i++)
		if(!(m_settings->text(row, i).isEmpty()))
			empty = false;
	if(empty)
	{
		m_settings->removeRow(row);
		row -= 1;
	}
	if(row == m_settings->numRows() - 1)
		m_settings->setNumRows(row + 2);
}

