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
// KGGZSelector: Let's the player select from the available game client frontends. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

#ifndef KGGZ_SELECTOR_H
#define KGGZ_SELECTOR_H

// Qt includes
#include <qwidget.h>
#include <qcombobox.h>
#include <qmap.h>

// A widget containing all available game frontends
class KGGZSelector : public QWidget
{
	Q_OBJECT
	public:
		// Constructor
		KGGZSelector(QWidget *parent = NULL, const char *name = NULL);
		// Destructor
		~KGGZSelector();
		// Adds a frontend
		void addFrontend(const char *frontend, int position);

	public slots:
		// User pressed ok
		void slotAccept();

	signals:
		// Request information window
		void signalFrontend(int frontend);

	private:
		// A frontend selection box
		QComboBox *m_box;
		QMap<int, int> m_positions;
};

#endif
