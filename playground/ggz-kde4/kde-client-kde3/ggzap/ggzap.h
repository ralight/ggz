// GGZap - GGZ quick start application for the KDE panel
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

#ifndef GGZAP_H
#define GGZAP_H

// Qt includes
#include <qwidget.h>
#include <qevent.h>

// Forward declarations
class GGZapHandler;
class GGZapSkin;
class QLabel;

// The main window for GGZap
class GGZap : public QObject
{
	Q_OBJECT
	public:
		GGZap(QWidget *parent = NULL, const char *name = NULL);
		~GGZap();

		void setModule(QString modulename);
		void setFrontend(QString frontendtype);

		void launch();

		QWidget *gui();

	signals:
		void signalMenu(int id);

	public slots:
		void slotState(int state);
		void slotLaunch(QString name, QString frontend);
		void slotCancel();

/*	protected:
		void timerEvent(QTimerEvent *e);*/

	private:
		GGZapHandler *m_handler;
		GGZapSkin *m_gui;
		int m_autolaunch;
};

#endif

