#include "toplevel.h"
#include "app.h"

#include <kmenubar.h>
#include <kpopupmenu.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kmessagebox.h>

Toplevel::Toplevel()
: KMainWindow()
{
	m_app = new App(this);
	setCentralWidget(m_app);

	menu_file = new KPopupMenu(this);
	menu_file->insertItem(KGlobal::iconLoader()->loadIcon("filesave", KIcon::Small),
		i18n("Save"), menusave);
	menu_file->insertSeparator();
	menu_file->insertItem(KGlobal::iconLoader()->loadIcon("exit", KIcon::Small),
		i18n("Quit"), menuquit);

	menu_file->setItemEnabled(menusave, false);

	menuBar()->insertItem(i18n("Program"), menu_file);
	menuBar()->insertItem(i18n("Help"), helpMenu());

	connect(menu_file, SIGNAL(activated(int)), SLOT(slotMenu(int)));

	connect(m_app, SIGNAL(signalChanged(bool)), SLOT(slotChanged(bool)));

	show();
}

Toplevel::~Toplevel()
{
}

void Toplevel::slotMenu(int id)
{
	switch(id)
	{
		case menusave:
			m_app->saveProfile();
			break;
		case menuquit:
			if(menu_file->isItemEnabled(menusave))
			{
				int ret = KMessageBox::questionYesNo(this,
					i18n("Should the changes be saved before quitting?"),
					i18n("Profile change"));
				if(ret == KMessageBox::Yes)
					m_app->saveProfile();
			}
			close();
			break;
	}
}

void Toplevel::slotChanged(bool changed)
{
	menu_file->setItemEnabled(menusave, changed);
}

