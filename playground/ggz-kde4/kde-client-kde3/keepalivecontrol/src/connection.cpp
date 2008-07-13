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

#include "connection.h"

#include <kapplication.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kconfig.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qprogressbar.h>
#include <qsocket.h>
#include <qdatastream.h>

Connection::Connection(QWidget *parent, const char *name)
: QDialog(parent, name, true)
{
	QVBoxLayout *vbox;
	QHBoxLayout *hbox;
	QLabel *label_server, *label_username, *label_password;
	QPushButton *button_connect, *button_cancel;
	KConfig *conf;
	QString server, username, password;

	conf = kapp->config();
	conf->setGroup("Connection");
	server = conf->readEntry("server");
	username = conf->readEntry("username");
	password = conf->readEntry("password");

	label_server = new QLabel(i18n("Server"), this);
	label_username = new QLabel(i18n("Username"), this);
	label_password = new QLabel(i18n("Password"), this);

	input_server = new QLineEdit(server, this);
	input_username = new QLineEdit(username, this);
	input_password = new QLineEdit(password, this);

	button_connect = new QPushButton(i18n("Connect"), this);
	button_cancel = new QPushButton(i18n("Cancel"), this);

	bar = new QProgressBar(4, this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(label_server);
	vbox->add(input_server);
	vbox->add(label_username);
	vbox->add(input_username);
	vbox->add(label_password);
	vbox->add(input_password);
	vbox->add(bar);
	hbox = new QHBoxLayout(vbox, 5);
	hbox->add(button_connect);
	hbox->add(button_cancel);

	connect(button_cancel, SIGNAL(clicked()), SLOT(close()));
	connect(button_connect, SIGNAL(clicked()), SLOT(slotConnect()));

	setCaption("Connection...");
	show();
}

Connection::~Connection()
{
}

void Connection::slotConnect()
{
	KConfig *conf;

	conf = kapp->config();
	conf->setGroup("Connection");
	conf->writeEntry("server", input_server->text());
	conf->writeEntry("username", input_username->text());
	conf->writeEntry("password", input_password->text());

	bar->setProgress(1);

	sock = new QSocket(this->parentWidget());
	//sock = new QSocket(this);
	sock->connectToHost(input_server->text(), 10001);
	connect(sock, SIGNAL(connected()), SLOT(slotConnected()));
	connect(sock, SIGNAL(error(int)), SLOT(slotError()));
}

void Connection::slotConnected()
{
	const int type_admin = 16;
	const int command_authorize = 5;
	int opcode = 0;
	int length;

	bar->setProgress(2);

	connect(sock, SIGNAL(readyRead()), SLOT(slotInput()));

	length = input_username->text().length() + input_password->text().length();
	length += 4 + 3;

	s = new QDataStream(sock);
	*s << (Q_INT8)type_admin;
	*s << (Q_INT8)opcode;
	*s << (Q_INT16)length;

	*s << (Q_INT8)command_authorize;
	s->writeRawBytes(input_username->text().latin1(), input_username->text().length());
	*s << (Q_INT8)0;
	s->writeRawBytes(input_password->text().latin1(), input_password->text().length());
	*s << (Q_INT8)0;
}

void Connection::slotError()
{
	bar->reset();
	KMessageBox::error(this, i18n("Could not connect to that host."), i18n("Connection error."));
}

void Connection::slotInput()
{
	const int type_admin = 16;
	const int command_authorizeanswer = 10;
	const int option_authorized = 3;
	const int option_unauthorized = 4;
	unsigned char opcode;
	unsigned char type;
	short length;
	int error = 0;

	bar->setProgress(3);
	if(sock->bytesAvailable() < 6) return;
	bar->setProgress(4);

	*s >> type;
	if(type != type_admin) error = 1;
	else
	{
		*s >> opcode;
		*s >> length;
		length = ((length & 0x00FF) << 8) + ((length & 0xFF00) >> 8);
		if(length != 6) error = 1;
		else
		{
			*s >> opcode;
			if(opcode != command_authorizeanswer) error = 1;
			else
			{
				*s >> opcode;
				delete s;
				if(opcode == option_authorized)
				{
					KMessageBox::information(this, i18n("Authorization successful."), i18n("Authorization"));
					//sock->disconnect();
					emit signalLogin(sock);
					accept();
				}
				else if(opcode == option_unauthorized)
				{
					KMessageBox::sorry(this, i18n("Authorization could not be granted."), i18n("Authorization"));
					//delete sock;
					sock->disconnect();
				}
				else error = 1;
			}
		}
	}

	if(error) KMessageBox::error(this, i18n("Unknown authorization scheme."), i18n("Authorization error."));
	bar->reset();
}

