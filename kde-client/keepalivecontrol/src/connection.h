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

#ifndef KEEPALIVE_CONTROL_CONNECTION_H
#define KEEPALIVE_CONTROL_CONNECTION_H

#include <qdialog.h>

class QLineEdit;
class QProgressBar;
class QSocket;
class QDataStream;

class Connection : public QDialog
{
	Q_OBJECT
	public:
		Connection(QWidget *parent = NULL, const char *name = NULL);
		~Connection();

	signals:
		void signalLogin(QSocket *socket);

	public slots:
		void slotConnect();
		void slotConnected();
		void slotError();
		void slotInput();

	private:
		QLineEdit *input_server, *input_username, *input_password;
		QProgressBar *bar;
		QSocket *sock;
		QDataStream *s;
};

#endif

