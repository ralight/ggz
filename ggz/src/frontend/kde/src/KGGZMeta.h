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
// KGGZMeta: Present a list of GGZ servers which are fetched from metaserver list. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

#ifndef KGGZ_META_H
#define KGGZ_META_H

// Qt includes
#include <qwidget.h>

// Forward definitions
class QSocket;
class KListView;
class QListViewItem;
class QPushButton;

// Metaserver-based GGZ server list
class KGGZMeta : public QWidget
{
	Q_OBJECT
	public:
		// Constructor
		KGGZMeta(QWidget *parent = NULL, const char *name = NULL);
		// Destructor
		~KGGZMeta();
		// Load available meta servers
		void load();

	signals:
		// Emit data
		void signalData(QString host, QString port);

	protected slots:
		// Accept the current input
		void slotAccept();
		// Execute query
		void slotConnected();
		// Read query results
		void slotRead();
		// Get selection
		void slotSelection(QListViewItem *item);

	private:
		// Socket for meta server connections
		QSocket *m_sock;
		// List view of meta servers
		KListView *m_view;
		// Dynamic button
		QPushButton *m_ok;
};

#endif

