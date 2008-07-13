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

#ifndef KEEPALIVE_CONTROL_APP_H
#define KEEPALIVE_CONTROL_APP_H

#include <qwidget.h>

class KListView;
class QSocket;
class QDataStream;
class QListViewItem;
class QPopupMenu;

class App : public QWidget
{
	Q_OBJECT
	public:
		App(QWidget *parent = NULL, const char *name = NULL);
		~App();

	public slots:
		void slotLogin(QSocket *sock);
		void slotInput();
		void slotSelected(QListViewItem *item, const QPoint& point, int column);
		void slotItem(int id);
		void slotCreate(QString worldname);

	private:
		enum Items
		{
			menucreate,
			menufreeze,
			menuunfreeze,
			menudestroy,
			menuban,
			menucount
		};

		KListView *m_list;
		QSocket *m_sock;
		QDataStream *s;
		QPopupMenu *popup;
		QListViewItem *commanditem;
		int commandnumber;
		QString world;
};

#endif

