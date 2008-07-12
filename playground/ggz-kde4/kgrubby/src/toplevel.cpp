#include "toplevel.h"
#include "app.h"

#include <kmenubar.h>
#include <kmenu.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kmessagebox.h>

Toplevel::Toplevel()
: KMainWindow()
{
	m_app = new App(this);
	setCentralWidget(m_app);

	menu_file = new KMenu(this);
	action_save = menu_file->addAction(KIconLoader::global()->loadIcon("filesave", KIconLoader::Small),
		i18n("Save"));
	menu_file->addSeparator();
	action_quit = menu_file->addAction(KIconLoader::global()->loadIcon("exit", KIconLoader::Small),
		i18n("Quit"));

	//menu_file->setItemEnabled(menusave, false);
	action_save->setEnabled(false);

	menu_file->setTitle(i18n("Program"));

	menuBar()->addMenu(menu_file);
	menuBar()->addMenu(helpMenu());

	connect(menu_file, SIGNAL(triggered(QAction*)), SLOT(slotMenu(QAction*)));

	connect(m_app, SIGNAL(signalChanged(bool)), SLOT(slotChanged(bool)));

	show();
}

Toplevel::~Toplevel()
{
}

void Toplevel::slotMenu(QAction *action)
{
	if(action == action_save)
	{
		m_app->saveProfile();
	}
	else if(action == action_quit)
	{
		//if(menu_file->isItemEnabled(menusave))
		if(action_save->isEnabled())
		{
			int ret = KMessageBox::questionYesNo(this,
				i18n("Should the changes be saved before quitting?"),
				i18n("Profile change"));
			if(ret == KMessageBox::Yes)
				m_app->saveProfile();
		}
		close();
	}
}

void Toplevel::slotChanged(bool changed)
{
	//menu_file->setItemEnabled(menusave, changed);
	action_save->setEnabled(changed);
}

