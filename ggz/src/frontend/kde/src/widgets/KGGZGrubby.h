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
// KGGZGrubby: Show a visual grubby dialog which allows one to talk with this guy. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

#ifndef KGGZ_GRUBBY_H
#define KGGZ_GRUBBY_H

// Qt includes
#include <qwidget.h>
#include <qcombobox.h>
#include <qlineedit.h>

// Displays a grubby talk dialog window.
class KGGZGrubby : public QWidget
{
	Q_OBJECT
	public:
		// Constructor
		KGGZGrubby(QWidget *parent = NULL, const char *name = NULL);
		// Destructor
		~KGGZGrubby();

		enum Actions
		{
			actionseen,
			actionmessages,
			actionalertadd,
			actionteach,
			actionwhois,
			actionhelp,
			actionabout,
			actionbye
		};

		// Clear the list of players
		void removeAll();
		// Add a player to the list
		void addPlayer(const char *player);

	public slots:
		// Check requested grubby action
		void slotInvoke();
		// Handle combobox updates
		void slotActivated(int index);

	signals:
		// Invoke a grubby action
		void signalAction(const char *grubby, const char *argument, int id);

	private:
		int m_lastaction;
		QComboBox *m_player;
		QLineEdit *m_ed;
};

#endif

