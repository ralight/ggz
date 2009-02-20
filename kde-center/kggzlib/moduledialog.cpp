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
	m_modules->setColumnCount(10);

	QStringList labels;
	labels << "Name";
	labels << "Version";
	labels << "Protocol engine";
	labels << "Protocol version";
	labels << "Author";
	labels << "Frontend";
	labels << "Homepage";
	labels << "Help path";
	labels << "Icon path";
	labels << "Environment";
	m_modules->setHorizontalHeaderLabels(labels);
	m_modules->setEditTriggers(QAbstractItemView::NoEditTriggers);

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

		QMap<KGGZCore::Module::Environment, QString> envnames;
		envnames[KGGZCore::Module::passive] = "(passive)";
		envnames[KGGZCore::Module::console] = "Text console";
		envnames[KGGZCore::Module::framebuffer] = "Framebuffer";
		envnames[KGGZCore::Module::xwindow] = "X11 window";
		envnames[KGGZCore::Module::xfullscreen] = "X11 fullscreen";

		QString env = envnames[module.environment()];

		QTableWidgetItem *item_name = new QTableWidgetItem(module.name());
		m_modules->setItem(i, 0, item_name);
		QTableWidgetItem *item_version = new QTableWidgetItem(module.version());
		m_modules->setItem(i, 1, item_version);
		QTableWidgetItem *item_protoengine = new QTableWidgetItem(module.protocolEngine());
		m_modules->setItem(i, 2, item_protoengine);
		QTableWidgetItem *item_protoversion = new QTableWidgetItem(module.protocolVersion());
		m_modules->setItem(i, 3, item_protoversion);
		QTableWidgetItem *item_author = new QTableWidgetItem(module.author());
		m_modules->setItem(i, 4, item_author);
		QTableWidgetItem *item_frontend = new QTableWidgetItem(module.frontend());
		m_modules->setItem(i, 5, item_frontend);
		QTableWidgetItem *item_homepage = new QTableWidgetItem(module.homepage());
		m_modules->setItem(i, 6, item_homepage);
		QTableWidgetItem *item_helppath = new QTableWidgetItem(module.helpPath());
		m_modules->setItem(i, 7, item_helppath);
		QTableWidgetItem *item_iconpath = new QTableWidgetItem(module.iconPath());
		m_modules->setItem(i, 8, item_iconpath);
		QTableWidgetItem *item_environment = new QTableWidgetItem(env);
		m_modules->setItem(i, 9, item_environment);
	}

	m_modules->resizeColumnsToContents();
	// FIXME: enabling sorting before will mess up the item order...
	m_modules->setSortingEnabled(true);
}

