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
// KGGZWorkspace: Hold together chat, table views, player views and the KGGZ logo. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

#ifndef KGGZ_WORKSPACE_H
#define KGGZ_WORKSPACE_H

// KGGZ includes
#include "KGGZChat.h"
#include "KGGZTables.h"
#include "KGGZUsers.h"
#include "KGGZLogo.h"

// Qt includes
#include <qwidget.h>
#include <qevent.h>
#include <qsplitter.h>
#include <qframe.h>

// Provide a common space for various widgets
class KGGZWorkspace : public QWidget
{
	Q_OBJECT
	public:
		// Constructor
		KGGZWorkspace(QWidget *parent = NULL, const char *name = NULL);
		// Destructor
		~KGGZWorkspace();
		// Returns pointer to chat widget
		KGGZChat *widgetChat();
		// Returns pointer to tables widget
		KGGZTables *widgetTables();
		// Returns pointer to players widget
		KGGZUsers *widgetUsers();
		// Returns pointer to logo widget
		KGGZLogo *widgetLogo();
		// Hides/shows upper bar
		void hideBar(int hide);

	protected:
		// Handle resizing on its own
		void resizeEvent(QResizeEvent *e);

	private:
		// The chat widget
		KGGZChat *m_chat;
		// Tables
		KGGZTables *m_tables;
		// Players
		KGGZUsers *m_userlist;
		// Main widget for geometry management
		QSplitter *m_vsbox;
		// Contains the games logos
		KGGZLogo *m_logo;
		// Little hack around qt bug
		int m_firstresize;
		// Another geometry widget
		QSplitter *m_hsbox;
		// widget containing logo and tables
		QWidget *m_widget;
};

#endif

