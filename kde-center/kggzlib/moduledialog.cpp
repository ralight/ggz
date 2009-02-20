// Module configuration dialog includes
#include "moduledialog.h"

// KGGZ includes
#include <kggzcore/module.h>
#include <kggzcore/coreclient.h>

// Qt includes
#include <qlayout.h>
#include <qpushbutton.h>
#include <qtablewidget.h>

ModuleDialog::ModuleDialog(QWidget *parent)
: QDialog(parent)
{
	m_modules = new QTableWidget();
	m_modules->setColumnCount(1);

	QStringList labels;
	labels << "Name";
	m_modules->setHorizontalHeaderLabels(labels);

	QPushButton *dismiss_button = new QPushButton("Dismiss");

	QHBoxLayout *hbox = new QHBoxLayout();
	hbox->addStretch();
	hbox->addWidget(dismiss_button);

	QVBoxLayout *vbox = new QVBoxLayout();
	vbox->addWidget(m_modules);
	vbox->addLayout(hbox);
	setLayout(vbox);

	connect(dismiss_button, SIGNAL(clicked()), SLOT(close()));

	load();

	setWindowTitle("Game Modules");
	resize(500, 350);
	show();
}

void ModuleDialog::load()
{
	KGGZCore::CoreClient core;
	QList<KGGZCore::Module> modules = core.modules();
	m_modules->setRowCount(modules.size());

	for(int i = 0; i < modules.size(); i++)
	{
		KGGZCore::Module module = modules.at(i);
		QTableWidgetItem *item = new QTableWidgetItem(module.name());
		m_modules->setItem(i, 0, item);
	}

	m_modules->resizeColumnsToContents();
}

