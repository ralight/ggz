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
	QVBoxLayout *vbox, *vboxroot;
	QHBoxLayout *hbox;
	QLabel *lusername, *lpassword;
	QPushButton *ok;
	QFrame *frame;

	frame = new QFrame(this);
	frame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	frame->setFixedHeight(200);

	lusername = new QLabel("Username", frame);
	lpassword = new QLabel("Password", frame);
	m_username = new QLineEdit(frame);
	m_password = new QLineEdit(frame);
	ok = new QPushButton("Login...", frame);
	ok->setFixedWidth(100);

	vbox = new QVBoxLayout(frame, 5);
	vbox->add(lusername);
	vbox->add(m_username);
	vbox->add(lpassword);
	vbox->add(m_password);
	hbox = new QHBoxLayout(vbox, 5);
	hbox->addStretch();
	hbox->add(ok);

	vboxroot = new QVBoxLayout(this, 5);
	vboxroot->addStretch();
	vboxroot->add(frame);

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

