/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//    KGGZ - The KDE client for the GGZ Gaming Zone - Version 0.0.4                //
//    Copyright (C) 2000, 2001 Josef Spillner - dr_maux@users.sourceforge.net      //
//    The MindX Open Source Project - http://mindx.sourceforge.net                 //
//    Published under GNU GPL conditions - view COPYING for details                //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//    This program is free software; you can redistribute it and/or modify         //
//    it under the terms of the GNU General Public License as published by         //
//    the Free Software Foundation; either version 2 of the License, or            //
//    (at your option) any later version.                                          //
//                                                                                 //
//    This program is distributed in the hope that it will be useful,              //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of               //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                //
//    GNU General Public License for more details.                                 //
//                                                                                 //
//    You should have received a copy of the GNU General Public License            //
//    along with this program; if not, write to the Free Software                  //
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA    //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
// KGGZConnect: Display a connection dialog which allows in-place server addition. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

#ifndef KGGZ_CONNECT_H
#define KGGZ_CONNECT_H

// Qt includes
#include <qwidget.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>

// A comprehensive connection dialog
class KGGZConnect : public QWidget
{
	Q_OBJECT
	public:
		// Constructor
		KGGZConnect(QWidget *parent = NULL, char *name = NULL);
		// Destructor
		~KGGZConnect();

	protected slots:
		// Accept the current input
		void slotAccept();
		// Load a specified connection profile
		void slotLoadProfile(int profile);
		// Save current profile
		void slotSaveProfile();
		// Toggle between login modes
		void slotModes(int loginmode);
		// Invoke configuration window
		void slotInvoke();

	signals:
		// Emitted if connection is wanted
		void signalConnect(const char *host, int port, const char *username, const char *password, int type, int server);

	private:
		// The login mode (guest, with password, first time)
		int m_loginmode;
		// Whether to start server locally or not
		QCheckBox *option_server;
		// Lists all known connection profiles
		QComboBox *profile_select;
		// Allows in-place editing of profiles
		QPushButton *profile_new, *profile_delete;
		// Input fields
		QLineEdit *input_host, *input_port, *input_name, *input_password;
		// Holds connection state
		int m_connected;
		// Container widget for all modes
		QButtonGroup *group_mode;
};

#endif
