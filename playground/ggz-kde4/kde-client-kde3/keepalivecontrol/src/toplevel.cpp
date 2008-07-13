//
//    Keepalive Control
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
#include "connection.h"

#include <kmenubar.h>
#include <kpopupmenu.h>
#include <klocale.h>

Toplevel::Toplevel()
: KMainWindow()
{
	KPopupMenu *menu_file;

	m_app = new App(this);
	setCentralWidget(m_app);

	menu_file = new KPopupMenu(this);
	menu_file->insertItem(i18n("Connect"), menuconnect);
	menu_file->insertSeparator();
	menu_file->insertItem(i18n("Quit"), menuquit);

	menuBar()->insertItem(i18n("Program"), menu_file);
	menuBar()->insertItem(i18n("Help"), helpMenu());

	connect(menu_file, SIGNAL(activated(int)), SLOT(slotMenu(int)));

	setCaption(i18n("Keepalive Control"));
	show();
}

Toplevel::~Toplevel()
{
}

void Toplevel::slotMenu(int id)
{
	switch(id)
	{
		case menuquit:
			close();
			break;
		case menuconnect:
			connection();
			break;
	}
}

void Toplevel::connection()
{
	Connection c(this);

	connect(&c, SIGNAL(signalLogin(QSocket*)), m_app, SLOT(slotLogin(QSocket*)));
	c.exec();
}

