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
// KGGZAbout: Display a nice window which honours all the GGZ developers in space. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

#ifndef KGGZ_ABOUT_H
#define KGGZ_ABOUT_H

// Qt includes
#include <qwidget.h>

// Forward declarations
class QFrame;

// Show an animated about dialog
class KGGZAbout : public QWidget
{
	Q_OBJECT
	public:
		// Constructor
		KGGZAbout(QWidget *parent = NULL, const char *name = NULL);
		// Destructor
		~KGGZAbout();

	protected slots:
		// Animate floating text
		void timerEvent(QTimerEvent *e);
		// Let the menu handle drawing on its own
		void paintEvent(QPaintEvent *e);
		// Handle window resize events
		void resizeEvent(QResizeEvent *e);

	private:
		// Internal string length measurement
		QString measure(QString s);
		// The animation frame
		QFrame *m_frame;
		// Flag which indicates the need for a repaint
		int m_repaint;
		// The dynamically adjusted font and its size
		QFont m_font;
};

#endif
