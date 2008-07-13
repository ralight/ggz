//
//    Competition Calendar
//
//    Copyright (C) 2002, 2003 Josef Spillner <josef@ggzgamingzone.org>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include "toplevel.h"
#include "app.h"
#include "conf.h"

#include <kmenubar.h>
#include <kpopupmenu.h>
#include <klocale.h>

Toplevel::Toplevel()
: KMainWindow()
{
	KPopupMenu *menu_file, *menu_view;

	m_app = new App(this);
	setCentralWidget(m_app);

	menu_file = new KPopupMenu(this);
	menu_file->insertItem(i18n("Configure"), menuconfigure);
	menu_file->insertItem(i18n("Update events"), menuupdate);
	menu_file->insertItem(i18n("Quit"), menuquit);

	menu_view = new KPopupMenu(this);
	menu_view->insertItem(i18n("Sorted"), menuviewsorted);
	menu_view->insertItem(i18n("Chronological"), menuviewchronological);

	menuBar()->insertItem(i18n("Program"), menu_file);
	menuBar()->insertItem(i18n("View"), menu_view);
	menuBar()->insertItem(i18n("Help"), helpMenu());

	connect(menu_file, SIGNAL(activated(int)), SLOT(slotMenu(int)));
	connect(menu_view, SIGNAL(activated(int)), SLOT(slotMenu(int)));

	show();
}

Toplevel::~Toplevel()
{
}

void Toplevel::slotMenu(int id)
{
	switch(id)
	{
		case menuconfigure:
			configure();
			break;
		case menuupdate:
			m_app->fetch();
			break;
		case menuviewsorted:
			m_app->view(App::sorted);
			break;
		case menuviewchronological:
			m_app->view(App::chronological);
			break;
		case menuquit:
			close();
			break;
	}
}

void Toplevel::configure()
{
	int ret;

	Conf c(this);
	ret = c.exec();
	if(ret == QDialog::Accepted)
	{
	}
}

