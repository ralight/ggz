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
// KGGZChatLine: Replace the simple line input with one which autocompletes names. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

#ifndef KGGZ_CHATLINE_H
#define KGGZ_CHATLINE_H

// Qt includes
#include <qlineedit.h>
#include <qevent.h>

// A QLineEdit extension
class KGGZChatLine : public QLineEdit
{
	Q_OBJECT
	public:
		// Constructor
		KGGZChatLine(QWidget *parent = NULL, char *name = NULL);
		// Destructor
		~KGGZChatLine();

		// Add a player to the internal completion list
		void addPlayer(char *name);
		// Remove a player
		void removePlayer(char *name);
		// Cleanup the players list
		void removeAll();

	protected:
		// Catch all user key events
		void keyPressEvent(QKeyEvent *e);
		// Catch dumb focus loss
		void focusOutEvent(QFocusEvent *e);

	private:
		// Internal: Autocomplete at current position
		void autocomplete(QString pattern);

		// The list of names for possible autocompletions
		QStringList m_list;
};

#endif

