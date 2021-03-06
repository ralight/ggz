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

#ifndef KEEPALIVE_CONTROL_CREATE_H
#define KEEPALIVE_CONTROL_CREATE_H

#include <qdialog.h>

class QLineEdit;

class Create : public QDialog
{
	Q_OBJECT
	public:
		Create(QWidget *parent = NULL, const char *name = NULL);
		~Create();
		QString world();

	signals:
		void signalWorld(QString world);

	public slots:
		void slotWorld();

	private:
		QLineEdit *input_world;
};

#endif

