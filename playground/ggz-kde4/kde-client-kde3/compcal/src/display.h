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

#ifndef COMPCAL_DISPLAY_H
#define COMPCAL_DISPLAY_H

#include <qwidget.h>

class KProcess;
class Event;
class KListView;
class KURLLabel;
class QTextEdit;
class QLabel;
class QFrame;

class Eventdisplay : public QWidget
{
	Q_OBJECT
	public:
		Eventdisplay(QWidget *parent = NULL, const char *name = NULL);
		~Eventdisplay();
		void setEvent(Event *event);

	public slots:
		void slotUrl(const QString& url);
		void slotProcess(KProcess *proc);

	private:
		void restoreDefaults();

		KListView *m_list;
		QLabel *participants;
		QFrame *image;
		QLabel *title, *date;
		KURLLabel *location;
		QTextEdit *description;
		QWidget *logo;
};

#endif

