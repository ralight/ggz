// ShadowBridge - Game developer tool to visualize network protocols
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

#ifndef SHADOW_CLIENT_H
#define SHADOW_CLIENT_H

// Qt includes
#include <qwidget.h>

// Forward declarations
class KLineEdit;

// Dialog to add an arbitrary (non-GGZ) client to shadow bridge
class ShadowClient : public QWidget
{
	Q_OBJECT
	public:
		ShadowClient(QWidget *parent = NULL, const char *name = NULL);
		~ShadowClient();

	public slots:
		void slotPressed();

	signals:
		void signalClient(const QString& exec, const QString& fdin, const QString& fdout);

	private:
		KLineEdit *edit_exec, *edit_fdin, *edit_fdout;
};

#endif

