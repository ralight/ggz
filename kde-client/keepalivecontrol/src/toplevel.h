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

#ifndef KEEPALIVE_CONTROL_TOPLEVEL_H
#define KEEPALIVE_CONTROL_TOPLEVEL_H

#include <kmainwindow.h>

class App;
class Connection;

class Toplevel : public KMainWindow
{
	Q_OBJECT
	public:
		Toplevel();
		~Toplevel();

	public slots:
		void slotMenu(int id);

	private:
		enum MenuItems
		{
			menuconnect,
			menuquit
		};

		App *m_app;
		Connection *m_connect;
};

#endif

