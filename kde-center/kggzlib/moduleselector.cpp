#include "moduleselector.h"

#include <QtGui/QPushButton>
#include <QtGui/QLayout>

#include <qlabel.h>

#include <klocale.h>
#include <kstandarddirs.h>

#include <kggzcore/coreclient.h>

#include "modulelist.h"

ModuleSelector::ModuleSelector(QWidget *parent)
: QDialog(parent), m_knowsmodules(false)
{
	KStandardDirs d;

	m_modules = new ModuleList();

	m_button = new QPushButton(i18n("Select this game client"));
	m_button->setEnabled(false);

	QPushButton *cancel = new QPushButton(i18n("Cancel"));

	QHBoxLayout *hbox = new QHBoxLayout();
	hbox->addWidget(m_button);
	hbox->addWidget(cancel);

	/*QLabel *logo = new QLabel();
	logo->setFixedSize(64, 64);
	logo->setPixmap(QPixmap(d.findResource("data", "kggzlib/icons/ggzmetaserv.png")));

	QHBoxLayout *hboxtop = new QHBoxLayout();
	hboxtop->addWidget(logo);
	hboxtop->addStretch(1);*/

	QVBoxLayout *vbox = new QVBoxLayout();
	setLayout(vbox);
	/*vbox->addLayout(hboxtop);*/
	vbox->addWidget(m_modules);
	vbox->addLayout(hbox);

	connect(m_button, SIGNAL(clicked()), SLOT(accept()));
	connect(cancel, SIGNAL(clicked()), SLOT(reject()));
	connect(m_modules, SIGNAL(signalSelected(const KGGZCore::Module&)), SLOT(slotSelected(const KGGZCore::Module&)));

	setWindowTitle(i18n("Game client module selection"));
	resize(400, 550);
	show();
}

ModuleSelector::~ModuleSelector()
{
}

// FIXME: This method is only needed due to issues with initialising ggzcore twice!
void ModuleSelector::setModules(QList<KGGZCore::Module> modules)
{
	m_knownmodules = modules;
	m_knowsmodules = true;
}

void ModuleSelector::setGameType(QString protengine, QString protversion)
{
	QList<KGGZCore::Module> modules;
	if(!m_knowsmodules)
	{
		KGGZCore::CoreClient core;
		modules = core.modules();
	}
	else
	{
		modules = m_knownmodules;
	}

	for(int i = 0; i < modules.size(); i++)
	{
		KGGZCore::Module module = modules.at(i);

		if((module.protocolEngine() == protengine)
		&& (module.protocolVersion() == protversion))
			m_modules->addModule(module);
	}
}

KGGZCore::Module ModuleSelector::module()
{
	return m_module;
}

void ModuleSelector::slotSelected(const KGGZCore::Module& module)
{
	m_module = module;
	m_button->setEnabled(!module.name().isEmpty());
}

#include "moduleselector.moc"
