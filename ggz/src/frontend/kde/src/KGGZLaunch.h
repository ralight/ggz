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
// KGGZLaunch: Shows a dialog for game launching which allows changing parameters. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

#ifndef KGGZ_LAUNCH_H
#define KGGZ_LAUNCH_H

// KDE includes
#include <klistview.h>

// Qt includes
#include <qcstring.h>
#include <qwidget.h>

// Forward declarations
class KGGZInput;
class QPushButton;
class QLineEdit;
class QPopupMenu;
class QLabel;
class QSlider;

// Show the launch dialog
class KGGZLaunch : public QWidget
{
	Q_OBJECT
	public:
		// Constructor
		KGGZLaunch(QWidget *parent = NULL, const char *name = NULL);
		// Destructor
		~KGGZLaunch();

		// Initialize the dialog with the given parameters
		void initLauncher(char *playername, int maxplayers, int maxbots);
		// Return the customizable room description
		QString description();
		// Return the number of seats wanted
		int seats();
		// For each seat, return the set type
		int seatType(int seat);
		// Enable/disable seat combinations
		void setSeatAssignment(int seat, int enabled);
		// Return the reservation name
		QString reservation(int seat);

		// All possible seat types
		enum SeatTypes
		{
			seatopen = -1,
			seatreserved = -3,
			seatbot = -2,
			seatplayer = -5,
			seatunused = -4,
			seatspectator = -6,
			seatunknown = -7
		};

	public slots:
		// User clicks on a seat entry
		void slotSelected(QListViewItem *selected, const QPoint&, int column);
		// User has selected a seat type from a popup menu
		void slotActivated(int id);
		// All settings are accepted
		void slotAccepted();
		// The number of seats has changed
		void slotChanged(int value);
		// Reservation name received
		void slotReservation(const char *player);

	signals:
		// Emitted if table is to be launched
		void signalLaunch();

	private:
		// Set the type of the given seat
		void setSeatType(int seat, int seattype);
		// Return the i18n'd name of a seat type
		QString typeName(int seattype);

		// Widget holding all seat entries
		KListView *m_listbox;
		// Allows editing of the number of players
		QSlider *m_slider;
		// Menu which lets user set the type of a seat
		QPopupMenu *m_popup;
		// Widget for the table name
		QLineEdit *m_edit;
		// Holds the player name
		char *m_playername;
		// Holds the current configuration
		QByteArray *m_array;
		// Hold the combination matrix
		QByteArray *m_assignment;
		// OK button
		QPushButton *m_ok;
		// Description of this dialog
		QLabel *m_label;
		// Holds the maximum number of bots
		int m_maxbots;
		// Holds the maximum number of spectators
		int m_maxspectators;
		// Holds the current number of players, spectators and bots
		int m_curplayers;
		// Current number of bots
		int m_curbots;
		// Reservation input dialog
		KGGZInput *m_input;
};

#endif
