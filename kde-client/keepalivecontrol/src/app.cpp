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
	QListViewItem *tmp;
	KStandardDirs d;

	popup = NULL;
	commanditem = NULL;
	commandnumber = -1;

	s = NULL;

	m_list = new KListView(this);
	m_list->addColumn(i18n("Server content"));
	m_list->setRootIsDecorated(true);

	tmp = new QListViewItem(m_list, i18n("Worlds"));
	QPixmap worlds(d.findResource("data", "keepalivecontrol/worlds.png"));
	tmp->setPixmap(0, worlds);

	tmp = new QListViewItem(m_list, i18n("Avatars"));
	QPixmap avatars(d.findResource("data", "keepalivecontrol/avatars.png"));
	tmp->setPixmap(0, avatars);

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
	const int command_success = 11;
	const int command_failure = 12;
	const int option_worlds = 2;
	const int option_avatars = 1;
	//const int option_normal = 5;
	//const int option_frozen = 6;
	unsigned char type;
	unsigned char opcode;
	short length;
	unsigned char number;
	char *worldbuf;
	int error = 0;
	QString world;
	int count;
	QListViewItem *parent, *tmp;
	KStandardDirs d;

	if(m_sock->bytesAvailable() < 7) return;

	*s >> type;
	if(type != type_admin) error = 1;
	else
	{
		*s >> opcode;
		*s >> length;
		length = ((length & 0x00FF) << 8) + ((length & 0xFF00) >> 8);
		if(length < 5) error = 1;
		else
		{
			*s >> opcode;
			if(opcode == command_listanswer)
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
					tmp = new QListViewItem(parent, world);
					QPixmap normalworld(d.findResource("data", "keepalivecontrol/normalworld.png"));
					tmp->setPixmap(0, normalworld);
					while(worldbuf[count++]);
					world = worldbuf + count;
				}
				delete[] worldbuf;
			}
			else if(opcode == command_success)
			{
				KMessageBox::information(this, i18n("Command executed successfully."), i18n("Status"));
				QPixmap frozenworld(d.findResource("data", "keepalivecontrol/frozenworld.png"));
				QPixmap normalworld(d.findResource("data", "keepalivecontrol/normalworld.png"));
				switch(commandnumber)
				{
					case menucreate:
						if(commanditem)
						{
							tmp = new QListViewItem(commanditem, world);
							tmp->setPixmap(0, normalworld);
						}
						break;
					case menudestroy:
						if(commanditem) delete commanditem;
						break;
					case menufreeze:
						if(commanditem) commanditem->setPixmap(0, frozenworld);
						break;
					case menuunfreeze:
						if(commanditem) commanditem->setPixmap(0, normalworld);
						break;
					default:
						break;
				}
			}
			else if(opcode == command_failure)
			{
				KMessageBox::sorry(this, i18n("Command failed."), i18n("Status"));
			}
			else error = 1;
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

	commanditem = item;

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

	commandnumber = id;

	switch(id)
	{
		case menucreate:
			slotCreate(QString::null);
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
	int ret;

	if(!s) return;

	if(world.isNull())
	{
		Create c(this);
		ret = c.exec();
		if(ret == QDialog::Accepted)
		{
			world = c.world();
		}
		else return;
	}
	
	length = 6;

	*s << (Q_INT8)type_admin;
	*s << (Q_INT8)0;
	*s << (Q_INT8)length;
	*s << (Q_INT8)0;
	*s << (Q_INT8)command_createworld;
	*s << (Q_INT8)0;
}

