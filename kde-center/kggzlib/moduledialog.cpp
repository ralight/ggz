// Module configuration dialog includes
#include "moduledialog.h"
#include "qrecursivesortfilterproxymodel.h"

// KGGZ includes
#include <kggzcore/module.h>
#include <kggzcore/coreclient.h>

// KDE includes
#include <klocale.h>

// Qt includes
#include <qlayout.h>
#include <qpushbutton.h>
#include <qtablewidget.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qstandarditemmodel.h>

ModuleDialog::ModuleDialog(QWidget *parent)
: QDialog(parent)
{
	m_modules = new QTableView();
	m_modules->setEditTriggers(QAbstractItemView::NoEditTriggers);

	m_model = new QStandardItemModel();
	m_model->setColumnCount(10);

	m_model->setHeaderData(0, Qt::Horizontal, i18n("Name"), Qt::DisplayRole);
	m_model->setHeaderData(1, Qt::Horizontal, i18n("Version"), Qt::DisplayRole);
	m_model->setHeaderData(2, Qt::Horizontal, i18n("Protocol engine"), Qt::DisplayRole);
	m_model->setHeaderData(3, Qt::Horizontal, i18n("Protocol version"), Qt::DisplayRole);
	m_model->setHeaderData(4, Qt::Horizontal, i18n("Author"), Qt::DisplayRole);
	m_model->setHeaderData(5, Qt::Horizontal, i18n("Frontend"), Qt::DisplayRole);
	m_model->setHeaderData(6, Qt::Horizontal, i18n("Homepage"), Qt::DisplayRole);
	m_model->setHeaderData(7, Qt::Horizontal, i18n("Help path"), Qt::DisplayRole);
	m_model->setHeaderData(8, Qt::Horizontal, i18n("Icon path"), Qt::DisplayRole);
	m_model->setHeaderData(9, Qt::Horizontal, i18n("Environment"), Qt::DisplayRole);

	m_proxymodel = new QRecursiveSortFilterProxyModel(this);
	m_proxymodel->setSourceModel(m_model);
	m_proxymodel->setDynamicSortFilter(true);
	m_proxymodel->setFilterKeyColumn(-1);

	m_modules->setModel(m_proxymodel);

	QPushButton *dismiss_button = new QPushButton(i18n("Dismiss"));

	QLineEdit *searchinput = new QLineEdit();
	QLabel *searchlabel = new QLabel(i18n("Search for:"));

	QHBoxLayout *searchbox = new QHBoxLayout();
	searchbox->addWidget(searchlabel);
	searchbox->addWidget(searchinput);

	QHBoxLayout *hbox = new QHBoxLayout();
	hbox->addStretch();
	hbox->addWidget(dismiss_button);

	QVBoxLayout *vbox = new QVBoxLayout();
	vbox->addWidget(m_modules);
	vbox->addLayout(searchbox);
	vbox->addLayout(hbox);
	setLayout(vbox);

	connect(dismiss_button, SIGNAL(clicked()), SLOT(close()));
	connect(searchinput, SIGNAL(textChanged(const QString&)), SLOT(slotSearch(const QString&)));

	load();

	setWindowTitle(i18n("Game Modules"));
	resize(500, 350);
	show();
}

void ModuleDialog::load()
{
	KGGZCore::CoreClient core;
	QList<KGGZCore::Module> modules = core.modules();

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

		QList<QStandardItem*> items;

		QStandardItem *item_name = new QStandardItem(module.name());
		QStandardItem *item_version = new QStandardItem(module.version());
		QStandardItem *item_protoengine = new QStandardItem(module.protocolEngine());
		QStandardItem *item_protoversion = new QStandardItem(module.protocolVersion());
		QStandardItem *item_author = new QStandardItem(module.author());
		QStandardItem *item_frontend = new QStandardItem(module.frontend());
		QStandardItem *item_homepage = new QStandardItem(module.homepage());
		QStandardItem *item_helppath = new QStandardItem(module.helpPath());
		QStandardItem *item_iconpath = new QStandardItem(module.iconPath());
		QStandardItem *item_environment = new QStandardItem(env);

		items << item_name;
		items << item_version;
		items << item_protoengine;
		items << item_protoversion;
		items << item_author;
		items << item_frontend;
		items << item_homepage;
		items << item_helppath;
		items << item_iconpath;
		items << item_environment;

		m_model->appendRow(items);
	}

	m_modules->resizeColumnsToContents();
	// FIXME: enabling sorting before will mess up the item order...
	m_modules->setSortingEnabled(true);
}

void ModuleDialog::slotSearch(const QString& text)
{
	m_proxymodel->setFilterRegExp(QRegExp(text, Qt::CaseInsensitive, QRegExp::FixedString));
	//m_modules->expandAll();
}

