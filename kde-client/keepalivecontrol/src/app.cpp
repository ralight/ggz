//
//    Keepalive Control
//
//    Copyright (C) 2002 Josef Spillner <dr_maux@users.sourceforge.net>
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

#include "app.h"
#include "create.h"

#include <klocale.h>
#include <klistview.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kdebug.h>

#include <qlayout.h>
#include <qdatastream.h>
#include <qsocket.h>
#include <qpopupmenu.h>

App::App(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	QHBoxLayout *hbox;

	popup = NULL;
	m_create = NULL;

	m_list = new KListView(this);
	m_list->addColumn(i18n("Server content"));
	m_list->setRootIsDecorated(true);

	(void)new QListViewItem(m_list, i18n("Worlds"));
	(void)new QListViewItem(m_list, i18n("Avatars"));

	hbox = new QHBoxLayout(this, 5);
	hbox->add(m_list);

	connect(m_list, SIGNAL(rightButtonPressed(QListViewItem*, const QPoint&, int)),
		SLOT(slotSelected(QListViewItem*, const QPoint&, int)));
}

App::~App()
{
}

void App::slotLogin(QSocket *sock)
{
	const int type_admin = 16;
	const int command_list = 1;
	const int option_worlds = 2;
	int length;

	m_sock = sock;
	connect(m_sock, SIGNAL(readyRead()), SLOT(slotInput()));

	length = 6;

	s = new QDataStream(m_sock);
	*s << (Q_INT8)type_admin;
	*s << (Q_INT8)0;
	*s << (Q_INT8)length;
	*s << (Q_INT8)0;
	*s << (Q_INT8)command_list;
	*s << (Q_INT8)option_worlds;
}

void App::slotInput()
{
	const int type_admin = 16;
	const int command_listanswer = 2;
	const int option_worlds = 2;
	const int option_avatars = 1;
	unsigned char type;
	unsigned char opcode;
	short length;
	unsigned char number;
	char *worldbuf;
	int error = 0;
	QString world;
	int count;
	QListViewItem *parent;

	if(m_sock->bytesAvailable() < 7) return;

	*s >> type;
	if(type != type_admin) error = 1;
	else
	{
		*s >> opcode;
		*s >> length;
		length = ((length & 0x00FF) << 8) + ((length & 0xFF00) >> 8);
		if(length < 7) error = 1;
		else
		{
			*s >> opcode;
			if(opcode != command_listanswer) error = 1;
			else
			{
				*s >> opcode;
				*s >> number;
				worldbuf = new char[m_sock->bytesAvailable()];
				s->readRawBytes(worldbuf, m_sock->bytesAvailable());
				world = worldbuf;
				count = 0;
				parent = m_list->firstChild();
				while(parent)
				{
					if((opcode == option_worlds) && (parent->text(0) == i18n("Worlds"))) break;
					if((opcode == option_avatars) && (parent->text(0) == i18n("Avatars"))) break;
					parent = parent->nextSibling();
				}
				for(int i = 0; i < number; i++)
				{
					(void)new QListViewItem(parent, world);
					while(worldbuf[count++]);
					world = worldbuf + count;
				}
				delete[] worldbuf;
			}
		}
	}
}

void App::slotSelected(QListViewItem *item, const QPoint& point, int column)
{
	if(!item) return;

	if(popup) delete popup;
	popup = new QPopupMenu(this);

	if(item->parent())
	{
		if(item->parent()->text(0) == i18n("Worlds"))
		{
			popup->insertItem(i18n("Remove this world"), menudestroy);
			popup->insertItem(i18n("Freeze"), menufreeze);
			popup->insertItem(i18n("Unfreeze"), menuunfreeze);
		}
		else if(item->parent()->text(0) == i18n("Avatars"))
		{
			popup->insertItem(i18n("Ban"), menuban);
		}
	}
	else
	{
		if(item->text(0) == i18n("Worlds"))
		{
			popup->insertItem(i18n("Create new world"), menucreate);
		}
		else if(item->text(0) == i18n("Avatars"))
		{
			popup->insertItem(i18n("Count"), menucount);
		}
	}

	popup->popup(point);
	connect(popup, SIGNAL(activated(int)), SLOT(slotItem(int)));
}

void App::slotItem(int id)
{
	const int type_admin = 16;
	const int command_destroyworld = 7;
	const int command_freezeworld = 8;
	const int command_unfreezeworld = 9;
	int length;

	switch(id)
	{
		case menucreate:
			if(!m_create)
			{
				m_create = new Create();
				connect(m_create, SIGNAL(signalWorld(QString)), SLOT(slotCreate(QString)));
			}
			m_create->show();
			break;
		case menuban:
			// ???
			break;
		case menucount:
			// statistics
			break;
		case menudestroy:
			length = 6;
			*s << (Q_INT8)type_admin;
			*s << (Q_INT8)0;
			*s << (Q_INT8)length;
			*s << (Q_INT8)0;
			*s << (Q_INT8)command_destroyworld;
			*s << (Q_INT8)0;
			break;
		case menufreeze:
			length = 6;
			*s << (Q_INT8)type_admin;
			*s << (Q_INT8)0;
			*s << (Q_INT8)length;
			*s << (Q_INT8)0;
			*s << (Q_INT8)command_freezeworld;
			*s << (Q_INT8)0;
			break;
		case menuunfreeze:
			length = 6;
			*s << (Q_INT8)type_admin;
			*s << (Q_INT8)0;
			*s << (Q_INT8)length;
			*s << (Q_INT8)0;
			*s << (Q_INT8)command_unfreezeworld;
			*s << (Q_INT8)0;
			break;
		default:
			break;
	}
}

void App::slotCreate(QString world)
{
	const int type_admin = 16;
	const int command_createworld = 6;
	int length;
	
	length = 6;

	*s << (Q_INT8)type_admin;
	*s << (Q_INT8)0;
	*s << (Q_INT8)length;
	*s << (Q_INT8)0;
	*s << (Q_INT8)command_createworld;
	*s << (Q_INT8)0;
}

