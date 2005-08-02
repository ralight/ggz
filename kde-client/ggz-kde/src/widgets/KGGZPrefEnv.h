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
// KGGZPrefEnv: Ask users for paths to programs and global configuration settings. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

#ifndef KGGZ_PREF_ENV_H
#define KGGZ_PREF_ENV_H

// Qt includes
#include <qwidget.h>

// Forward declarations
class QCheckBox;
class QLineEdit;
class QComboBox;

// KGGZPrefEnv: Global configuration
class KGGZPrefEnv : public QWidget
{
	Q_OBJECT
		public:
			// Constructor
			KGGZPrefEnv(QWidget *parent = NULL, const char *name = NULL);
			// Destructor
			~KGGZPrefEnv();

		public slots:
			// Accept changes
			void slotAccept();

		signals:
			// Configuration saved
			void signalAccepted();

		private:
			// Loads the configuration
			void loadSettings();
			// Fill country selection box
			void loadCountries();

			// The local ggzd binary
			QLineEdit *m_server;
			// Whether to show connection dialog or not
			QCheckBox *m_startup;
			// Enable chat logging
			QCheckBox *m_chatlog;
			// Time stamps in chat window
			QCheckBox *m_timestamps;
			// Enable text-to-speech synth of chat
			QCheckBox *m_speech;
			// Display MOTD upon login
			QCheckBox *m_motd;
			// Player full name
			QLineEdit *m_playername;
			// Player homepage
			QLineEdit *m_homepage;
			// Player email
			QLineEdit *m_email;
			// Player country
			QComboBox *countrybox;
};

#endif

