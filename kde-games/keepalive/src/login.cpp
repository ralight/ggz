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

// Header file
#include "login.h"

// Qt includes
#include <qlayout.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qpushbutton.h>

// Constructor: show mask to query username/password
Login::Login(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox;
	QLabel *lusername, *lpassword;
	QPushButton *ok;

	lusername = new QLabel("Username", this);
	lpassword = new QLabel("Password", this);
	m_username = new QLineEdit(this);
	m_password = new QLineEdit(this);
	ok = new QPushButton("Login...", this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(lusername);
	vbox->add(m_username);
	vbox->add(lpassword);
	vbox->add(m_password);
	vbox->add(ok);

	connect(ok, SIGNAL(clicked()), SLOT(slotLogin()));

	show();
}

// Destructor
Login::~Login()
{
}

// Send login attempt to the server
void Login::slotLogin()
{
	emit signalLogin(m_username->text(), m_password->text());
}

