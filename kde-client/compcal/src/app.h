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

#ifndef COMPCAL_APP_H
#define COMPCAL_APP_H

#include <qsplitter.h>

class KListView;
class Eventdisplay;
class QListViewItem;
class Parser;

class App : public QSplitter
{
	Q_OBJECT
	public:
		App(QWidget *parent = NULL, const char *name = NULL);
		~App();
		void view(int mode);
		void load(bool destructive);
		void fetch();

		enum Modes
		{
			chronological,
			sorted
		};

	public slots:
		void slotEvent(QListViewItem *item);

	private:
		KListView *m_list;
		Eventdisplay *m_display;
		int m_view;
		Parser *p;
};

#endif

