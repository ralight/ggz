// Keepalive - Experimental GGZ game
// Copyright (C) 2002 Josef Spillner, dr_maux@users.sourceforge.net
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

#ifndef KEEPALIVE_LOGIN_H
#define KEEPALIVE_LOGIN_H

// Qt includes
#include <qwidget.h>

// Class declarations
class QLineEdit;

// Login mask to authorize players
class Login : public QWidget
{
	Q_OBJECT
	public:
		Login(QWidget *parent = NULL, const char *name = NULL);
		~Login();

	signals:
		void signalLogin(QString username, QString password, QString hostname);

	public slots:
		void slotLogin();

	private:
		QLineEdit *m_username, *m_password;
};

#endif

