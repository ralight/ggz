/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//    KGGZ - The KDE client for the GGZ Gaming Zone - Version 0.0.5pre             //
//    Copyright (C) 2000 - 2002 Josef Spillner - dr_maux@users.sourceforge.net     //
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

// KGGZ includes
#include "KGGZInput.h"
#include "KGGZMeta.h"
#include "KGGZCommon.h"

// KDE includes
#include <dnssd/remoteservice.h>

// Qt includes
#include <qwidget.h>

// Forward declarations
class QSocket;
class QButtonGroup;
class QPushButton;
class QCheckBox;
class QComboBox;
class QLineEdit;

// A comprehensive connection dialog
class KGGZConnect : public QWidget
{
	Q_OBJECT
	public:
		// Constructor
		KGGZConnect(QWidget *parent = NULL, const char *name = NULL);
		// Destructor
		~KGGZConnect();

		// start server locally?
		int optionServer();
		// secure connection?
		int optionSecure();

	//protected:
		// Widget is about to be shown
		void showEvent(QShowEvent *e);

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
		// Empty all fields and create new profile
		void slotProfileNew();
		// Delete the current profile
		void slotProfileDelete();
		// Process information about new profile
		void slotProfileProcess(QString identifier);
		// Show meta server list window
		void slotProfileMeta();
		// Get meta server data
		void slotProfileMetaProcess(QString host, QString port);
		// Show pane or not
		void slotPane();
		// Write to meta server
		void slotWrite();
		// Read from meta server
		void slotRead();
		// Data from the DNSSD implementation
		void slotService(DNSSD::RemoteService::Ptr ptr);
		// Data from DNSSD finished
		void slotServiceFinished();
		// Metaserver errors
		void slotError(int code);

	signals:
		// Emitted if connection is wanted
		void signalConnect(QString host, int port, QString username, QString password, int type);

	private:
		// add (1) or delete (2) a server from the configuration list
		void modifyServerList(QString server, int mode);
		// start a zeroconf query
		void zeroconfQuery();

		// Login modes
		enum LoginModes
		{
			mode_normal,
			mode_guest,
			mode_firsttime
		};

		// The login mode (guest, with password, first time)
		int m_loginmode;
		// Whether to start server locally or not
		QCheckBox *option_server;
		// Whether to setup a secure connection or not
		QCheckBox *option_tls;
		// Lists all known connection profiles
		QComboBox *profile_select;
		// Allows in-place editing of profiles
		QPushButton *profile_new, *profile_delete, *profile_edit;
		// Invokes server selection list
		QPushButton *button_select;
		// Input fields
		QLineEdit *input_host, *input_port, *input_name, *input_password;
		// Holds connection state
		int m_connected;
		// Container widget for all modes
		QButtonGroup *group_mode;
		// Little dialog asking for server id
		KGGZInput *m_input;
		// Dialog to select from a list of meta servers
		KGGZMeta *m_meta;
		// Pointer to current profile (previous one on change)
		QString m_current;
		// Indicated whether to save current profile when loading new one or not
		int m_nosafe;
		// Pane holding server settings
		QWidget *m_pane;
		// Meta server connection
		QSocket *m_sock;
		// Buttons
		QPushButton *button_ok, *button_cancel;
};

#endif

