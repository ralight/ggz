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
// KGGZBrowser: Embedded HTML browser component for help, games, updates and news. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

#ifndef KGGZ_BROWSER_H
#define KGGZ_BROWSER_H

// KDE includes
#include <khtml_part.h>
#include <kurl.h>
#include <kparts/browserextension.h>

// Qt includes
#include <qwidget.h>
#include <qlineedit.h>

// Browser component for KGGZ
class KGGZBrowser : public QWidget
{
	Q_OBJECT
	public:
		// Constructor
		KGGZBrowser(QWidget *parent = NULL, const char *name = NULL);
		// Destructor
		~KGGZBrowser();

	public slots:
		// Invoked by clicking on an URL
		void slotRequest(const KURL& url, const KParts::URLArgs& args);
		// Invoked by typing an URL
		void slotRequestExplicit();
		// Invoked by pressing the home button
		void slotRequestHome();

	private:
		// The KPart for the browser
		KHTMLPart *m_part;
		// User input field
		QLineEdit *m_edit;
};

#endif

