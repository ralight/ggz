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
// KGGZSplash: Show a splash screen on startup of KGGZ. This is overload but cool. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

// Header file
#include "KGGZSplash.h"

// Qt includes
#include <qlayout.h>

KGGZSplash::KGGZSplash(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox;

	setBackgroundColor(QColor(255, 255, 255));

	m_splash = new KGGZSplashScreen(this);

	vbox = new QVBoxLayout(this, 0);
	vbox->add(m_splash);
}

KGGZSplash::~KGGZSplash()
{
}


