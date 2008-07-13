// Krosswater - Cross The Water for KDE
// Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef DLG_PERSON_H
#define DLG_PERSON_H

// Qt includes
#include <qwidget.h>
#include <qevent.h>

// Dialog to let the player select his character
class DlgPerson : public QWidget
{
	Q_OBJECT
	public:
		DlgPerson(QWidget *parent = NULL, const char *name = NULL);
		~DlgPerson();

	signals:
		void signalAccepted(int person);

	protected:
		void paintEvent(QPaintEvent *e);

	private slots:
		void slotPerson1();
		void slotPerson2();
		void slotPerson3();
};

#endif

